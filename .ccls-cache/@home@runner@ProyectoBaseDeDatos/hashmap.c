#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
#include "hashmap.h"
#include <stdbool.h>

typedef struct HashMap HashMap;
int enlarge_called=0;

struct HashMap {
    Par ** buckets;
    long size; //cantidad de datos/pairs en la tabla
    long capacity; //capacidad de la tabla
    long current; //indice del ultimo dato accedido
};

Par * createPair( char * key,  void * value) {
    Par * new = (Par *)calloc(1,sizeof(Par));
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


void insertMap(HashMap * map, char * key, void * value) {

  long newPos = hash(key, map->capacity);

  if( *(map->buckets+newPos) == NULL){

    *(map->buckets+newPos) = createPair(key, value);
    
  }else{
    
    for(long i = 0, it = newPos+1; i < map->capacity; i++, it++){
      it = it%map->capacity;
      if(*(map->buckets+it) ==  NULL){
        *(map->buckets+it) = createPair(key, value);
        break;
      } else if((*(map->buckets+it))->key == NULL){
        *(map->buckets+it) = createPair(key, value);
        break;
      }
    }
  }
  map->size++;
}


void enlarge(HashMap * map) {
  enlarge_called = 1; //no borrar (testing purposes)

  map->capacity*=2;
  Par** oldB = map->buckets; 

  map->buckets = (Par**) calloc(map->capacity, sizeof(Par));

  map->size = 0;

  for(long it = 0; it < map->capacity/2; it++){
    if(*(oldB+it) ==  NULL) continue;
    insertMap(map, (*(oldB+it))->key, (*(oldB+it))->value ); 
  }
}


HashMap * createHash(long capacity) {

  HashMap* nuevo = (HashMap*) calloc(1, sizeof(HashMap));

  nuevo->buckets = (Par**) calloc(capacity, sizeof(Par) );

  nuevo->current = -1;
  nuevo->size = 0;
  nuevo->capacity = capacity;
  
  
  return nuevo;
}

void eraseMap(HashMap * map,  char * key) {    
  if (map == NULL || key == NULL) {
    return;
  }

  long pos = hash(key, map->capacity);

  if (*(map->buckets + pos) == NULL) {
    return;
  }

  if (is_equal(key, (*(map->buckets + pos))->key)) {
    *(map->buckets + pos) = NULL;
  } else {
    for (long i = 0, it = pos + 1; i < map->capacity; i++, it++) {
      it = it % map->capacity;

      if (*(map->buckets + it) == NULL) continue;
  
      if (is_equal(key, (*(map->buckets + it))->key)) {
        free((*(map->buckets + it))->key);
        free(*(map->buckets + it));
        *(map->buckets + it) = NULL;
        break;
      }
    }
    
  }

  if (map->size > 0) map->size--;
  
}
Par * searchMap(HashMap * map,  char * key) {   
  
  long pos = hash(key, map->capacity);
  
  if(*(map->buckets+pos) == NULL){
    return NULL;
  }

  if( is_equal((*(map->buckets+pos))->key, key) ){
    return *(map->buckets+pos);
  }else{

    for(long i = 0, it = pos+1; i < map->capacity; i++, it++){
      it = it%map->capacity;
      if( *(map->buckets+it) ==  NULL) continue;
      if(is_equal(key, (*(map->buckets+it))->key)){
        return *(map->buckets+it);
      }
    }
    
  }
  
  return NULL;
}

Par * firstMap(HashMap * map) {

  for(long it = 0; it < map->capacity; it++){

    if(*(map->buckets+it) ==  NULL) continue;

    if( (*(map->buckets+it))->key != NULL ){
      map->current = it;
      return *(map->buckets+it);
    }
  }
  return NULL;
}

Par * nextMap(HashMap * map) {

  for(long it = map->current+1; it < map->capacity; it++){

    if(*(map->buckets+it) ==  NULL) continue;

    if( (*(map->buckets+it))->key != NULL ){
      map->current = it;
      return *(map->buckets+it);
    }
  }
  
  return NULL;
}
