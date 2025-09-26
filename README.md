# 🎶 Spotifind  

**Trabajo de:** Patricia Ulloa y Juan Mercade  
**Para:** Estructura de Datos  

Spotifind es una aplicación en **C** que permite manejar una **base de datos de canciones** cargada desde un archivo **CSV**.  
El programa utiliza **HashMap** para organizar las canciones por **ID, Artista, Género y Tempo**, además de ofrecer funcionalidades para crear y gestionar **playlists**.  

---

## Funcionalidades previstas  

El programa ofrece un menú con las siguientes opciones:  

1. **Buscar por ID**  
   Permite encontrar una canción específica usando su identificador único.  

2. **Buscar por Artista**  
   Muestra todas las canciones asociadas a un artista dado.  

3. **Buscar por Tempo**  
   Filtra canciones por categorías de tempo (lento / moderado / rápido).  

4. **Buscar por Género**  
   Lista todas las canciones de un género musical específico.  

5. **Crear Playlist**  
   Permite crear una nueva playlist con nombre personalizado.  

6. **Agregar Canción a Playlist**  
   Añade una canción ya existente a una playlist seleccionada.  

7. **Mostrar Playlist**  
   Muestra todas las canciones contenidas en una playlist.  

8. **Salir**  
   Termina la ejecución del programa.  

---

## 🚧 Estado actual del proyecto  

- Lectura del archivo CSV: En progreso (`leer_linea_csv` aún no implementada).  
- Inserción de canciones en mapas (`songsByID`, `songsByArtist`, `songsByGenre`): ✅  
- Menú principal: ✅ (todas las opciones aparecen).  
- Funciones de búsqueda: (no implementadas aún).  
  - `showSongsID`  
  - `showSongsArtist`  
  - `showSongsByGenre`  
  - `showSongsTempo`  
- Playlists:  
  - `createPlaylist`: ✅  
  - `addSongPlaylist`:  
  - `showPlaylist`:   

---

## ⚙️ Compilación y Ejecución  

Para **compilar** el programa:  
```bash
gcc -o spotifind spotifind.c hashmap.c list.c -Wall -std=c99

Para **ejecutar**:
```bash
./spotifind