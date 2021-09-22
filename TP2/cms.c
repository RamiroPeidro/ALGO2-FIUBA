#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "cms.h"
#define CAPACIDAD_INICIAL 1000000
#define CANTIDAD_FUNCIONES 3



//link de donde la sacamos: http://profesores.elo.utfsm.cl/~agv/elo320/miscellaneous/hashFunction/hashFunction.html
static unsigned long funcion_hash1(const char *str, size_t tamanio){
    unsigned long hash = 0;
    int c;

    while ((c = *str++)){
        hash = c + (hash << 6) + (hash << 16) - hash;
    }
    return hash % tamanio;
}



unsigned long funcion_hash2(const char *str, size_t tamanio)
    {
        unsigned long hash = 5381;
        int c;

        while ((c = *str++))
            hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

        return hash%tamanio;
    }


unsigned long funcion_hash3(const char *str, size_t tamanio)
    {
	unsigned int hash = 0;
	int c;

	while ((c = *str++))
	    hash += c;

	return hash % tamanio;
    }


cms_t* crear_cms(){
    cms_t* cms = malloc(sizeof(cms_t));
    if (!cms){
        return NULL;
    }
    cms->hash1 = funcion_hash1;
    cms->hash2 = funcion_hash2;
    cms->hash3 = funcion_hash3;

    cms->arreglo1 = calloc((CAPACIDAD_INICIAL/CANTIDAD_FUNCIONES),sizeof(size_t*));
    if (!cms->arreglo1){
        free(cms);
        return NULL;
    }

    cms->arreglo2 = calloc((CAPACIDAD_INICIAL/CANTIDAD_FUNCIONES),sizeof(size_t*));
    if (!cms->arreglo2){
        free(cms);
        return NULL;
    }

    cms->arreglo3 = calloc((CAPACIDAD_INICIAL/CANTIDAD_FUNCIONES),sizeof(size_t*));
    if (!cms->arreglo3){
        free(cms);
        return NULL;
    }

    cms->cantidad1 = 0;
    cms->cantidad2 = 0;
    cms->cantidad3 = 0;
    return cms;
}

bool cms_guardar(cms_t* cms, const char* clave){

    long unsigned int pos1 = cms->hash1(clave, CAPACIDAD_INICIAL/CANTIDAD_FUNCIONES);
    long unsigned int pos2 = cms->hash2(clave, CAPACIDAD_INICIAL/CANTIDAD_FUNCIONES);
    long unsigned int pos3 = cms->hash3(clave, CAPACIDAD_INICIAL/CANTIDAD_FUNCIONES);

    if(cms->cantidad1 == 0) {
        cms->arreglo1[pos1] = 1;
    }else {
        cms->arreglo1[pos1]++;
    }

    if(cms->cantidad2 == 0) {
        cms->arreglo2[pos2] = 1;
    }else {
        cms->arreglo2[pos2]++;
    }

    if(cms->cantidad3 == 0) {
        cms->arreglo3[pos3] = 1;
    } else {
        cms->arreglo3[pos3]++;
    }

    cms->cantidad1++;
    cms->cantidad2++;
    cms->cantidad3++;
    return true;
}

size_t cms_obtener_cantidad(cms_t* cms, const char *clave){   
    size_t primera = cms->arreglo1[cms->hash1(clave, CAPACIDAD_INICIAL/CANTIDAD_FUNCIONES)];
    size_t segunda = cms->arreglo2[cms->hash2(clave, CAPACIDAD_INICIAL/CANTIDAD_FUNCIONES)];
    size_t tercera = cms->arreglo3[cms->hash3(clave, CAPACIDAD_INICIAL/CANTIDAD_FUNCIONES)];
    
    size_t min = primera;

    segunda<min ? (min = segunda) : (min = primera);
    tercera<min ? (min =tercera) : (min = min);

    return min;
}


void cms_destruir(cms_t* cms){
    free(cms->arreglo1);
    free(cms->arreglo2);
    free(cms->arreglo3);
    free(cms);
}


