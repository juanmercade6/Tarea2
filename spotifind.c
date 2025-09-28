#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "list.h"
#include "hashmap.h"

typedef struct {
    char *id;
    char *artist_name;
    char *album_name;
    char *track_name;
    float tempo;
    char *track_genre;
} Song;

typedef struct {
    char *playlist_name;
    List *songs;
} Playlist;

HashMap *stringPool;
HashMap *songsByID;
HashMap *songsByArtist;
HashMap *songsByGenre;
HashMap *playlists;
List *allSongs;

char *strdup_c(const char *src) {
    char *copy = (char *)malloc(strlen(src) + 1);
    strcpy(copy, src);
    return copy;
}

char *getPooledString(const char *str) {
    Pair *p = searchMap(stringPool, (char *)str);
    if (p) return (char *)p->value;
    char *copy = strdup_c(str);
    insertMap(stringPool, copy, copy);
    return copy;
}

// CSV parser con soporte para comillas
char **leer_linea_csv(FILE *archivo, char separador) {
    static char buffer[4096];
    if (!fgets(buffer, sizeof(buffer), archivo)) return NULL;

    char **campos = malloc(50 * sizeof(char *));
    int i = 0, dentroComillas = 0;
    char *inicio = buffer, *ptr = buffer;

    while (*ptr) {
        if (*ptr == '\"') {
            dentroComillas = !dentroComillas;
        } else if (*ptr == separador && !dentroComillas) {
            *ptr = '\0';
            campos[i++] = strdup_c(inicio);
            inicio = ptr + 1;
        } else if (*ptr == '\n' || *ptr == '\r') {
            *ptr = '\0';
            break;
        }
        ptr++;
    }
    campos[i++] = strdup_c(inicio);
    campos[i] = NULL;
    return campos;
}

Song *createSong(char *id, char *artist_name, char *album_name,
                 char *track_name, float tempo, char *track_genre) {
    Song *s = malloc(sizeof(Song));
    s->id = getPooledString(id);
    s->artist_name = getPooledString(artist_name);
    s->album_name = getPooledString(album_name);
    s->track_name = getPooledString(track_name);
    s->tempo = tempo;
    s->track_genre = getPooledString(track_genre);
    return s;
}

Song *readSong(char **campos) {
    return createSong(campos[0], campos[2], campos[3], campos[4],
                      atof(campos[18]), campos[20]);
}

void addSong(Song *s) {
    insertMap(songsByID, s->id, s);
    list_pushBack(allSongs, s);

    char *tok = strtok(s->artist_name, ";");
    while (tok != NULL) {
        char *artist = getPooledString(tok);
        Pair *p = searchMap(songsByArtist, artist);
        List *lst = p ? p->value : createList();
        if (!p) insertMap(songsByArtist, artist, lst);
        list_pushBack(lst, s);
        tok = strtok(NULL, ";");
    }

    Pair *pG = searchMap(songsByGenre, s->track_genre);
    List *lstG = pG ? pG->value : createList();
    if (!pG) insertMap(songsByGenre, s->track_genre, lstG);
    list_pushBack(lstG, s);
}

void showSongsTempo(char *category) {
    printf("Canciones con tempo %s:\n", category);
    for (Song *s = list_first(allSongs); s != NULL; s = list_next(allSongs)) {
        if (strcmp(category, "lento") == 0 && s->tempo < 80)
            printf("%s - %s (%.1f)\n", s->artist_name, s->track_name, s->tempo);
        else if (strcmp(category, "moderado") == 0 && s->tempo >= 80 && s->tempo <= 120)
            printf("%s - %s (%.1f)\n", s->artist_name, s->track_name, s->tempo);
        else if (strcmp(category, "rapido") == 0 && s->tempo > 120)
            printf("%s - %s (%.1f)\n", s->artist_name, s->track_name, s->tempo);
    }
}

void addSongPlaylist(char *playlistN, char *songID) {
    Pair *pPL = searchMap(playlists, playlistN);
    if (!pPL) { printf("Playlist no encontrada.\n"); return; }
    Pair *pS = searchMap(songsByID, songID);
    if (!pS) { printf("Cancion con ID %s no encontrada.\n", songID); return; }
    Playlist *pl = pPL->value;
    list_pushBack(pl->songs, pS->value);
    printf("Cancion agregada a playlist '%s'.\n", playlistN);
}

void showPlaylist(char *name) {
    Pair *p = searchMap(playlists, name);
    if (!p) { printf("Playlist '%s' no existe.\n", name); return; }
    Playlist *pl = p->value;
    printf("Playlist: %s\n", name);
    for (Song *s = list_first(pl->songs); s != NULL; s = list_next(pl->songs)) {
        printf("%s - %s (Album: %s, Tempo: %.1f, Genero: %s)\n",
               s->artist_name, s->track_name, s->album_name,
               s->tempo, s->track_genre);
    }
}

void freeMemory() {
    // Liberar playlists
    Pair *p = firstMap(playlists);
    while (p) {
        Playlist *pl = p->value;
        free(pl->songs);
        free(pl);
        p = nextMap(playlists);
    }

    // Liberar canciones (sin strings, ya que están en el pool)
    Song *s = list_first(allSongs);
    while (s) {
        free(s);
        s = list_next(allSongs);
    }

    // Liberar strings únicos del pool
    p = firstMap(stringPool);
    while (p) {
        free(p->value);
        p = nextMap(stringPool);
    }
}

int main() {
    stringPool = createMap(10000);
    songsByID = createMap(10000);
    songsByArtist = createMap(10000);
    songsByGenre = createMap(10000);
    playlists = createMap(100);
    allSongs = createList();

    FILE *fp = fopen("song_dataset_.csv", "r");
    if (!fp) { printf("No se pudo abrir el archivo CSV.\n"); return 1; }

    char **campos = leer_linea_csv(fp, ','); // Saltar header
    while ((campos = leer_linea_csv(fp, ',')) != NULL) {
        Song *s = readSong(campos);
        addSong(s);
        for (int i = 0; campos[i] != NULL; i++) free(campos[i]);
        free(campos);
    }
    fclose(fp);

    int opcion;
    char buffer[256], buffer2[256];

    do {
        printf("\n--- MENU ---\n");
        printf("1. Buscar cancion por ID\n");
        printf("2. Buscar canciones por artista\n");
        printf("3. Buscar canciones por genero\n");
        printf("4. Mostrar canciones por tempo\n");
        printf("5. Crear playlist\n");
        printf("6. Agregar cancion a playlist\n");
        printf("7. Mostrar playlist\n");
        printf("8. Salir\n");
        printf("Seleccione: ");
        scanf("%d", &opcion);
        getchar();

        switch (opcion) {
            case 1: {
                printf("Ingrese ID: ");
                fgets(buffer, 256, stdin); buffer[strcspn(buffer, "\n")] = 0;
                Pair *p = searchMap(songsByID, buffer);
                if (p) {
                    Song *s = p->value;
                    printf("%s - %s (%s)\n", s->artist_name, s->track_name, s->album_name);
                } else printf("No encontrada.\n");
            } break;

            case 2: {
                printf("Ingrese artista: ");
                fgets(buffer, 256, stdin); buffer[strcspn(buffer, "\n")] = 0;
                Pair *p = searchMap(songsByArtist, buffer);
                if (p) {
                    for (Song *s = list_first(p->value); s; s = list_next(p->value))
                        printf("%s - %s\n", s->artist_name, s->track_name);
                } else printf("No hay canciones de ese artista.\n");
            } break;

            case 3: {
                printf("Ingrese genero: ");
                fgets(buffer, 256, stdin); buffer[strcspn(buffer, "\n")] = 0;
                Pair *p = searchMap(songsByGenre, buffer);
                if (p) {
                    for (Song *s = list_first(p->value); s; s = list_next(p->value))
                        printf("%s - %s\n", s->artist_name, s->track_name);
                } else printf("No hay canciones de ese genero.\n");
            } break;

            case 4: {
                printf("Ingrese categoria (lento/moderado/rapido): ");
                fgets(buffer, 256, stdin); buffer[strcspn(buffer, "\n")] = 0;
                showSongsTempo(buffer);
            } break;

            case 5: {
                printf("Ingrese nombre de playlist: ");
                fgets(buffer, 256, stdin); buffer[strcspn(buffer, "\n")] = 0;
                Playlist *pl = malloc(sizeof(Playlist));
                pl->playlist_name = getPooledString(buffer);
                pl->songs = createList();
                insertMap(playlists, pl->playlist_name, pl);
                printf("Playlist creada.\n");
            } break;

            case 6: {
                printf("Ingrese nombre de playlist: ");
                fgets(buffer, 256, stdin); buffer[strcspn(buffer, "\n")] = 0;
                printf("Ingrese ID de cancion: ");
                fgets(buffer2, 256, stdin); buffer2[strcspn(buffer2, "\n")] = 0;
                addSongPlaylist(buffer, buffer2);
            } break;

            case 7: {
                printf("Ingrese nombre de playlist: ");
                fgets(buffer, 256, stdin); buffer[strcspn(buffer, "\n")] = 0;
                showPlaylist(buffer);
            } break;
        }
    } while (opcion != 8);

    freeMemory();
    return 0;
}
