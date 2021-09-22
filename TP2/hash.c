#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "hash.h"
#include <string.h>
#define TAMANIO_INICIAL 1001
#define FACTOR_CARGA 0.20
#define REDIMENSION 2
#define COCIENTE_REDUCCION 4

typedef enum estado {
    OCUPADO, 
    VACIO,
    BORRADO, 
} estado_t;

typedef struct hash_campo {
	char* clave;
	void* dato; 
	estado_t estado;
} campo_t;

struct hash {
	size_t cantidad;
	size_t tamanio;
	hash_destruir_dato_t destruir_dato;
	campo_t* tabla;
};

struct hash_iter {
	const hash_t* hash;
	size_t indice;
};


long unsigned int pertenece_aux(const hash_t *hash, const char *clave);

bool redimensionar_aumento(hash_t *hash, double factor_carga) {
    if ((float) hash->cantidad / (float) hash->tamanio >= (float) FACTOR_CARGA){
        return true;
    }
    return false;
}


//link de donde la sacamos: http://profesores.elo.utfsm.cl/~agv/elo320/miscellaneous/hashFunction/hashFunction.html
static unsigned long funcion_hash(const char *str, size_t tamanio){
    unsigned long hash = 0;
    int c;

    while ((c = *str++)){
        hash = c + (hash << 6) + (hash << 16) - hash;
    }
    return hash % tamanio;
}


campo_t* campo_crear(size_t talla){
    campo_t* campo = malloc(sizeof(campo_t) * talla);
    if (campo == NULL){
        return NULL;
    }
    for (int i = 0; i < talla; i++){
        campo[i].clave = NULL;
        campo[i].dato = NULL;
        campo[i].estado = VACIO;
    }
    return campo;
}


bool hash_redimensionar(hash_t *hash, size_t tamanio_nuevo){
    size_t tamanio_auxiliar = 0;
    size_t i = 0;
    campo_t* antiguo = hash->tabla;
    campo_t* nuevo = campo_crear(tamanio_nuevo);
    if (nuevo == NULL){
        return false;
    }
    tamanio_auxiliar = hash->tamanio;
    hash->tamanio = tamanio_nuevo;
    hash->tabla = nuevo;
    hash->cantidad = 0;

    //vuelvo a hashear todos los elementos
    while (i < tamanio_auxiliar){
        if (antiguo[i].estado == OCUPADO){
            hash_guardar(hash, antiguo[i].clave, antiguo[i].dato);
            free(antiguo[i].clave);
        }
        i++;
    }
    free(antiguo);
    return true;
}



hash_t *hash_crear(hash_destruir_dato_t destruir_dato){
    hash_t* hash = malloc(sizeof(hash_t));

    if (hash == NULL){
        return NULL;
    }

    hash->tabla = campo_crear(TAMANIO_INICIAL);

    if (hash->tabla == NULL){
        free(hash);
        return NULL;
    }

    hash->tamanio = TAMANIO_INICIAL;
    hash->cantidad = 0;
    hash->destruir_dato = destruir_dato;
    return hash;
}


size_t guardar_aux(const hash_t * hash, size_t posicion_actual, const char* clave){
    size_t pos = posicion_actual;

    while(pos < hash->tamanio){  
        if(hash->tabla[pos].estado == VACIO){
            return pos;
        } 
        else if(hash->tabla[pos].estado != BORRADO && strcmp(hash->tabla[pos].clave, clave) == 0 && hash->tabla[pos].estado == OCUPADO){
            return pos;
        }
        pos++;
        if(pos == hash->tamanio){
            pos = 0;
        }       
    }
    return pos;
}

bool hash_guardar(hash_t *hash, const char *clave, void *dato){
    if (hash == NULL || clave == NULL){
        return false;
    }

    if(redimensionar_aumento(hash, FACTOR_CARGA)){
       if(hash_redimensionar(hash, hash->tamanio * REDIMENSION) == false) return false;
    }

    size_t posicion_hash = funcion_hash(clave, hash->tamanio);

    if (hash->tabla[posicion_hash].estado == VACIO){
        hash->tabla[posicion_hash].clave = strdup(clave);
        hash->tabla[posicion_hash].dato = dato;
        hash->tabla[posicion_hash].estado = OCUPADO;
        hash->cantidad++;
        
    } 
    else if(hash->tabla[posicion_hash].estado == OCUPADO && strcmp(hash->tabla[posicion_hash].clave, clave)==0 ) {
        if(hash->destruir_dato){
            hash->destruir_dato(hash->tabla[posicion_hash].dato);
        }
        hash->tabla[posicion_hash].dato = dato;
    }   

    else{
        size_t pos_aux = guardar_aux(hash, posicion_hash, clave);

        if(hash->tabla[pos_aux].estado ==VACIO){
            hash->tabla[pos_aux].clave = strdup(clave);
            hash->tabla[pos_aux].dato = dato;
            hash->tabla[pos_aux].estado = OCUPADO;
            hash->cantidad++;
        }
        
        else if(hash->tabla[pos_aux].estado == OCUPADO && strcmp(hash->tabla[pos_aux].clave, clave) == 0){
            if(hash->destruir_dato){
                hash->destruir_dato(hash->tabla[pos_aux].dato);
            }   
            hash->tabla[pos_aux].dato = dato;
        }
    }    
    return true;
}

void *hash_borrar(hash_t *hash, const char *clave){

    if (!hash_pertenece(hash, clave)) return NULL;


    size_t posicion = pertenece_aux(hash, clave);
    

    void* valor_aux = hash->tabla[posicion].dato;
    hash->cantidad--;
    hash->tabla[posicion].estado = BORRADO;
    

    if (hash->tamanio <= hash->cantidad * COCIENTE_REDUCCION){
       if(!hash_redimensionar(hash, hash->tamanio / REDIMENSION)) return NULL;
    }   

    
    char* clave_des = hash->tabla[posicion].clave;
    free(clave_des);
    
    return valor_aux;
}

//devuelve la posicion en la que se encuentra la clave buscada, caso contraio devuelve -1;
long unsigned int pertenece_aux(const hash_t *hash, const char *clave){
    size_t posicion_aux = funcion_hash(clave, hash->tamanio);
    size_t i = 0;

    while(i < hash->tamanio){
        if(hash->tabla[posicion_aux].clave == NULL) return -1;

        if(hash->tabla[posicion_aux].estado == OCUPADO && strcmp(hash->tabla[posicion_aux].clave, clave) == 0) return posicion_aux;
        posicion_aux++;
        i++;
    }
    return -1;
}

void *hash_obtener(const hash_t *hash, const char *clave){
    if (!hash_pertenece(hash, clave)){
        return NULL;
    }

    size_t posicion = funcion_hash(clave, hash->tamanio);
    if(hash->tabla[posicion].clave == clave) {
        return hash->tabla[posicion].dato;
    } else{
        return hash->tabla[pertenece_aux(hash, clave)].dato;
    }
}


bool hash_pertenece(const hash_t *hash, const char *clave){
    size_t posicion = funcion_hash(clave, hash->tamanio);

    if(hash->tabla[posicion].clave == NULL) return false;

    return (pertenece_aux(hash, clave) != -1);
}    


size_t hash_cantidad(const hash_t *hash){
    return hash->cantidad;
}

void hash_destruir(hash_t *hash){
    size_t i = 0;
    if (hash_cantidad(hash) != 0){
        while(i < hash->tamanio){
            if (hash->tabla[i].estado == OCUPADO){
                if (hash->destruir_dato){
                    hash->destruir_dato(hash->tabla[i].dato);
                }
                free(hash->tabla[i].clave);
            }
            i++;
        } 
    }
    free(hash->tabla);
    free(hash);
}


//funcion auxiliar
//avanza el iterador hasta una posicion ocupada o hasta el final en caso contrario

long int hash_posicionar_iterador_aux(hash_iter_t *iter){
    while(iter->hash->tamanio > iter->indice){
        if (iter->hash->tabla[iter->indice].estado == OCUPADO) return iter->indice;
        iter->indice++;
    }
    return -1;
}

long int hash_avanzar_aux(hash_iter_t *iter, unsigned long int pos_actual){
    pos_actual++;
    while(iter->hash->tamanio > pos_actual){
        if (iter->hash->tabla[pos_actual].estado == OCUPADO){
             return pos_actual;
        }
        pos_actual++;
    }
    return -1;
}

hash_iter_t *hash_iter_crear(const hash_t *hash){
    hash_iter_t *iter = malloc(sizeof(hash_iter_t));
    if(!iter) return NULL;

    iter->hash = hash;
    iter->indice = 0;

    long unsigned int indice_aux = hash_posicionar_iterador_aux(iter);


    (indice_aux != -1) ? (iter->indice = indice_aux) : (iter->indice = iter->hash->tamanio);

    
    return iter;
}


bool hash_iter_al_final(const hash_iter_t *iter){
    return iter->indice == iter->hash->tamanio;
}

bool hash_iter_avanzar(hash_iter_t *iter){
    if(hash_iter_al_final(iter)) return false;

    unsigned long int pos_actual = iter->indice;
    unsigned long int nueva_pos = hash_avanzar_aux(iter, pos_actual);

    (nueva_pos != -1) ? (iter->indice = nueva_pos) : (iter->indice = iter->hash->tamanio);

    return true;
}

const char *hash_iter_ver_actual(const hash_iter_t *iter){
    if(hash_iter_al_final(iter)) return NULL;

    return iter->hash->tabla[iter->indice].clave;
}


void hash_iter_destruir(hash_iter_t *iter){
    free(iter);
}