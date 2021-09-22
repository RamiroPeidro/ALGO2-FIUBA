#define _POSIX_C_SOURCE 200809L
#include "heap.h"
#include "hash.h"
#include "strutil.h"
#include "cms.h"
#include <stdlib.h>
#include "strutil.h"
#include <stdio.h>
#include <string.h>
#define _GNU_SOURCE
#include <string.h> 

typedef struct tags {
    char* cadena;
    size_t cantidad;
}tags_t;


tags_t* crear_tag(char* string, size_t n){
    tags_t* tag = malloc(sizeof(tags_t));
    if(!tag) return NULL;

    tag->cadena = string;
    tag->cantidad = n;

    return tag;
}


size_t tamanio_arreglo(char** arreglo){
    size_t i=0;
    while(arreglo[i]){
        i++;
    }
    return i;
}

size_t tamanio_arreglo_aux(void** arreglo){
    size_t i=0;
    while(arreglo[i]!=NULL){
        i++;
    }
    return i;
}

void almacenar_tags(char** arreglo, cms_t* cms, size_t n){
    for (size_t i = 0; i<n; i++){
        cms_guardar(cms, arreglo[i]);
    }
}


int funcion_comparacion(const void* a, const void* b){
    tags_t tag1 = *(tags_t*)a;
    tags_t tag2 = *(tags_t*)b;
    
    if(tag1.cantidad == tag2.cantidad){
        return strcmp(tag1.cadena, tag2.cadena) == 0 ? 0 : ( strcmp(tag1.cadena, tag2.cadena) > 0 ? -1 : 1);
    }
    return tag1.cantidad > tag2.cantidad ? 1 : -1; 
}


void imprimir_k_mayores(heap_t* heap, size_t k, size_t n_linea){
    fprintf(stdout, "--- %li\n", n_linea);
	for (size_t i = 0; i < k; i++){
        if(heap_esta_vacio(heap)) break;
        tags_t* tag = (tags_t*)heap_desencolar(heap);
        fprintf(stdout, "%li %s\n", tag->cantidad, tag->cadena);
        free(tag);
    }
}

void cambiar_caracter_final(char* linea){
    size_t i = 0;
    while(linea[i] != '\n'){
        i++;
    }
    linea[i] = '\0';
}

bool tiene_fin_de_linea(char* linea){
    int i = 0;
    while(linea[i]){
        if(linea[i] == '\n')return true;
        i++;
    }
    return false;
}

void procesar_tweets(cms_t *cms, char* linea, size_t k, size_t n_linea){
    
    char** linea_spliteada = split(linea, ',');

    size_t tamanio = tamanio_arreglo(linea_spliteada);


    void** arreglo_aux = calloc(tamanio+1, sizeof(void*));

    almacenar_tags(linea_spliteada, cms, tamanio);

    hash_t* hash = hash_crear(NULL);
    
    size_t pos = 0;
    for (size_t i = 0; i< tamanio; i++){
        if(hash_pertenece(hash, linea_spliteada[i])){
            continue;
        }
        tags_t* tag = crear_tag(linea_spliteada[i], cms_obtener_cantidad(cms, linea_spliteada[i]));
        hash_guardar(hash, tag->cadena, &tag->cantidad);
        arreglo_aux[pos] = tag;
        pos++;
    }

    size_t tamanio_aux = tamanio_arreglo_aux(arreglo_aux);

    heap_t* heap = heap_crear_arr(arreglo_aux, tamanio_aux, funcion_comparacion);

    imprimir_k_mayores(heap, k, n_linea);
    free(arreglo_aux);
    heap_destruir(heap, free);
    hash_destruir(hash);    
    free_strv(linea_spliteada);
}


char* ignorar_autor(char* linea){
    char** splitted = split(linea, ',');
    char** resultado = splitted +1;
    char* res = join(resultado, ',');
    free_strv(splitted);
    return res;
}

char** sobrantes(char** arreglo, int contador){
    char** resultado = calloc(contador+1,sizeof(char*));
    for (size_t i = 0; i < contador; i++){
        resultado[i] = arreglo[i];
    }
    //resultado[contador] = NULL;
    return resultado;
}
    


int main(int argc, char const *argv[]) {

    char* linea = NULL;
    int contador = 0;
    size_t memoria = atoi(argv[1])%2==0? atoi(argv[1]) : atoi(argv[1])+1;
    char** arreglo = calloc(memoria+1, sizeof(char*));
    size_t capacidad = 0;
    cms_t* cms = crear_cms();
    char* copia_linea;
    size_t n_linea = 1;
    while ((getline(&linea, &capacidad, stdin)) != EOF){
        copia_linea = strdup(linea);

        if(tiene_fin_de_linea(copia_linea)){
            cambiar_caracter_final(copia_linea);
        }
        
        char* linea_final = ignorar_autor(copia_linea);

        arreglo[contador] = linea_final;

        if(contador == atoi(argv[1])-1){
            char* tw = join(arreglo, ',');
            procesar_tweets(cms, tw, atoi(argv[2]), n_linea);
            n_linea++; 

            size_t tamanio_arr = tamanio_arreglo(arreglo);

            for (size_t i = 0; i < tamanio_arr; i++){
                free(arreglo[i]);
            }

            contador = -1;
            free(tw);
        }
        contador++;
        free(copia_linea);
    }


    //si quedaron elementos en el arreglo
    if(contador > 0){
        char** tw_sobrantes = sobrantes(arreglo, contador);
        char* tw = join(tw_sobrantes, ',');
        procesar_tweets(cms, tw, atoi(argv[2]), n_linea);
        free(tw);
        free(tw_sobrantes);
        
    }

    for (size_t i = 0; i < contador; i++){
        free(arreglo[i]);
    }

    free(arreglo);
    free(linea);
    cms_destruir(cms);
    return 0;
}

