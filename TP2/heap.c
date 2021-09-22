#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include "heap.h"
#define CAPACIDAD_INICIAL 5000
#define COCIENTE_REDUCCION 4
#define FACTOR_AUMENTO 2

 
void swap(void** x, void** y){
    void* auxiliar = *x;
    *x = *y;
    *y = auxiliar;
}

struct heap{
    void** datos;
    size_t cantidad;
    size_t capacidad;
    cmp_func_t cmp;
};

heap_t *heap_crear(cmp_func_t cmp){
    heap_t* heap = malloc(sizeof(heap_t));
    if(!heap) return NULL;

    void**datos = malloc(sizeof(void*) * CAPACIDAD_INICIAL);    
    if (!datos) {
        free(heap);
        return NULL;
    }

    heap->cantidad = 0;
    heap->capacidad = CAPACIDAD_INICIAL;
    heap->datos = datos;
    heap->cmp = cmp;
    return heap;
}


bool heap_redimensionar(heap_t* heap, size_t nueva_capacidad){
    void** datos_nuevo = realloc(heap->datos, nueva_capacidad * sizeof(void*));

    if (heap->capacidad > 0 && datos_nuevo == NULL){
        return false;
    }

    heap->datos = datos_nuevo;
    heap->capacidad = nueva_capacidad;
    return true;
}


size_t obtener_padre(size_t hijo){
    return (hijo - 1)/2;
}


void upheap(void** datos, size_t hijo, cmp_func_t cmp){
    if(hijo <= 0) return;

    size_t padre = obtener_padre(hijo);
    if(cmp(datos[padre], datos[hijo]) < 0){
        swap(&datos[padre], &datos[hijo]);
        upheap(datos, padre, cmp);
    }
}


bool heap_encolar(heap_t *heap, void *elem){
    if(heap->cantidad == heap->capacidad){
        if(!heap_redimensionar(heap, heap->capacidad * FACTOR_AUMENTO)) return false;
    }

    heap->datos[heap->cantidad] = elem;
    upheap(heap->datos, heap->cantidad, heap->cmp);
    heap->cantidad++;
    return true;
}

size_t maximo(void** datos, cmp_func_t cmp, size_t padre, size_t hijo_izq, size_t hijo_der, size_t cant){
    size_t max = padre;
    if(hijo_izq < cant) (cmp(datos[padre], datos[hijo_izq]) > 0) ? (max = padre) : (max = hijo_izq);
    if(hijo_der < cant) (cmp(datos[max], datos[hijo_der]) > 0) ? (max = max) : (max = hijo_der);
    return max;
}


void downheap(void** datos, size_t cantidad, size_t padre, cmp_func_t cmp){
    if(padre >= cantidad) return;
    
    size_t hijo_izquierdo = (2*padre) + 1;
    size_t hijo_derecho = (2*padre) + 2;
    size_t max = maximo(datos, cmp, padre, hijo_izquierdo, hijo_derecho, cantidad);
    if(max!=padre){
        swap(&datos[max], &datos[padre]);
        downheap(datos, cantidad, max, cmp);
    }
}

void *heap_desencolar(heap_t *heap){
    if(heap_esta_vacio(heap)) return NULL;
    void* retorno = heap->datos[0];
    swap(&heap->datos[0], &heap->datos[heap->cantidad-1]);
    heap->cantidad--;

    if(heap->cantidad * COCIENTE_REDUCCION < heap->capacidad && heap->capacidad > CAPACIDAD_INICIAL){
        if(!heap_redimensionar(heap, heap->capacidad / FACTOR_AUMENTO)) return false;
    }

    downheap(heap->datos, heap->cantidad, 0, heap->cmp);

    return retorno;
}

size_t heap_cantidad(const heap_t *heap){
    return heap->cantidad;
}

bool heap_esta_vacio(const heap_t *heap){
    return heap->cantidad == 0;
}

void *heap_ver_max(const heap_t *heap){
    if(heap_esta_vacio(heap)) return NULL;
    return heap->datos[0];
}


void _heap_destruir(heap_t *heap, void (*destruir_elemento)(void *e), size_t cantidad){
    if(cantidad <= 0){
        free(heap->datos);
        free(heap);
        return;
    }
    destruir_elemento(heap->datos[heap->cantidad - cantidad]);
    _heap_destruir(heap, destruir_elemento, cantidad-1);
}

void heap_destruir(heap_t *heap, void (*destruir_elemento)(void *e)){
    if(!destruir_elemento){
        free(heap->datos);
        free(heap);
        return;
    }
    _heap_destruir(heap, destruir_elemento, heap->cantidad);
}


//recibe un arreglo y lo convierte en heap
void heapify(void** arreglo, size_t cantidad, cmp_func_t cmp){
    for (size_t i = 0; i < cantidad/2; i++){
        downheap(arreglo, cantidad, cantidad/2-i-1, cmp);
    }
}

//crea un heap desde un arreglo
heap_t *heap_crear_arr(void *arreglo[], size_t n, cmp_func_t cmp){
    heap_t* heap = malloc(sizeof(heap_t));
    if(!heap) return NULL;

    heap->datos = malloc(sizeof(void*) * n);
    if(!heap->datos){
        free(heap);
        return NULL;
    }

    heap->cmp = cmp;
    heap->cantidad = n;
    heap->capacidad = n;

    for (size_t i = 0; i < n; i++){
        heap->datos[i] = arreglo[i];
    }
    heapify(heap->datos, n, cmp);
    return heap;
}


void heap_sort(void *elementos[], size_t cant, cmp_func_t cmp){
    heapify(elementos, cant, cmp);

    while (cant>0){
        swap(&elementos[0], &elementos[cant-1]);
        downheap(elementos, cant-1, 0, cmp);
        cant--;
    }

}

