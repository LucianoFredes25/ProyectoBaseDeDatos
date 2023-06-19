#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
#include "map.h"

typedef struct AVLTreeNode {
    Pair* pair;
    int height;
    struct AVLTreeNode * left;
    struct AVLTreeNode * right;
    struct AVLTreeNode * parent;
}AVLTreeNode;

typedef struct Map {
    AVLTreeNode * root;
    AVLTreeNode * current;
}Map;

int Height(AVLTreeNode* root){
  if(!root) return -1;
  return root->height;
}

int getBalance(AVLTreeNode* root){
    if (root == NULL) return 0;
    return Height(root->left) - Height(root->right);
}

int max(int a, int b){
  if(a>b) return a;
  return b;
}

Map* createMap(){
  Map* new = (Map*)calloc(1, sizeof(Map));
  return new;
}

struct AVLTreeNode* createTreeNode(char* key, void* value){
  AVLTreeNode* new = (AVLTreeNode*) calloc(1, sizeof(AVLTreeNode) );
  new->pair = (Pair*) calloc(1, sizeof(Pair) );
  new->pair->key = key;
  new->pair->value = value;
  new->parent = new->left = new->right = NULL;
  new->height = 0;
  return new;
}

struct AVLTreeNode* SingleRotateLeft(struct AVLTreeNode* X){
  struct AVLTreeNode* W = X->left;
  X->left = W->right;
  if (W->right != NULL) {
    W->right->parent = X;
  }
  W->right = X;
  W->parent = X->parent;
  X->parent = W;
  X->height = max(Height(X->left), Height(X->right)) + 1;
  W->height = max(Height(W->left), X->height) + 1;
  return W;
}

struct AVLTreeNode* SingleRotateRight(struct AVLTreeNode* W){
  struct AVLTreeNode* X = W->right;
  W->right = X->left;
  if (X->left != NULL) {
    X->left->parent = W;
  }
  X->left = W;
  X->parent = W->parent;
  W->parent = X;
  W->height = max(Height(W->left), Height(W->right)) + 1;
  X->height = max(Height(X->right), W->height) + 1;
  return X;
}


struct AVLTreeNode* DoubleRotateLeft(struct AVLTreeNode *Z){
  Z->left = SingleRotateRight(Z->left);
  return SingleRotateLeft(Z);
}

struct AVLTreeNode* DoubleRotateRight(struct AVLTreeNode* X){
  X->right = SingleRotateLeft(X->right);
  return SingleRotateRight(X);
}

struct AVLTreeNode* insertar(AVLTreeNode* root, AVLTreeNode* parent, char* key, void* value){

  if(!root){
    
    root = createTreeNode(key, value);
    root->parent = parent;
    
  }else if( strcmp(key, root->pair->key ) < 0){

    root->left = insertar(root->left, root, key, value);
    if ( abs(Height(root->right) - Height(root->left)) == 2){
      if( strcmp( key, root->left->pair->key ) < 0) root = SingleRotateLeft(root);
      else root = DoubleRotateLeft(root);
    }
    
  }else if( strcmp(key, root->pair->key) > 0){

    root->right = insertar(root->right, root, key, value);
    
    if( abs(Height(root->right) - Height(root->left)) == 2 ){
      if( strcmp(key, root->right->pair->key) > 0) root = SingleRotateRight(root);
      else root = DoubleRotateRight(root);
    }
    
  }
  root->height = max( Height(root->right), Height(root->left) ) + 1;
  return root;
}


void Insert(Map* map, char* key, void* value){
  map->root = insertar(map->root, NULL, key, value);
}

AVLTreeNode* minimum(AVLTreeNode* x){
  if(x->left == NULL ) return x;
  else return minimum(x->left);
}

AVLTreeNode* maximum(AVLTreeNode* x){
  if(x->right == NULL) return x;
  else return maximum(x->right);
}

struct AVLTreeNode* eliminar(AVLTreeNode* root, char* key){

  if(root == NULL) return root;

  if( strcmp(key, root->pair->key) < 0) root->left = eliminar(root->left, key);
  else if(strcmp(key, root->pair->key) > 0) root->right = eliminar(root->right, key);
  else{

    if( (root->left == NULL) || (root->right == NULL) ){
      AVLTreeNode* temp =  (root->left)? root->left : root->right;

      if(temp == NULL){
        temp = root;
        root = NULL;
      }else{ 
        AVLTreeNode* parent = root->parent;
        *root = *temp;
        root->parent = parent;
      }
      
      free(temp);
      
    }else{
      AVLTreeNode* temp = minimum(root->right);
      root->pair = temp->pair; 
      root->right = eliminar(root->right, temp->pair->key);
    }
    
  }

  if(root == NULL) return root;

  root->height = max( Height(root->right), Height(root->left) ) + 1;

  int aux = getBalance(root);

  if(aux > 1){
    if(getBalance(root->left) >= 0) root = SingleRotateLeft(root);
    else root = DoubleRotateLeft(root);
  }else if(aux < -1){
    if(getBalance(root->right) <= 0) root = SingleRotateRight(root);
    else root = DoubleRotateRight(root);
  }

  return root;
  
}

void Remove(Map* map, char* key){
  map->root = eliminar(map->root, key);
}

AVLTreeNode* busqueda(AVLTreeNode* root, char* key){
  if(root == NULL) return NULL;
  
  if( strcmp(key, root->pair->key) == 0){
    return root;
  }
  
  if( strcmp(key, root->pair->key) < 0) return busqueda(root->left, key);
  else return busqueda(root->right, key);
  
}

Pair* Search(Map* map, char* key){
  AVLTreeNode* oie = busqueda(map->root, key);
  if(oie ==  NULL)return NULL;
  return oie->pair;
}

AVLTreeNode* firstUp(Map* map, AVLTreeNode* node, AVLTreeNode* parent){
  if(parent == NULL) return NULL;

  if( strcmp(node->pair->key, parent->pair->key) > 0 ) return firstUp(map, node, parent->parent);
  else return parent;
}

AVLTreeNode* firstDown(Map* map, AVLTreeNode* node, AVLTreeNode* parent){
  if(parent == NULL) return NULL;

  if( strcmp(node->pair->key, parent->pair->key) < 0 ) return firstDown(map, node, parent->parent);
  else return parent;
}


Pair* First(Map* map){
  AVLTreeNode* it = map->root;
  if(it == NULL) return NULL;
  map->current = minimum(it);
  return map->current->pair;
}

Pair* rFirst(Map* map){
  AVLTreeNode* it = map->root;
  if(it == NULL) return NULL;
  map->current = maximum(it);
  return map->current->pair;
}

Pair* Next(Map* map){
  if(map->current->right != NULL){
    map->current = minimum(map->current->right);
  }else{
    map->current = firstUp(map, map->current, map->current->parent);
  }

  if(map->current == NULL) return NULL;
  return map->current->pair;
}

Pair* rNext(Map* map){

  if(map->current->left != NULL){
    map->current = maximum(map->current->left);
  }else{
    map->current = firstDown(map, map->current, map->current->parent);
  }

  if(map->current == NULL) return NULL;
  return map->current->pair;
  
}