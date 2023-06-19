#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdbool.h>
#include <time.h>

#include "list.h"
#include "map.h"
#include "hashmap.h"

#define TAM 100

typedef struct{
	int x;
	int y;
}Pos;

typedef struct{
	char nombre[50];
	int vidas;
	int ataque;
	int defensa;
	float experiencia;
	Map* inventario;
  //Pos posJug;
}Jugador;

typedef struct{
	char id[10];
	char nombre[50];
	int vidas;
	int ataque;
	int defensa;
  //Pos posEne;
}Enemigo;

typedef struct{
	char id[10];
	int ancho;
	int alto;
	Pos inicio;
	Pos salida;
	int tablero[TAM][TAM];
}Laberinto;

typedef struct{
	Jugador* jugador;
	Map* mapEnemigos;
	Laberinto* laberinto;
}Juego;


int cantidadLaberintos = 0;
int cantidadEnemigos = 5;

void generarEnemigos(Juego * game){
  
  srand(time(NULL));
  
  int existen = cantidadEnemigos;
  int porcentaje = 15;
  int maximo = (existen*100 )/porcentaje;
  

  for(int i =0; i < game->laberinto->alto; i++){
    for(int j =0; j < game->laberinto->ancho; j++){
      if( (i == game->laberinto->inicio.x && j == game->laberinto->inicio.y)  || (i == game->laberinto->salida.x && j == game->laberinto->salida.y) ) continue;
      if( game->laberinto->tablero[i][j] == 0){
        
        int obtenido = 1 + rand() % (maximo);	
				if(obtenido <= existen){
					game->laberinto->tablero[i][j] = 1;
				} 
      }
    }
  }
}

Laberinto* obtenerLaberinto(Map* laberintos){
  srand(time(NULL));
  int obtenido = 0 + rand() % (cantidadLaberintos);

  char idBuscado[10];
  snprintf(idBuscado, sizeof(idBuscado), "%d", obtenido);
  Pair* parLab = Search(laberintos, idBuscado);
  return parLab->value;
}

/*Map* importarLaberintos(){
  
  FILE* archivoLaberintos;
  Map* laberinto = createMap();
  int alto,ancho;

  archivoLaberintos = fopen("laberintos.csv","r");
  
  if (archivoLaberintos == NULL){
    printf("Error al abrir el archivo. \n");
    return NULL;
  }
  
  fscanf(archivoLaberintos, "%d %d\n", &alto, &ancho);
  char linea[256];
  
  while (fgets(linea, sizeof(linea),archivoLaberintos) != NULL){
    
    char nombre[10];
    Pos inicio,salida;

    int tablero[alto][ancho];

    sscanf(linea,"%9[^,],%d,%d",nombre,&inicio,&salida);
    //printf("hola");
    Laberinto* lab = (Laberinto*)malloc(sizeof(Laberinto));
    strcpy(lab->id,nombre);
    lab->alto = alto;
    lab->ancho = ancho;
    lab->inicio = inicio;
    lab->salida = salida;
    memcpy(lab->tablero, tablero, sizeof(int) * alto * ancho);
    
    insertMap(laberinto, nombre, lab);
  }

  fclose(archivoLaberintos);
  return laberinto;
}*/

Map* NimportarLaberintos(){

  FILE* file = fopen("laberintos.csv","r");

  Map* laberintos = createMap();

  char encabezados[256];
  fgets(encabezados, sizeof(encabezados), file);

  char linea[256];

  Laberinto* nuevoLaberinto;
  
  while( fgets(linea, sizeof(linea), file ) ){

    nuevoLaberinto = calloc(1, sizeof(Laberinto) );
    
    //printf("linea-> %s\n", linea);
    
    char* token = strtok(linea, ",");

    strcpy(nuevoLaberinto->id, token);

    token = strtok(NULL, ",");
    nuevoLaberinto->alto = atoi(token);

    token = strtok(NULL, ",");
    nuevoLaberinto->ancho = atoi(token);

    token = strtok(NULL, ",");
    nuevoLaberinto->inicio.x = atoi(token);

    token = strtok(NULL, ",");
    nuevoLaberinto->inicio.y = atoi(token);

    token = strtok(NULL, ",");
    nuevoLaberinto->salida.x = atoi(token);

    token = strtok(NULL, ",");
    nuevoLaberinto->salida.y = atoi(token);

    //printf(" %s, %d, %d, %d, %d, %d, %d  \n", nuevoLaberinto->id, nuevoLaberinto->alto, nuevoLaberinto->ancho, nuevoLaberinto->inicio.x, nuevoLaberinto->inicio.y, nuevoLaberinto->salida.x, nuevoLaberinto->salida.y);

    for(int i = 0; i < nuevoLaberinto->alto; i++){
      fgets(linea, sizeof(linea), file);
      token = strtok(linea, ",");

      for(int j = 0; j < nuevoLaberinto->ancho; j++){
        if( strcmp(token, "#") == 0) nuevoLaberinto->tablero[i][j] = -1;
        else nuevoLaberinto->tablero[i][j] = 0;
        //printf("[%d]", nuevoLaberinto->tablero[i][j]);
        token = strtok(NULL, ",");
      }
    }

    nuevoLaberinto->tablero[ nuevoLaberinto->inicio.x ][nuevoLaberinto->inicio.y] = -2;
    
    Insert(laberintos, nuevoLaberinto->id, nuevoLaberinto);
    
    cantidadLaberintos++;
    
  }
  fclose(file);
  return laberintos;
}

Map* importarEnemigos(){
    
    FILE* archivo = fopen("enemigos.csv", "r");
    if (archivo == NULL) {
        printf("Error al abrir el archivo.\n");
        return NULL;
    }

    Map* enemigosMap = createMap();

    char linea[256];
    
    while (fgets(linea, sizeof(linea), archivo) != NULL) {
        char id[10];
        char nombre[50];
        int vidas;
        int cantAtaques;
        int cantDefensas;

        sscanf(linea, "%9[^,],%49[^,],%d,%d,%d", id, nombre, &vidas, &cantAtaques, &cantDefensas);

        Enemigo* enemigoData = (Enemigo*)malloc(sizeof(Enemigo));
        strcpy(enemigoData->id, id);
        strcpy(enemigoData->nombre, nombre);
        enemigoData->vidas = vidas;
        enemigoData->ataque = cantAtaques;
        enemigoData->defensa = cantDefensas;

        Insert(enemigosMap, id, enemigoData);
    }
    printf("Enemigos importados correctamente\n");
    fclose(archivo);
    return enemigosMap;
}

void mostrarLaberinto(Laberinto* laberinto){

  for(int i = 0; i < laberinto->alto; i++){
    for(int j = 0; j < laberinto->ancho;j++){
      //printf("[%d]", laberinto->tablero[i][j]) ;
      if(laberinto->tablero[i][j] == -1) printf("[#]");
      else if(laberinto->tablero[i][j] == 1) printf("[E]"); 
      else if(laberinto->tablero[i][j] == -2) printf("[P]"); 
      else printf("[ ]");
    }
    printf("\n");
  }
  
}


int main( ){
  Map* laberintos = NimportarLaberintos();

  Laberinto* laberinto = obtenerLaberinto(laberintos);
  
  //funcion prueba
 
  Juego* nuevoJuego ;
  nuevoJuego = calloc(1 , sizeof(Juego));
  nuevoJuego->laberinto = laberinto;
  
  generarEnemigos(nuevoJuego);
  mostrarLaberinto(laberinto);

  Jugador* joselito = calloc(1, sizeof(Jugador) );

  return 0;
}