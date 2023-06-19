//
//  Map.H
//  
//
//  Created by Al1bi
//  Copyright © 2023 Denilson Fiesta. All rights reserved. xd
//

#ifndef Map_h
#define Map_h

typedef struct AVLTreeNode AVLTreeNode;
typedef struct Map Map;

typedef struct Pair {
     char * key;
     void * value;
} Pair;

int Height(AVLTreeNode* root);

int getBalance(AVLTreeNode* root);

int max(int a, int b);

Map* createMap();

struct AVLTreeNode* createTreeNode(char* key, void* value);

struct AVLTreeNode* SingleRotateLeft(struct AVLTreeNode* X);

struct AVLTreeNode* SingleRotateRight(struct AVLTreeNode* W);

struct AVLTreeNode* DoubleRotateLeft(struct AVLTreeNode *Z);

struct AVLTreeNode* DoubleRotateRight(struct AVLTreeNode* X);

struct AVLTreeNode* insertar(AVLTreeNode* root, AVLTreeNode* parent, char* key, void* value);

void Insert(Map* map, char* key, void* value);

AVLTreeNode* minimum(AVLTreeNode* x);

AVLTreeNode* maximum(AVLTreeNode* x);

struct AVLTreeNode* eliminar(AVLTreeNode* root, char* key);

void Remove(Map* map, char* key);

AVLTreeNode* busqueda(AVLTreeNode* root, char* key);

Pair* Search(Map* map, char* key);

AVLTreeNode* firstUp(Map* map, AVLTreeNode* node, AVLTreeNode* parent);

AVLTreeNode* firstDown(Map* map, AVLTreeNode* node, AVLTreeNode* parent);

Pair* First(Map* map);

Pair* rFirst(Map* map);

Pair* Next(Map* map);

Pair* rNext(Map* map);

#endif /*Map_h */

//joselito