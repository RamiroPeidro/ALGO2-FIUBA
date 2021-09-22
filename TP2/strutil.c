#define STRUTIL_H
#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>


char *substr(const char *str, size_t n){
    char* resultado = strndup(str, n);
    if(!resultado) return NULL;

    return resultado;
}


int cantidad_separadores_split(const char* str, char separador){
    int res = 0;
    for (size_t i = 0; str[i]!='\0'; i++){
        if (str[i] == separador) res++;
    }
    return res;
}

int cantidad_necesaria_split(int separadores){
    return separadores + 2;
}

int longitud_aux(const char* auxiliar, char sep){
    int res = 0;
    int i = 0;
    while(auxiliar[i]!=sep && auxiliar[i]!= '\0'){
        res++;
        i++;
    }
    return res+1;   
    //el +1 es por el \0 que necesita 
}

char** split (const char* str, char sep){

    int separadores = cantidad_separadores_split(str, sep);
    int cant_necesaria = cantidad_necesaria_split(separadores);
    const char* ultima_posicion = str;      
    char** resultado = malloc(sizeof(char*)*(cant_necesaria));

    for (size_t i = 0; i < separadores+1; i++){
        int cant_aux = longitud_aux(ultima_posicion, sep);
        char* auxiliar = strndup(ultima_posicion, cant_aux-1);//el -1 para que no copie el separador
        resultado[i] = auxiliar;
        //actualizo el valor del puntero hasta la posicion despues del separador
        ultima_posicion += cant_aux;
    }
    //agrego el null en la ultima posicion
    resultado[separadores+1] = NULL;
    return resultado;
}


int cantidad_necesaria(char** strv){
    int largo = 0;
    int i = 0;
    for (; strv[i]; i++){
        largo+=(int)strlen(strv[i]);
    }
    return largo;
}

//siempre va a ser la longitud del arreglo -1
int cantidad_separadores(char** strv){
    int i = 0;
    for(; strv[i]; i++){}
    return i-1;
}



char *join(char **strv, char sep){
    if(strv[0] == NULL){
         char* resultado_null = calloc(1, sizeof(char));
         return resultado_null;
    }

    int longitud = cantidad_necesaria(strv);
    int separadores = cantidad_separadores(strv);
    char* ultima_posicion;
    int n = 0;
    char* resultado;

    resultado = calloc((longitud+separadores+1),sizeof(char));
    
    if(!resultado) return NULL;

    for(int i=0; strv[i]; i++){
        if(i==0){
            ultima_posicion = stpcpy(resultado, strv[i]);
        } else {
            if(sep != '\0'){
                ultima_posicion = stpcpy(ultima_posicion+1, strv[i]);   
            } else {
                ultima_posicion = stpcpy(ultima_posicion, strv[i]);
            }
        }
        if(strv[i+1]!=NULL){
            ultima_posicion[i+n] = sep;
            n--;
        }
    }   
    return resultado;
}

void free_strv(char *strv[]){
    for (int i = 0; strv[i]; i++){
        free(strv[i]);
    }
    free(strv);
}
