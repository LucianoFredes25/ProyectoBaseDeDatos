#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdbool.h>

#include <termios.h>
#include <unistd.h>

#include <time.h>

#include "list.h"
#include "map.h"
#include "hashmap.h"

#define MURO 35

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
  int nivel;
  float experiencia;
	List* inventario;
  Pos ubi;
}Jugador;

typedef struct{
	char id[10];
	char nombre[50];
	int vidas;
	int ataque;
	int defensa;
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
					game->laberinto->tablero[i][j] = obtenido;
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


Map* importarLaberintos(){

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

    //nuevoLaberinto->tablero[ nuevoLaberinto->inicio.x ][nuevoLaberinto->inicio.y] = -2;
    
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

      
      
        Insert(enemigosMap, enemigoData->id, enemigoData);
    }
    fclose(archivo);
    return enemigosMap;
}

void mostrarEntorno(Laberinto* laberinto, Pos posW, char dir){

  FILE* archivo = fopen("readLab.csv", "w");

  int x = posW.x -2;
  int y = posW.y -2;

  if(x < 0) x = 0;
  if(y < 0) y = 0;

  if(x+5 > laberinto->alto-1) x = laberinto->alto - 5;
  if(y+5 > laberinto->ancho-1) y = laberinto->ancho - 5;
  printf("\n\n");
  
  for(int i = x ; i < x+5; i++){
    for(int j = y; j < y+5; j++){
      if( i == posW.x && j == posW.y )fprintf(archivo, "%c\,", dir); 
      else fprintf(archivo, "%d,", laberinto->tablero[i][j]); 
    }
    fprintf(archivo, "\n");
  }

  fclose(archivo);
  
}
      
void mostrarLaberinto(Laberinto* laberinto, Pos posW ){

  for(int i = 0; i < laberinto->alto; i++){
    for(int j = 0; j < laberinto->ancho;j++){
      //printf("[%d]", laberinto->tablero[i][j]) ;
      if(laberinto->tablero[i][j] == -1) printf("[%c]",MURO);
      else if( i == posW.x && j == posW.y ) printf(" P "); 
      else if(laberinto->tablero[i][j] >= 1) printf(" %d ", laberinto->tablero[i][j]); 
      else printf("   ");
    }
    printf("\n");
  }
  
}


int getch() {
    struct termios old, new;
    int ch;

    tcgetattr(STDIN_FILENO, &old);
    new = old;
    new.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &new);

    ch = getchar();

    tcsetattr(STDIN_FILENO, TCSANOW, &old);

    return ch;
}

int dueloAmuerteConCuchillos(Jugador* jugador , Enemigo* enemigo){

  while(true){
    if(jugador->vidas <= 0 ) return -1;
    if(enemigo->vidas <= 0 ) return 0;
    
    printf("%s - %d\n", jugador->nombre, jugador->vidas);
    printf("Ataque: %d\n\n", jugador->ataque);
    
    printf("%s - %d\n", enemigo->nombre, enemigo->vidas);
    printf("Ataque: %d\n\n", enemigo->ataque);

    break;
    //opciones ?? 1 2 3
    
  }
}

void moverWachin(Laberinto* laberinto, Pos* posW, char* dir ){

  int tecla;
  tecla = getch();  // Leer la tecla presionada

  switch (tecla) {
    case 119:  // Flecha arriba
        
      if( posW->x-1 >= 0 && posW->x-1 < laberinto->alto  && laberinto->tablero[posW->x-1][posW->y] != -1 ){
         posW->x--; 
        }
        
        break;
                
    case 115:  // Flecha abajo
        
      if( posW->x+1 >= 0 && posW->x+1 < laberinto->alto  && laberinto->tablero[posW->x+1][posW->y] != -1 ){
        posW->x++; 
      }
        
      break;
                
    case 97:  // Flecha izquierda
      if( posW->y-1 >= 0 && posW->y-1 < laberinto->ancho  && laberinto->tablero[posW->x][posW->y-1] != -1 ){
        posW->y--;
        *dir = 'L';
      }
      break;
                
    case 100:  // Flecha derecha
      if( posW->y+1 >= 0 && posW->y+1 < laberinto->ancho  && laberinto->tablero[posW->x][posW->y+1] != -1 ){
        posW->y++;
        *dir = 'R';
      }
      break;
                
    default:
      printf("Otra tecla\n");
      break;
  }

}



void juego(Juego* newGame){


  int tecla;
  char dir = 'R';
  
  while(true){
    
    mostrarLaberinto(newGame->laberinto, newGame->jugador->ubi);
    mostrarEntorno(newGame->laberinto, newGame->jugador->ubi, dir);
    
    moverWachin(newGame->laberinto, &newGame->jugador->ubi, &dir);

    system("clear");
    
    if( newGame->laberinto->tablero[newGame->jugador->ubi.x][newGame->jugador->ubi.y] >= 1){

      char idBuscado[10];
  snprintf(idBuscado, sizeof(idBuscado), "%d", newGame->laberinto->tablero[newGame->jugador->ubi.x][newGame->jugador->ubi.y] );

      Pair* enemigo = Search( newGame->mapEnemigos , idBuscado);
      
      dueloAmuerteConCuchillos(newGame->jugador, enemigo->value);

      
      printf("Duelo a muerte con cuchillos\n\n");
      printf("Presione una tecla para continuar\n");
      getch();
      system("clear");
    }
    
  }
  
}


int main( ){


  Juego* jueguito = calloc(1,sizeof(Juego));
  
  Map* laberintos = importarLaberintos();
  Laberinto* laberinto = obtenerLaberinto(laberintos);
  Jugador* joselito = calloc(1, sizeof(Jugador) );
  joselito->vidas = 10;
  joselito->experiencia = 0;
  joselito->ataque = 3;
  joselito->defensa = 2;
  strcpy(joselito->nombre, "joselito");
  Map* enemigos = importarEnemigos();
  joselito->ubi = laberinto->inicio;
  
  //funcion prueba
 
  jueguito->laberinto = laberinto;
  jueguito->jugador = joselito;
  jueguito->mapEnemigos = enemigos;

  
  generarEnemigos(jueguito);
  
  juego(jueguito);

  return 0;
}