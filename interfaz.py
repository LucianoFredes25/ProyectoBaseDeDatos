import csv
import pygame
import time

# Inicializar pygame
pygame.init()

# Dimensiones de la ventana
ANCHO_VENTANA = 1600
ALTO_VENTANA = 900

# Tamaño de cada celda en el laberinto
ANCHO_CELDA = 180
ALTO_CELDA = 180

# Cargar imágenes
imagen_pared = pygame.image.load("Wall.png")
imagen_camino = pygame.image.load("Floor.jpg")
imagen_wachinR = pygame.image.load("PlayerR.jpg")
imagen_wachinL = pygame.image.load("PlayerL.jpg")
imagen_bichoFeo = pygame.image.load("Enemy.jpg")
imagen_tesoro = pygame.image.load("Treasure.jpg")
imagen_scenario = pygame.image.load("Scenario.jpg")
imagen_character = pygame.image.load("character.png")
imagen_torch = pygame.image.load("WallTorch.jpg")

imagen_enemy22 = pygame.image.load("enemy(22).png")


imagen_enemy = {}

for i in range(1, 6):
    cadena = "enemy(" + str(i) + ").png"
    imagen_enemy[str(i)] = pygame.image.load(cadena)

coor_enemy = {} 

coor_enemy['1'] = (825, 350)
coor_enemy['2'] = (925, 520)
coor_enemy['3'] = (950, 520)
coor_enemy['4'] = (875, 430)
coor_enemy['5'] = (975, 550)

scale_enemy = {}
scale_enemy['1'] = (400, 400)
scale_enemy['2'] = (225, 225)
scale_enemy['3'] = (200, 200)
scale_enemy['4'] = (300, 300)
scale_enemy['5'] = (200, 200)

imagen_pared = pygame.transform.scale(imagen_pared, (ANCHO_CELDA, ALTO_CELDA))
imagen_camino = pygame.transform.scale(imagen_camino, (ANCHO_CELDA, ALTO_CELDA))
imagen_wachinR = pygame.transform.scale(imagen_wachinR, (ANCHO_CELDA, ALTO_CELDA))
imagen_wachinL = pygame.transform.scale(imagen_wachinL, (ANCHO_CELDA, ALTO_CELDA))
imagen_bichoFeo = pygame.transform.scale(imagen_bichoFeo, (ANCHO_CELDA, ALTO_CELDA))
imagen_tesoro = pygame.transform.scale( imagen_tesoro, (ANCHO_CELDA, ALTO_CELDA))
imagen_scenario = pygame.transform.scale(imagen_scenario, (900,900) )
imagen_character = pygame.transform.scale(imagen_character, (200,200) )
imagen_torch = pygame.transform.scale(imagen_torch, (ANCHO_CELDA, ALTO_CELDA) )


for i in range(1, 6):
    imagen_enemy[str(i)] = pygame.transform.scale(imagen_enemy[str(i)], scale_enemy[str(i)])


imagen_enemy22 = pygame.transform.scale(imagen_enemy22, (200,200) )

# Cargar laberinto desde archivo CSV
def cargar_laberinto(nombre_archivo):
    laberinto = []
    with open(nombre_archivo, 'r') as archivo_csv:
        lector_csv = csv.reader(archivo_csv)
        for fila in lector_csv:
            laberinto.append(fila)
    return laberinto

def leer_med(ruta_archivo):
    with open(ruta_archivo, 'r') as archivo_csv:
        lector_csv = csv.reader(archivo_csv)
        for fila in lector_csv:
            dato = fila[0]
            return dato

def leer_coso(ruta_archivo):
    with open(ruta_archivo, 'r') as archivo_csv:
        lector_csv = csv.reader(archivo_csv)
        for fila in lector_csv:
            return fila
        

def leer_battleD(ruta_archivo):
    with open(ruta_archivo, 'r') as archivo_csv:
        lector_csv = csv.reader(archivo_csv)
        for fila in lector_csv:
            dato = fila[0]
            return dato

# Dibujar el laberinto en la ventana
def dibujar_laberinto(laberinto, ventana):
    
    laberinto_ancho = 5
    laberinto_alto = 5
    laberinto_ancho_pixels = laberinto_ancho * ANCHO_CELDA
    laberinto_alto_pixels = laberinto_alto * ALTO_CELDA
    x_inicial = (ANCHO_VENTANA - laberinto_ancho_pixels) // 2
    y_inicial = (ALTO_VENTANA - laberinto_alto_pixels) // 2
    
    for fila in range(len(laberinto)):
        for columna in range(len(laberinto[fila])-1):
            x = x_inicial + columna * ANCHO_CELDA
            y = y_inicial + fila * ALTO_CELDA
            if laberinto[fila][columna] == '-1':
                ventana.blit(imagen_pared, (x, y))
            elif laberinto[fila][columna] == '-2':
                ventana.blit(imagen_torch, (x,y))
            elif laberinto[fila][columna] == 'R':
                ventana.blit(imagen_wachinR, (x, y))
            elif laberinto[fila][columna] == 'L':
                ventana.blit(imagen_wachinL, (x, y))
            elif laberinto[fila][columna] == 'e':
                ventana.blit(imagen_bichoFeo, (x, y))
            elif laberinto[fila][columna] == 'c':
                ventana.blit(imagen_tesoro, (x, y))
            else:
                ventana.blit(imagen_camino, (x, y))

# Crear laberinto
laberinto = cargar_laberinto('readLab.csv')

# Inicializar la ventana
ventana = pygame.display.set_mode((ANCHO_VENTANA, ALTO_VENTANA))
pygame.display.set_caption("Laberinto")



#Meseji
COLOR_TEXTO = (255, 255, 255)

# Crear una fuente de texto
fuente = pygame.font.Font(None,46)

# Lista de mensajes
mensajes = [ "W : UP", "A : LEFT ", "S : DOWN", "D: RIGHT", "Mensaje 5"]

# Posición inicial del primer mensaje
posMX = 1300
posMY = 15

# Espacio vertical entre mensajes
espacio_vertical = 10

# Bucle principal
corriendo = True


def showLabyrinth(ventana):
    laberinto = cargar_laberinto('readLab.csv')
    personaje = leer_coso('readJug.csv');

    personaje[1] = 'N. vidas: ' + personaje[1]
    personaje[2] = 'Ataque: ' + personaje[2]
    personaje[3] = 'Defensa: ' + personaje[3]
    personaje[4] = 'Nivel: ' + personaje[4]
    personaje[5] = 'Exp.: ' + personaje[5]

    # Dibujar el laberinto en la ventana
    dibujar_laberinto(laberinto, ventana)

    posMY =  50
    # Dibujar los mensajes en la ventana
    for mensaje in personaje:
        # Renderizar el texto
        texto = fuente.render(mensaje, True, COLOR_TEXTO)
    
        # Obtener las dimensiones del texto
        texto_rect = texto.get_rect()
    
        # Establecer la posición del texto
        texto_rect.topleft = (posMX, posMY)
    
        # Dibujar el texto en la ventana
        ventana.blit(texto, texto_rect)
    
        # Actualizar la posición vertical para el siguiente mensaje
        posMY += texto_rect.height + espacio_vertical
        
        
        
def showBattle(ventana, tipo_pantalla):
    
    
    
    personaje = leer_coso('readJug.csv');
    enemigo = leer_coso('readEne.csv');
    
    ventana.blit(imagen_scenario, (350, 0))
    
    ventana.blit(imagen_character, (475, 550) )

    
    if(int(enemigo[4]) > 5):
        ventana.blit(imagen_enemy22, (975, 550))
    else:
        ventana.blit(imagen_enemy[enemigo[4]], coor_enemy[enemigo[4]])        
    
    personaje[1] = 'N. vidas: ' + personaje[1]
    personaje[2] = 'Ataque: ' + personaje[2]
    personaje[3] = 'Defensa: ' + personaje[3]

    enemigo[1] = 'N. vidas: ' + enemigo[1]
    enemigo[2] = 'Ataque: ' + enemigo[2]
    enemigo[3] = 'Defensa: ' + enemigo[3]

    posMX = 1300
    posMY = 50
    
    cont = 0
    # Dibujar los mensajes en la ventana
    for mensaje in enemigo:
        # Renderizar el texto
        texto = fuente.render(mensaje, True, COLOR_TEXTO)
    
        # Obtener las dimensiones del texto
        texto_rect = texto.get_rect()
    
        # Establecer la posición del texto
        texto_rect.topleft = (posMX, posMY)
    
        # Dibujar el texto en la ventana
        ventana.blit(texto, texto_rect)
    
        # Actualizar la posición vertical para el siguiente mensaje
        posMY += texto_rect.height + espacio_vertical
        cont +=1
        if(cont == 4): break
    
    posMX = 50
    posMY = 50
    cont = 0
    for mensaje in personaje:
        # Renderizar el texto
        texto = fuente.render(mensaje, True, COLOR_TEXTO)
    
        # Obtener las dimensiones del texto
        texto_rect = texto.get_rect()
    
        # Establecer la posición del texto
        texto_rect.topleft = (posMX, posMY)
    
        # Dibujar el texto en la ventana
        ventana.blit(texto, texto_rect)
    
        # Actualizar la posición vertical para el siguiente mensaje
        posMY += texto_rect.height + espacio_vertical
        cont +=1
        if(cont == 4): break
    
    
    if(tipo_pantalla == '3'):
        getD = leer_battleD('readBD.csv')
        
        decision = ["ATACAR", "MOCHILA", "HUIR"]
        
        if( getD == "0"): decision[0] = "--> " + decision[0]
        else: decision[0] = "    " + decision[0]
        
        
        if( getD == "1"): decision[1] = "--> " + decision[1]
        else: decision[1] = "    " + decision[1]
        
        if( getD == "2"): decision[2] = "--> " + decision[2]
        else: decision[2] = "    " + decision[2]
        
        posMX = 50
        posMY = 525
        
        for mensaje in decision:
            texto = fuente.render(mensaje, True, COLOR_TEXTO)
        
            # Obtener las dimensiones del texto
            texto_rect = texto.get_rect()
        
            # Establecer la posición del texto
            texto_rect.topleft = (posMX, posMY)
        
            # Dibujar el texto en la ventana
            ventana.blit(texto, texto_rect)
        
            # Actualizar la posición vertical para el siguiente mensaje
            posMY += texto_rect.height + espacio_vertical
    
    else:
            
        objeto = leer_coso('readObj.csv')
        
        posMX = 50
        posMY = 525
        
        for mensaje in objeto:
            texto = fuente.render(mensaje, True, COLOR_TEXTO)
        
            # Obtener las dimensiones del texto
            texto_rect = texto.get_rect()
        
            # Establecer la posición del texto
            texto_rect.topleft = (posMX, posMY)
        
            # Dibujar el texto en la ventana
            ventana.blit(texto, texto_rect)
        
            # Actualizar la posición vertical para el siguiente mensaje
            posMY += texto_rect.height + espacio_vertical
    
        
        
        
def showTreasure(ventana):
    
    objeto = leer_coso('readObj.csv')
    
    objeto[0] = "en el encontraste una: " + objeto[0]
    mensajes = ["HAZ ABIERTO UN COFRE!!!" ,objeto[0] ]
    
    posMX = 400
    posMY = 150
    
    for mensaje in mensajes:
        texto = fuente.render(mensaje, True, COLOR_TEXTO)
    
        # Obtener las dimensiones del texto
        texto_rect = texto.get_rect()
    
        # Establecer la posición del texto
        texto_rect.topleft = (posMX, posMY)
    
        # Dibujar el texto en la ventana
        ventana.blit(texto, texto_rect)
    
        # Actualizar la posición vertical para el siguiente mensaje
        posMY += texto_rect.height + espacio_vertical
    
    
def showObject(ventana):
    objeto = leer_coso('readObj.csv')
    
    posMX = 50
    posMY = 525
    
    for mensaje in objeto:
        texto = fuente.render(mensaje, True, COLOR_TEXTO)
    
        # Obtener las dimensiones del texto
        texto_rect = texto.get_rect()
    
        # Establecer la posición del texto
        texto_rect.topleft = (posMX, posMY)
    
        # Dibujar el texto en la ventana
        ventana.blit(texto, texto_rect)
    
        # Actualizar la posición vertical para el siguiente mensaje
        posMY += texto_rect.height + espacio_vertical

def mostrarVentana(ventana):
    
    tipo_pantalla = leer_med('readMed.csv')
    
    if(tipo_pantalla == '1'):
        showLabyrinth(ventana)
    elif(tipo_pantalla == '3' or tipo_pantalla == '4') :
        showBattle(ventana, tipo_pantalla)
    elif(tipo_pantalla == '5'):
        showTreasure(ventana)
    elif(tipo_pantalla == '6'):
        showLabyrinth(ventana)
        showObject(ventana)

while corriendo:
    
    ventana.fill((0, 0, 0)) 
    mostrarVentana(ventana)

    # Actualizar la ventana
    pygame.display.flip()

    # Esperar un poco antes de volver a leer el archivo (para evitar un bucle extremadamente rápido)
    time.sleep(0.1)

    # Revisar eventos
    for evento in pygame.event.get():
        if evento.type == pygame.QUIT:
            corriendo = False


pygame.quit()