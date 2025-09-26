#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include "hashmap.h"
#include "list.h"


typedef struct{
    char *id;
    char *artist_name;
    char *album_name;
    char *track_name;
    float tempo;
    char *track_genre;
}Song;

typedef struct{
    char *playlist_name;
    List *songs;
}Playlist;


HashMap *songsByID;     //clave = id, valor = Song*
HashMap *songsByArtist;     //clave = artist, valor = List* (Song*)
HashMap *playlists;     //clave = nombre playlist, valor = Playlist*
HashMap *songsByGenre;  //clave = genre, valor = List*(Song*)
List *allSongs;     //Lista general de Song*



//Funcion que sirve para duplicar un string en memoria dinámica, asegurando que cada dato tenga su propia copia independiente.
char *strdup_c(char *c){
    char *copy = malloc(strlen(c) + 1);
    if(copy) strcpy(copy, c);
    return copy;
}


//Función para crear la canción
Song *createSong(char *id, char *artists, char *album, char *track, float tempo, char *genre){

    Song *s = malloc(sizeof(Song));
    s -> id = strdup_c(id);
    s -> artist_name = strdup_c(artists);
    s -> album_name = strdup_c(album);
    s -> track_name = strdup_c(track);
    s -> tempo = tempo;
    s -> track_genre = strdup_c(genre);

    return s;
}

//Función para agregar canciones a mapas
void addSong(Song *s){

    //Agregar a songsByID
    insertMap(songsByID, s -> id, s);

    //Agregar a lista general
    list_pushBack(allSongs, s);

    //Agregar por artista
    Pair *pArt = searchMap(songsByArtist, s-> artist_name);
    List *artistSongs;

    if(!pArt){
        artistSongs = list_create();
        insertMap(songsByArtist, strdup_c(s->artist_name), artistSongs);
    }
    else{
        artistSongs = pArt -> value;
    }

    list_pushBack(artistSongs, s);

    //Agregar por género
    Pair* pGenre = searchMap(songsByGenre, s-> track_genre);
    List *genreSongs;
    if(!pGenre){
        genreSongs = list_create();
        insertMap(songsByGenre, strdup_c(s -> track_genre), genreSongs);
    }
    else{
        genreSongs = pGenre -> value;
    }
    list_pushBack(genreSongs, s);
}

//Función que busca canciones por el artista y la/s muestra si es que encuentra
void showSongsArtist(char *artist){
    Pair *p = searchMap(songsByArtist, artist);
    if(!p){
        printf("No se encontro cancion del Artista %s\n", artist);
        return;
    }
    List *ls = p -> value;
    for(Song *s = list_first(ls); s != NULL; s = list_next(ls)){
        printf("%s - %s (Album: %s, Tempo: %.1f, Genre: %s)\n",
        s->artist_name, s->track_name, s->album_name, s->tempo, s->track_genre);
    }

    return;
}

//Función que busca una cancion por su ID y la muestra si es que la encuentra
void showSongsID(char *id){
    Pair *p = searchMap(songsByID, id);
    if(!p){
        printf("No se encontro cancion con ID %s\n", id);
        return;
    }

    Song *s = p -> value;
    printf("%s - %s (Album: %s, Tempo: %.1f, Genero: %s\n)",
    s -> artist_name, s-> track_name, s-> album_name, s-> tempo, s ->track_genre);
}

//Funcion que busca canciones por su genero y la/s muestra si es que encuentra
void showSongsByGenre(char *genre){
    Pair *p = searchMap(songsByGenre, genre);
    if(!p){
        printf("NO hay canciones del genero %s\n", genre);
        return;
    }

    List *ls = p -> value;
    for(Song *s = list_first(ls); s != NULL; s = list_next(ls)){
        printf("%s - %s (Album: %s, Tempo: %.1f)\n",
            s-> artist_name, s-> track_name, s-> album_name, s->tempo);
    }
}

//Función que busca canciones por su Tempo y la/s muestra si es que encuentra
void showSongsTempo(char *category){
    return;
}

//Función para crear la playlist
void createPlaylist(char *name){
    Playlist *p1 = malloc(sizeof(Playlist));
    p1 -> playlist_name = strdup_c(name);
    p1 -> songs = list_create();
    insertMap(playlists, p1 -> playlist_name, p1);
    printf("Playlist '%s' creada.\n", name);
}

//Función para agregar canciones a la playlist
void addSongPlaylist(char *playlistN, char* songID){
    return;
}

//Función que busca la playlist por su nombre y la muestra si es que existe
void showPlaylist(char *name){
    return;
}



char **leer_linea_csv(FILE *archivo, char separador){
    return NULL;
}

void readSong(){

    FILE *archivo = fopen("data/song_dataset_.csv", "r");
    if(!archivo){
        perror("Error al abrir el archivo");
        return;
    }

    char **campos = leer_linea_csv(archivo, ',');
    int k = 0;

    while((campos = leer_linea_csv(archivo, ',')) != NULL){
        k++;
        if(k > 1000) break; //limitado a 1000 canciones
        Song *s = createSong(campos[0], campos[2], campos[3], campos[4], atof(campos[18]), campos[20]);
        addSong(s);
    }
    fclose(archivo);
}


void menu(){
    int option;
    char buffer[100];
    while(1){
        printf("========================================\n");
        printf("        Base de Datos de Spotifind      \n");
        printf("========================================\n");
        printf("1) Buscar por ID\n");
        printf("2) Buscar por Artista\n");
        printf("3) Buscar por Tempo\n");
        printf("4) Buscar por Genero\n");
        printf("5) Crear Playlist\n");
        printf("6) Agregar Cancion a Playlist\n");
        printf("7) Mostrar Playlist\n");
        printf("8) Salir\n");
        printf("Ingrese su opcion: ");

        scanf("%d", &option);
        getchar();

        switch(option){
            case 1:
                printf("Ingrese ID:");
                fgets(buffer, 100, stdin); buffer[strcspn(buffer, "\n")] = 0;
                showSongsID(buffer);
                break;

            case 2:
                printf("Ingrese el artista: ");
                fgets(buffer, 100, stdin); buffer[strcspn(buffer, "\n")] = 0;
                showSongsArtist(buffer);
                break;
            case 3:
                printf("Ingrese tempo (lento / moderado /rapido): ");
                fgets(buffer, 100, stdin); buffer[strcspn(buffer, "\n")] = 0;
                showSongsTempo(buffer);
                break;
            case 4:
                printf("Ingrese genero: ");
                fgets(buffer, 100, stdin); buffer[strcspn(buffer, "\n")] = 0;
                showSongsByGenre(buffer);
                break;
            case 5:
                printf("Ingrese nombre de la playlist: ");
                fgets(buffer, 100, stdin); buffer[strcspn(buffer, "\n")] = 0;
                createPlaylist(buffer);
                break;
            case 6:
                {
                    char id[50];
                    printf("Ingrese ID de la cancion: ");
                    fgets(id, 50, stdin); id[strcspn(id, "\n")] = 0;

                    printf("Ingrese nombre de la playlist: ");
                    fgets(buffer, 100, stdin); buffer[strcspn(buffer, "\n")] = 0;

                    addSongPlaylist(buffer, id);
                }

                break;
            case 7:
                printf("Ingrese nombre de la playlist: ");
                fgets(buffer, 100, stdin); buffer[strcspn(buffer, "\n")] = 0;
                showPlaylist(buffer);
                break;
            case 8:
                printf("Saliendo...\n");
                return;
            default:
                printf("Opcion Invalida\n");
        }
    }
}


int main(){
    songsByID = createMap(2000);
    songsByArtist = createMap(2000);
    songsByGenre = createMap(2000);
    playlists = createMap(100);
    allSongs = list_create();

    readSong();
    menu();

    return 0;
}
