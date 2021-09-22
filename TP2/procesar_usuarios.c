#define _POSIX_C_SOURCE 200809L
#include "heap.h"
#include "hash.h"
#include "strutil.h"
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

typedef struct tag_apariciones {
    size_t cantidad;
    hash_t* hash;
}tag_apariciones_t;


tag_apariciones_t* crear_tag_apariciones(){
    tag_apariciones_t* tag_apariciones = malloc(sizeof(tag_apariciones_t));
    if(!tag_apariciones) return NULL;

    tag_apariciones->cantidad = 0;
    hash_t* hash = hash_crear(NULL);
    tag_apariciones->hash = hash;
    return tag_apariciones;
}


void destruir_total(void* tag){
    tag_apariciones_t* tag2 = (tag_apariciones_t*)tag;
    hash_destruir(tag2->hash);
    free(tag);
}



bool tiene_fin_de_linea(char* linea){
    int i = 0;
    while(linea[i]){
        if(linea[i] == '\n')return true;
        i++;
    }
    return false;
}

void cambiar_caracter_final(char* linea){
    size_t i = 0;
    while(linea[i] != '\n'){
        i++;
    }
    linea[i] = '\0';
}

char* ignorar_autor(char* linea){
    char** splitted = split(linea, ',');
    char** resultado = splitted +1;
    char* res = join(resultado, ',');
    free_strv(splitted);
    return res;
}

size_t tamanio_arreglo(char** arreglo){
    size_t i=0;
    while(arreglo[i]){
        i++;
    }
    return i;
}

void procesar_apariciones(hash_t* hash, char** arreglo, size_t n, char* autor){
    for (size_t i = 0; i < n; i++){
        int x = 0; //es indiferente que guardar en el dato

        if(hash_pertenece(hash, autor)){
            tag_apariciones_t* tag_guardado = (tag_apariciones_t*)hash_obtener(hash, autor);

            hash_t* hash_guardado = tag_guardado->hash;         
            hash_guardar(hash_guardado, arreglo[i], &x);

            tag_guardado->cantidad = hash_cantidad(hash_guardado);
        }
        else{
            tag_apariciones_t* tag_nuevo = crear_tag_apariciones();
            hash_guardar(tag_nuevo->hash, arreglo[i], &x);
            tag_nuevo->cantidad = hash_cantidad(tag_nuevo->hash);
            hash_guardar(hash, autor, tag_nuevo);
        }
    }
    
}  

size_t obtener_cantidad(hash_t* hash, char* autor){
    tag_apariciones_t* tag = (tag_apariciones_t*)hash_obtener(hash, autor);
    return tag->cantidad;
}


void procesar_usuarios(char* linea, hash_t* hash, hash_t* hash_aux){
    char** linea_spliteada = split(linea, ',');

    char* linea_sin_autor = ignorar_autor(linea);
    char** arreglo_apariciones = split(linea_sin_autor, ',');
    size_t tamanio = tamanio_arreglo(arreglo_apariciones);
    char* autor = linea_spliteada[0];

    procesar_apariciones(hash_aux, arreglo_apariciones, tamanio, autor);

    size_t cantidad_tw = obtener_cantidad(hash_aux, autor);
    size_t nueva_cantidad = cantidad_tw;
    size_t* cantidad_a_guardar = malloc(sizeof(size_t));
    *cantidad_a_guardar = nueva_cantidad;
    hash_guardar(hash, autor, cantidad_a_guardar);

    free(linea_sin_autor);
    free_strv(linea_spliteada);
    free_strv(arreglo_apariciones);
}


size_t obtener_tamanio(tags_t** arreglo){
    size_t i=0;
    while(arreglo[i]!=NULL){
        i++;
    }
    return i;
}


void imprimir_apariciones(tags_t** arreglo, size_t n){

    fprintf(stdout, "%li: %s", arreglo[0]->cantidad, arreglo[0]->cadena);
    size_t k = arreglo[0]->cantidad;
    for (size_t i = 1; i < n; i++){
        if(arreglo[i]->cantidad == k){
            fprintf(stdout, ", %s", arreglo[i]->cadena);
        } else{
            fprintf(stdout, "\n");
            fprintf(stdout, "%li: %s", arreglo[i]->cantidad, arreglo[i]->cadena);
            k = arreglo[i]->cantidad;
        }
    }
    fprintf(stdout, "\n");
}



tags_t** almacenar_tags(hash_t* hash){
    tags_t** arreglo_tags = calloc(hash_cantidad(hash)+1, sizeof(tags_t*));

    hash_iter_t* iter = hash_iter_crear(hash);
    size_t i =0;
    while(!hash_iter_al_final(iter)){

        const char* actual = hash_iter_ver_actual(iter);

        char* copia = strdup(actual); //liberarla

        size_t cantidad = *(size_t*) hash_obtener(hash, copia);
        tags_t* tag = crear_tag(copia, cantidad);
        arreglo_tags[i] = tag;
        hash_iter_avanzar(iter);
        i++;   
    }
    hash_iter_destruir(iter);
    return arreglo_tags;
}


tags_t** counting_sort_apariciones(tags_t** arreglo, int n, int min, int max){
    int k = max -min;

    int contador[k+1];
    for(int i = 0; i < (k + 1); i++) contador[i] = 0;
    for (int i = 0; i < n; i++){
        size_t numero = arreglo[i]->cantidad;
        contador[numero-min]++;
    }

    int suma_acumulada[k+1];
    suma_acumulada[0] = 0;
    for (int i = 1; i < (k+1); i++){
        int cantidad = contador[i-1];
        suma_acumulada[i] = suma_acumulada[i-1] + cantidad;
    }

    tags_t** resultado = calloc(n,sizeof(tags_t*));
    if(!resultado) return NULL;

    for (int i = 0; i < n; i++){
        tags_t* valor_tag = arreglo[i];
        size_t valor = arreglo[i]->cantidad;

        int pos = suma_acumulada[valor-min];
        suma_acumulada[valor-min]++;
        resultado[pos] = valor_tag;
    }
    return resultado;
}



typedef int (*convertir_string)(char* a, size_t n);

int convertir_st(char* string, size_t n){
    int res = (int)string[n];
    return res;
}


void counting_sort_alfabeticamente(tags_t** arreglo, int n, int min, int max, convertir_string convertir, size_t l){
    int k = max -min;

    int contador[k+1];
    for(int i=0; i <(k+1); i++) contador[i] = 0;
    
    for (int i = 0; i < n; i++){
        size_t numero = convertir(arreglo[i]->cadena, l);
        contador[numero-min]++;
    }


    int suma_acumulada[k+1];
    suma_acumulada[0] = 0;
    for (int i = 1; i < (k+1); i++){
        int cantidad = contador[i-1];
        suma_acumulada[i] = suma_acumulada[i-1] + cantidad;
    }
    
    tags_t** resultado = calloc(n+1, sizeof(tags_t*));
    if(!resultado) return ;
    for (int i = 0; i < n; i++){
        tags_t* valor_tag = arreglo[i];
        size_t valor = convertir(arreglo[i]->cadena, l);
        int pos = suma_acumulada[valor-min];
        suma_acumulada[valor-min]++;
        resultado[pos] = valor_tag;
    }
    //free(arreglo);

    for (size_t i = 0; i < n; i++){
        arreglo[i] = resultado[i];
    }
    
    free(resultado);
}


int obtener_maximo(tags_t** arreglo, int tamanio){
    int maximo = (int)arreglo[0]->cantidad;
    for (size_t i = 0; i < tamanio; i++){
        if(arreglo[i]->cantidad > maximo) maximo = (int)arreglo[i]->cantidad;
    }
    return maximo;
}

char* agregar_asteriscos(char* string){
    int i = 0;
    char* resultado = calloc(16, sizeof(char));

    while(string[i] != '\0') {
        resultado[i] = string[i];
        i++;
    }

    while (i<15){
        resultado[i] ='*';
        i++;
    }

    resultado[i] = '\0';
    free(string);
    return resultado;
}



void completar_caracteres(tags_t** arreglo, int n){
    
    for (size_t i = 0; i < n; i++){
        if(strlen(arreglo[i]->cadena) < 15){
            char* elemento_nuevo = agregar_asteriscos(arreglo[i]->cadena);
            arreglo[i]->cadena = elemento_nuevo;
        }
    }
}

tags_t** radix_alfabeticamente(tags_t** arreglo, int n){
    completar_caracteres(arreglo, n);
    
    for (int i = 14; i >= 0; i--){
        counting_sort_alfabeticamente (arreglo, n, 0, 255, convertir_st, i);
    }
    return arreglo;
}


void eliminar_asteriscos_aux(char* string){
    int i = 0;
    while(string[i] != '*') i++;
    string[i] = '\0';
}

void elimimnar_asteriscos(tags_t** arreglo, int n){
    for (size_t i = 0; i < n; i++){
        eliminar_asteriscos_aux(arreglo[i]->cadena);
    }
}

void destruccion();


int main(int argc, char const *argv[]){
    FILE* archivo = fopen(argv[1], "r");
    if(!archivo) return -1;

    char* linea = NULL;
    size_t capacidad = 0;
    hash_t* hash = hash_crear(free);
    hash_t* hash_aux = hash_crear(destruir_total);
    char* copia_linea;

    while ((getline(&linea, &capacidad, archivo)) != EOF){
        copia_linea = strdup(linea);

        if(tiene_fin_de_linea(copia_linea)){
            cambiar_caracter_final(copia_linea);
        }
        procesar_usuarios(copia_linea, hash, hash_aux);
        free(copia_linea);  
    }

    tags_t** arreglo_tags = almacenar_tags(hash);
    int tamanio_arreglo_tags = (int)obtener_tamanio(arreglo_tags);

    int maximo = obtener_maximo(arreglo_tags, tamanio_arreglo_tags);

    tags_t** ordenado_alfabeticamente = radix_alfabeticamente(arreglo_tags, tamanio_arreglo_tags);

    tags_t** ordenado = counting_sort_apariciones(ordenado_alfabeticamente, tamanio_arreglo_tags, 0, maximo);
    elimimnar_asteriscos(ordenado, tamanio_arreglo_tags);
    imprimir_apariciones(ordenado, tamanio_arreglo_tags);
    
    free(linea);
    fclose(archivo);

    for (size_t i = 0; i < tamanio_arreglo_tags; i++){
        free(arreglo_tags[i]->cadena);
        free(arreglo_tags[i]);
    }

    free(ordenado);
    free(ordenado_alfabeticamente);
    //free(arreglo_tags);


    hash_destruir(hash_aux);
    hash_destruir(hash);
    return 0;
}
