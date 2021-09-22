#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

typedef unsigned long (*funcion_hash_t) (const char *, size_t);

typedef struct cms {
    funcion_hash_t hash1;
    funcion_hash_t hash2;
    funcion_hash_t hash3;
    size_t* arreglo1;
    size_t* arreglo2;
    size_t* arreglo3;
    size_t cantidad1;
    size_t cantidad2;
    size_t cantidad3;
}cms_t;

cms_t* crear_cms();
bool cms_guardar(cms_t* cms, const char* clave);
size_t cms_obtener_cantidad(cms_t* cms, const char *clave);
void cms_destruir(cms_t* cms);