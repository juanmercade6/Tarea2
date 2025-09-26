#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
#include "hashmap.h"


typedef struct HashMap HashMap;
int enlarge_called=0;

struct HashMap {
    Pair ** buckets;
    long size; //cantidad de datos/pairs en la tabla
    long capacity; //capacidad de la tabla
    long current; //indice del ultimo dato accedido
};

Pair * createPair( char * key,  void * value) {
    Pair * new = (Pair *)malloc(sizeof(Pair));
    new->key = key;
    new->value = value;
    return new;
}

long hash( char * key, long capacity) {
    unsigned long hash = 0;
    char * ptr;
    for (ptr = key; *ptr != '\0'; ptr++) {
        hash += hash*32 + tolower(*ptr);
    }
    return hash%capacity;
}

int is_equal(void* key1, void* key2){
    if(key1==NULL || key2==NULL) return 0;
    if(strcmp((char*)key1,(char*)key2) == 0) return 1;
    return 0;
}

//Funcion que inserta un nuevo par (key, value) en el mapa
/*Como: La funcion calcula la posición usando la funcion hash y coloca el par allí.
Si la posición esta ocupada, avanza linealmente hasta encontrar un lugar vacio y, si la clave
ya existe, no inserta nada.
*/
void insertMap(HashMap * map, char * key, void * value) {
    if(map == NULL || key == NULL){
        return;
    }
    long indice = hash(key, map-> capacity);

    while(map -> buckets[indice] != NULL && map-> buckets[indice]-> key != NULL){
        if(strcmp(map-> buckets[indice] -> key, key) == 0){
            return;
        }
        indice = (indice + 1) % map -> capacity;
    }
    map-> buckets[indice] = createPair(key, value);
    map -> size++;
    map-> current = indice;
}

//Funcion que duplica la capacidad del mapa y reubica todos los elementos
/*Como: La funcion crea un nuevo arreglo más grande, recalcula las posiciones de
todas las claves y reubica los pares
*/
void enlarge(HashMap * map) {
    Pair** bucketsA = map -> buckets;
    long capacidadA = map -> capacity;

    map -> capacity *= 2;
    map -> buckets = (Pair**) calloc(map -> capacity, sizeof(Pair*));
    map ->size = 0;

    for(long i = 0; i < capacidadA; i++){
        Pair* par = bucketsA[i];
        if (par != NULL && par -> key != NULL){
            long posicion = hash(par -> key, map -> capacity);
            if(posicion < 0){
                posicion += map-> capacity;
            }
            while(map -> buckets[posicion] != NULL){
                posicion = (posicion + 1) % map -> capacity;
            }
            map -> buckets[posicion] = par;
            map -> size++;
        }
    }
    free(bucketsA);
    enlarge_called = 1; //no borrar (testing purposes)
}

//Funcion que crea e inicializa un nuevo hashMap vacio
/* Como: La funcion reserva memoria, inicializa los buckets en NULL y prepara la 
estructura vacia
*/
HashMap * createMap(long capacity) {
    HashMap* map = (HashMap*) malloc(sizeof(HashMap));
    if(map == NULL){
        return NULL;
    }
    map -> buckets = (Pair**) malloc(sizeof(Pair*) * capacity);
    if(map-> buckets == NULL){
        free(map);
        return NULL;
    }

    for(long i = 0; i < capacity; i++){
        map -> buckets[i] = NULL;
    }

    map-> capacity = capacity;
    map-> size = 0;
    map-> current = -1;

    return map;

}

//Funcion que elimina una clave (key) del mapa, marcandola como borrada
/*Como: La funcion busca la clave usando hash y avanzando linealmente. Cuando encuentra 
la key buscada la marca como NULL para indicar que fue borrada sin liberar el valor asociado
*/
void eraseMap(HashMap * map,  char * key) { 
    if(map == NULL || key == NULL){
        return;
    }

    long indice = hash(key, map -> capacity);
    long inicio = indice;

    while(map -> buckets[indice] != NULL){
        if(map -> buckets[indice] -> key != NULL && strcmp(map -> buckets[indice] -> key, key) == 0){
            map -> buckets[indice] -> key = NULL;
            map -> size--;
            map -> current = indice;
        }
        indice = (indice + 1) % map -> capacity;
        if(indice == inicio){
            break;
        }
    }
}

//Funcion que busca una clave y, devuelve su par si  es que existe
/*Como: La funcion calcula la posicion inicial con hash y recorre de manera lineal hasta
encontrar la clave. Devuelve el par si lo encuentra y NULL si no existe.
*/
Pair * searchMap(HashMap * map,  char * key) {  
    if(map == NULL || key == NULL){
        return NULL;
    }   
    long indice = hash(key, map-> capacity);
    long inicio = indice;
    while(map -> buckets[indice] != NULL){
        if(map-> buckets[indice]-> key != NULL && strcmp(map-> buckets[indice]-> key, key) == 0){
            map-> current = indice;
            return map-> buckets[indice];
        }
        indice = (indice + 1) % map ->capacity;
        if(indice == inicio){
            break;
        }
    }
    return NULL; 
}

//Funcion que devuelve el primer par valido del mapa.
/*Como: Recorre desde el inicio de los buckets hasta encontrar la primera entrada valida y 
 la devuelve si existe, si no retorna NULL.
*/
Pair * firstMap(HashMap * map) {
    if(map == NULL){
        return NULL;
    }
    for(long i = 0; i < map-> capacity; i++){
        if(map -> buckets[i] != NULL && map -> buckets[i] -> key != NULL){
            map -> current = i;
            return map -> buckets[i];
        }
    }
    return NULL;
}

//Funcion que devuelve el siguiente par valido despues del actual
/*Como: La funcion continua el recorrido desde la posicion actual + 1, hasta encontrar
otro par valido, si es que existe, si no retorna NULL
*/
Pair * nextMap(HashMap * map) {
    if(map == NULL){
        return NULL;
    }
    
    long indice = map -> current + 1;
    for(long i = indice; i < map -> capacity; i++){
        if(map -> buckets[i] != NULL && map -> buckets[i] -> key != NULL){
            map -> current = i;
            return map -> buckets[i];
        }
    }
    return NULL;
}
