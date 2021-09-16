#include "cola.h"
#include <stdlib.h>
#include <stdbool.h>

typedef struct nodo{
    void *dato;
    struct nodo *sig;
}nodo_t;

struct cola{
    nodo_t *prim;
    nodo_t *ult;
};


nodo_t* crear_nodo(void* dato){
    nodo_t* nuevo = malloc(sizeof(nodo_t));
    if(nuevo == NULL){
        return NULL;
    }
    nuevo->dato = dato;
    nuevo->sig = NULL;
    return nuevo;
}

void nodo_destruir(nodo_t* nodo){
    free(nodo);
}


cola_t *cola_crear(void){
    cola_t *cola = malloc(sizeof(cola_t));
    if (cola == NULL){
        return NULL;
    }
    cola->prim = NULL;
    cola->ult = NULL;
    return cola;
}

bool cola_esta_vacia(const cola_t *cola){
    return (cola->prim == NULL);
}

bool cola_encolar(cola_t *cola, void *valor){
    nodo_t* nuevo = crear_nodo(valor);
    if (nuevo == NULL){
        return false;
    }

    if(cola_esta_vacia(cola)){
        cola->prim = nuevo;
    } else {
        cola->ult->sig =nuevo;
    }
    cola->ult =nuevo;
    return true;
}   

void *cola_ver_primero(const cola_t *cola){
    if(cola_esta_vacia(cola)){
        return NULL;
    }
    return cola->prim->dato;
}

void *cola_desencolar(cola_t *cola){
    if (cola_esta_vacia(cola)){
        return NULL;
    }
    void* valor = cola_ver_primero(cola);
    nodo_t* nodo_a_destruir = cola->prim;
    cola->prim = cola->prim->sig;
    if(!cola->prim) cola->ult = NULL;
    nodo_destruir(nodo_a_destruir);
    return valor;    
}

void cola_destruir(cola_t *cola, void (*destruir_dato)(void*)){
    while(!cola_esta_vacia(cola)){
        void* valor = cola_desencolar(cola);
        if (destruir_dato){
            destruir_dato(valor);
        }
        
    }free(cola);
}
