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
    char id[10];
    int tipo;
    int subtipo;
    char nombre[50];
    int efecto;
    int duracion;
}Objeto;

typedef struct{
    char nombre[50];
    int vidas;
    int ataque;
    int defensa;
    int nivel;
    List* inventario;
    Pos ubi;
    Pos antes;
    Objeto* arma;
    Objeto* armadura;
}Jugador;

typedef struct{
    Jugador* jugador;
    Map* mapEnemigos;
    Map* mapObjetos;
    Laberinto* laberinto;
}Juego;

void mostrarMenu() {
    printf("\n===== MENÚ PRINCIPAL =====\n");
    printf("1. Iniciar partida\n");
    printf("2. Salir\n");
    printf("Seleccione una opción: 1/2");
}

//###Variables Globales

int cantidadLaberintos = 0;
int cantidadEnemigos = 5;

//###Extras

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

void pausar(){
    printf("Presione una tecla para continuar\n");
    getch();
    system("clear");
}
//###Importar

Map* importarLaberintos(){

    FILE* file = fopen("laberintos.csv","r");

    Map* laberintos = createMap();

    char encabezados[256];
    fgets(encabezados, sizeof(encabezados), file);

    char linea[256];

    Laberinto* nuevoLaberinto;

    while( fgets(linea, sizeof(linea), file ) ){

        nuevoLaberinto = calloc(1, sizeof(Laberinto) );

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

        for(int i = 0; i < nuevoLaberinto->alto; i++){
            fgets(linea, sizeof(linea), file);
            token = strtok(linea, ",");

            for(int j = 0; j < nuevoLaberinto->ancho; j++){
                if( strcmp(token, "#") == 0) nuevoLaberinto->tablero[i][j] = -1;
                else if( strcmp(token, "?") == 0) nuevoLaberinto->tablero[i][j] = -2;
                else if(strcmp(token, " ") == 0){
                    nuevoLaberinto->tablero[i][j] = 0;
                }else{
                    nuevoLaberinto->tablero[i][j] = atoi(token);
                }

                token = strtok(NULL, ",");
            }
        }

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

Map* importarObjetos(){

    FILE* archivo = fopen("objetos.csv", "r");

    if (archivo == NULL) {
        printf("Error al abrir el archivo.\n");
        return NULL;
    }

    Map* mapaObjetos = createMap();
    char linea[256];
    while (fgets(linea, sizeof(linea), archivo) != NULL) {

        Objeto* objeto = calloc(1,sizeof(Objeto));

        char id[10];
        int tipo;
        int subtipo;
        char nombre[50];
        int efecto;
        int duracion;

        char* token = strtok(linea, ",");

        strcpy(objeto->id, token);

        token = strtok(NULL, ",");
        objeto->tipo = atoi(token);

        if(objeto->tipo == 3){
            token = strtok(NULL, ",");
            objeto->subtipo = atoi(token);
        }

        token = strtok(NULL, ",");
        strcpy(objeto->nombre , token) ;

        token = strtok(NULL, ",");
        objeto->efecto = atoi(token);

        token = strtok(NULL, ",");
        objeto->duracion = atoi(token);

        Insert(mapaObjetos, objeto->id, objeto);
    }

    return mapaObjetos;
}


//### Pospreparacion


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

void crearJugador(Jugador * joselito){
    joselito->vidas = 10;
    joselito->ataque = 3;
    joselito->defensa = 1;
    joselito->inventario = createList();
    char nombre[15];
    printf("Ingrese el nombre de su jugador: ");
    scanf("%s",nombre);
    strcpy(joselito->nombre, nombre);
}


//### Para python

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
            if( i == posW.x && j == posW.y )fprintf(archivo, "%c,", dir);
            else if(laberinto->tablero[i][j] > 0 && laberinto->tablero[i][j] < 100) fprintf(archivo, "e,");
            else if(laberinto->tablero[i][j] > 100) fprintf(archivo, "c,");
            else fprintf(archivo, "%d,", laberinto->tablero[i][j]);
        }
        fprintf(archivo, "\n");
    }

    fclose(archivo);

}

void mostrarEntornoJugador(Jugador* inserteNombre ){

    FILE* archivo = fopen("readJug.csv", "w");

    fprintf(archivo, "%s,", inserteNombre->nombre);
    fprintf(archivo, "%d,", inserteNombre->vidas);
    fprintf(archivo, "%d,", inserteNombre->ataque);
    fprintf(archivo, "%d,", inserteNombre->defensa);
    fprintf(archivo, "%d,", inserteNombre->nivel);
    fprintf(archivo, "\n");

    fclose(archivo);

}

void mostrarEntornoEnemigo(Enemigo* enemigo){
    FILE* archivo = fopen("readEne.csv", "w");

    fprintf(archivo, "%s,", enemigo->nombre);
    fprintf(archivo, "%d,", enemigo->vidas);
    fprintf(archivo, "%d,", enemigo->ataque);
    fprintf(archivo, "%d,", enemigo->defensa);
    fprintf(archivo, "%s,", enemigo->id);

    fclose(archivo);
}


void mostrarEntornoDecision(int coso){
    FILE* archivo = fopen("readBD.csv", "w");
    fprintf(archivo, "%d,", coso);
    fclose(archivo);
}

void mostrarEntornoObjeto(Objeto* objeto){
    FILE* archivo = fopen("readObj.csv", "w");

    fprintf(archivo, "%s,", objeto->nombre);
    fprintf(archivo, "%d,", objeto->tipo);
    fprintf(archivo, "%d,", objeto->efecto);
    fprintf(archivo, "%d,", objeto->duracion);

    fclose(archivo);
}


//###archivo mediador

void screenHome(){
    FILE* archivo = fopen("readMed.csv", "w");
    fprintf(archivo, "0");
    fclose(archivo);
}

void screenLab(){
    FILE* archivo = fopen("readMed.csv", "w");
    fprintf(archivo, "1");
    fclose(archivo);
}

void screenDied(){
    FILE* archivo = fopen("readMed.csv", "w");
    fprintf(archivo, "2");
    fclose(archivo);
}

void screenBattle(){
    FILE* archivo = fopen("readMed.csv", "w");
    fprintf(archivo, "3");
    fclose(archivo);
}

void screenBattleOb(){
    FILE* archivo = fopen("readMed.csv", "w");
    fprintf(archivo, "4");
    fclose(archivo);
}

void screenTreasure(){
    FILE* archivo = fopen("readMed.csv", "w");
    fprintf(archivo, "5");
    fclose(archivo);
}

void screenObj(){
  FILE* archivo = fopen("readMed.csv", "w");
  fprintf(archivo, "6");
  fclose(archivo);
}

///###Para consola

void mostrarLaberinto(Laberinto* laberinto, Pos posW ){

    for(int i = 0; i < laberinto->alto; i++){
        for(int j = 0; j < laberinto->ancho;j++){
            //printf("[%d]", laberinto->tablero[i][j]) ;
            if(laberinto->tablero[i][j] == -1) printf("[%c]",MURO);
            else if(laberinto->tablero[i][j] == -2) printf("[?]");
            else if( i == posW.x && j == posW.y ) printf(" P ");
            else if(laberinto->tablero[i][j] >= 1 && laberinto->tablero[i][j] <= 100) printf(" E ");
            else if(laberinto->tablero[i][j] > 100) printf(" C ");
            else printf("   ");
        }

        printf("\n");
    }

}

void mostrarDetBat(Jugador* jugador, int op, Enemigo* enemigo){
    printf("[#][#][#][#][#][#][#][#][#][#]\n");
    printf("%6s-%d-%d  %10s-%d\n",jugador->nombre,jugador->vidas,jugador->ataque,enemigo->nombre,enemigo->vidas);
    printf("[#][#][#][#][#][#][#][#][#][#]\n");
    if(op == 0)printf("-> ");
    printf("1) Atacar\n");
    if(op == 1) printf("-> ");
    printf("2) Mochila\n");
    if(op == 2) printf("-> ");
    printf("3) Huir\n");
    printf("\nPresione [J] para seleccionar\n");

}

Objeto* copiarObjeto(Objeto * objeto){

    Objeto* nuevoObjeto = calloc(1,sizeof(Objeto));

    strcpy(nuevoObjeto->id , objeto->id);
    nuevoObjeto->duracion = objeto->duracion;
    nuevoObjeto->efecto = objeto->efecto;
    nuevoObjeto->subtipo = objeto->subtipo;
    strcpy(nuevoObjeto->nombre , objeto->nombre);
    nuevoObjeto->tipo = objeto->tipo;

    return nuevoObjeto;
}

void mostrarInventario(Objeto * objeto){

    printf("<- %s - %d - %d - %d ->\n\n",objeto->nombre, objeto->tipo , objeto->efecto , objeto->duracion);
    printf("Presione [J] para seleccionar\n");
    printf("Presione [K] para cerrar\n\n");
}

void mostrarEquipado(Jugador * jugador){
    if(jugador->arma != NULL)
        printf("Arma equipada: %s - %d - %d - %d\n",jugador->arma->nombre, jugador->arma->tipo , jugador->arma->efecto , jugador->arma->duracion);
      else
        printf("Arma equipada: No tiene un arma equipada\n");
      
    if(jugador->armadura != NULL)
        printf("%s - %d - %d - %d\n\n",jugador->armadura->nombre, jugador->armadura->tipo , jugador->armadura->efecto , jugador->armadura->duracion);
      else
        printf("Armadura equipada: No tiene una armadura equipada\n\n");
  
}

int interactuarInventario(Jugador * jugador, Enemigo* enemigo, int op){
    Objeto* it = firstList(jugador->inventario);
    
    if(firstList(jugador->inventario) == NULL){
      mostrarEquipado(jugador);
      printf("Mochila Vacia, Presione una tecla para continuar\n");
      getch();
      return 1;
    }

    int coso;

    while(true){
        mostrarEquipado(jugador);
        mostrarEntornoObjeto(it);
        if(enemigo != NULL){
          
          mostrarDetBat(jugador, op, enemigo);
          screenBattleOb();
          printf("\n");
        }else{
          screenObj();//
        }
        mostrarInventario(it);
        coso = getch();
        //printf("%d\n",coso);
        switch(coso){

            case 100:
                //rotarDerecha
                it = nextList(jugador->inventario);
                if(it == NULL) it = firstList(jugador->inventario);

                break;
            case 97:
                //rotarIzquierda(a, b, c);
                it = prevList(jugador->inventario);
                if(it == NULL) it = lastList(jugador->inventario);
                break;
            case 106:
                //objeto seleccionado
                
                switch(it->tipo){
                  case 1:
                    if(jugador->arma == NULL){
                      printf("Desea equipar esta arma?\n");
                      printf("[J] Aceptar - [K] Cancelar\n");
                      int tecla = getch();
                      
                      if(tecla == 106){
                        jugador->arma = it; 
                        jugador->ataque += jugador->arma->efecto;
                        popCurrent(jugador->inventario);
                      }
                      return 1;
                      break;
                    }
                    else{
                      printf("Desea equipar esta arma?\n");
                      printf("[J] Aceptar - [K] Cancelar\n");
                      int tecla = getch();
                      
                      if(tecla == 106){
                        pushBack(jugador->inventario, jugador->arma);
                        jugador->ataque -= jugador->arma->efecto;
                        jugador->arma = it; 
                        jugador->ataque += jugador->arma->efecto;
                        popCurrent(jugador->inventario);
                      }
                      return 1;
                      break;
                    }
                    
                    break;

                  case 2:
                    if(jugador->armadura == NULL){
                      printf("Desea equipar esta armadura?\n");
                      printf("[J] Aceptar - [K] Cancelar\n");
                      int tecla = getch();
                      
                      if(tecla == 106){
                        jugador->armadura = it; 
                        jugador->defensa += jugador->armadura->efecto;
                        popCurrent(jugador->inventario);
                      }
                      return 1;
                      break;
                    }
                    else{
                      printf("Desea equipar esta armadura?\n");
                      printf("[J] Aceptar - [K] Cancelar\n");
                      int tecla = getch();
                      
                      if(tecla == 106){
                        pushBack(jugador->inventario, jugador->armadura);
                        jugador->defensa -= jugador->armadura->efecto;
                        jugador->armadura = it; 
                        jugador->defensa += jugador->armadura->efecto;
                        popCurrent(jugador->inventario);
                      }
                      return 1;
                      break;
                    }
                    
                    break;
                  case 3:
                    switch(it->subtipo){
                        case 1:
                          jugador->vidas += it->efecto;
                          popCurrent(jugador->inventario);
                          return 1;
                          break;

                        case 2:
                          jugador->ataque += it->efecto;
                          popCurrent(jugador->inventario);
                          return 1;
                          break;
                          
                        case 3:
                          jugador->defensa += it->efecto;
                          popCurrent(jugador->inventario);
                          return 1;
                          break;
                          
                        case 5:
                          jugador->vidas = 0;
                          popCurrent(jugador->inventario);
                          return 1;
                          break;
                      
                      }
                      break;
                }
                
                break;
            case 107:
                return 1;
                break;
        }

        system("clear");
    }

}


int dueloAmuerteConCuchillos(Jugador* jugador , Enemigo* enemigo){
    int vida = enemigo->vidas;
    int op = 0;
    int salida;
    while(true){
        mostrarDetBat(jugador, op, enemigo);
        mostrarEntornoJugador(jugador);
        mostrarEntornoEnemigo(enemigo);
        mostrarEntornoDecision(op);
        int tecla ;
        tecla = getch();



        switch (tecla) {
            case 106:

                switch (op) {
                    case 0:
                        if((jugador->ataque - enemigo->defensa) > 0)
                          enemigo->vidas -= (jugador->ataque - enemigo->defensa);
                        if(jugador->arma != NULL){
                          jugador->arma->duracion--;
                          if(jugador->arma->duracion == 0){
                            jugador->ataque -= jugador->arma->efecto;
                            jugador->arma = NULL;
                            printf("Tu arma se ha roto!\n");
                            printf("Presione un boton para continuar");
                            getch();
                          }
                        }
                        if (enemigo->vidas <= 0) {

                            enemigo->vidas = vida;

                            //bonificacion por ganar

                            if (atoi(enemigo->id) >= 22) return 2;
                            else return 1;
                        }
                        break;

                    case 1:
                        system("clear");
                        salida = 0;
                        salida = interactuarInventario(jugador, enemigo, op);

                        break;

                    case 2:
                        printf("te arrancaste\n");
                        return -1;
                        break;
                }
                if (salida == 0){
                    screenBattle();
                    break;
                }
                if((enemigo->ataque - jugador->defensa) > 0){
                  jugador->vidas -= (enemigo->ataque - jugador->defensa);
                }
                if(jugador->armadura != NULL){
                  jugador->armadura->duracion--;
                  if(jugador->armadura->duracion == 0){
                    jugador->defensa -= jugador->armadura->efecto;
                    jugador->armadura = NULL;
                    printf("Tu armadura se ha roto!\n");
                    printf("Presione un boton para continuar");
                    getch();
                  }
                }
                if(jugador->vidas <= 0)
                    return 0;

                break;
            case 119:
                op--;
                break;
            case 115:
                op++;
                break;
        }

        if(op < 0) op = 2;
        if(op > 2) op = 0;


        system("clear");

    }
}

int moverWachin(Juego * newGame, Pos* posW, char* dir ){

    int tecla;
    tecla = getch();  // Leer la tecla presionada
    //printf("%d\n",tecla);
    switch (tecla) {
        case 119:  // W

            if( posW->x-1 >= 0 && posW->x-1 < newGame->laberinto->alto  && newGame->laberinto->tablero[posW->x-1][posW->y] != -1 ){
                posW->x--;
            }

            break;

        case 115:  // S

            if( posW->x+1 >= 0 && posW->x+1 < newGame->laberinto->alto  && newGame->laberinto->tablero[posW->x+1][posW->y] != -1 ){
                posW->x++;
            }

            break;

        case 97:  // A
            if( posW->y-1 >= 0 && posW->y-1 < newGame->laberinto->ancho  && newGame->laberinto->tablero[posW->x][posW->y-1] != -1 ){
                posW->y--;
                *dir = 'L';
            }
            break;

        case 100:  // D
            if( posW->y+1 >= 0 && posW->y+1 < newGame->laberinto->ancho  && newGame->laberinto->tablero[posW->x][posW->y+1] != -1 ){
                posW->y++;
                *dir = 'R';
            }
            break;

        case 109: // M
          system("clear");
          interactuarInventario(newGame->jugador, NULL, 0);
          screenLab();
          break;

        case  107: // K -> salir

            return 1;

        default:
            printf("Otra tecla\n");
            break;
    }
    return 0;
}




int juego(Juego* newGame){

    int tecla;
    int check;
    char dir = 'R';

    while(true){
        screenLab();
        mostrarLaberinto(newGame->laberinto, newGame->jugador->ubi);
        mostrarEntorno(newGame->laberinto, newGame->jugador->ubi, dir);
        mostrarEntornoJugador(newGame->jugador);

        newGame->jugador->antes = newGame->jugador->ubi;

        check = moverWachin(newGame, &newGame->jugador->ubi, &dir);

        if(check == 1){
            return 0;
        }

        system("clear");
        if(newGame->laberinto->tablero[newGame->jugador->ubi.x][newGame->jugador->ubi.y] > 100){

            char idBuscado[10];
            snprintf(idBuscado, sizeof(idBuscado), "%d",  newGame->laberinto->tablero[newGame->jugador->ubi.x][newGame->jugador->ubi.y]);

            Pair * item = Search(newGame->mapObjetos, idBuscado);

            Objeto* objeto = item->value;

            screenTreasure();
            mostrarEntornoObjeto(objeto);

            printf("Has abierto un cofre!\n");
            printf("en él has encontrado una: %s!\n", objeto->nombre);
            pausar();

            newGame->laberinto->tablero[newGame->jugador->ubi.x][newGame->jugador->ubi.y] = 0;
            pushBack(newGame->jugador->inventario , copiarObjeto(objeto));
        }else if( newGame->laberinto->tablero[newGame->jugador->ubi.x][newGame->jugador->ubi.y] >= 1){

            char idBuscado[10];
            snprintf(idBuscado, sizeof(idBuscado), "%d", newGame->laberinto->tablero[newGame->jugador->ubi.x][newGame->jugador->ubi.y] );

            Pair* enemigo = Search( newGame->mapEnemigos , idBuscado);
            screenBattle();
            int resultado = dueloAmuerteConCuchillos(newGame->jugador, enemigo->value);

            if(resultado == 1){
                newGame->laberinto->tablero[newGame->jugador->ubi.x][newGame->jugador->ubi.y] = 0;
                //recompensa
                printf("lol lo mate\n");
            }else if (resultado == 0){
                printf("lol me mataron \n");
                printf("Perdiste por manco, Fin del juego\n");
                exit(EXIT_SUCCESS); //pa cerrar el juego por que perdiste
            }
            else if(resultado == -1){

                newGame->jugador->ubi = newGame->jugador->antes;

            }else if(resultado == 2){
              
              break;
            }

            printf("Presione una tecla para continuar\n");
            getch();
            system("clear");
        }
    }
  return 1;
}

int main( ){
  int opcionPrincipal;
  
  mostrarMenu();
  scanf("%d",&opcionPrincipal);
    
  if (opcionPrincipal == 1){
    screenHome();
    Juego* jueguito = calloc(1,sizeof(Juego));

    Map* laberintos = importarLaberintos();
    Laberinto* laberinto = obtenerLaberinto(laberintos);

    Jugador* joselito = calloc(1, sizeof(Jugador) );
    crearJugador(joselito);

    Map* enemigos = importarEnemigos();

    joselito->ubi = laberinto->inicio;

    Map * objetos = importarObjetos();
    jueguito->mapObjetos = objetos;

    jueguito->laberinto = laberinto;
    jueguito->jugador = joselito;
    jueguito->mapEnemigos = enemigos;

    generarEnemigos(jueguito);

    while(juego(jueguito) ){
      jueguito->laberinto = laberinto; = obtenerLaberinto(laberintos);
    }

  }
  else{
    exit(EXIT_SUCCESS);
  }
  
  return 0;
}