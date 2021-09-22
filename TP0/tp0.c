#include "tp0.h"
#include <stdio.h>
#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define MIN(x, y) (((x) < (y)) ? (x) : (y))


/* *****************************************************************
 *                     FUNCIONES A COMPLETAR                       *
 *         (ver en tp0.h la documentación de cada función)         *
 * *****************************************************************/

void swap(int *x, int *y) {
     int aux = *x;
    *x = *y;
    *y = aux;
}


int maximo(int vector[], int n) {
    if (n==0){
        return -1;
    }

    int maximo = vector[0];
    int resultado = 0;
    for (int i = 0; i < n; i++){
        if (vector[i]>maximo){
            maximo = vector[i];
            resultado = i;
        }
    }
    return resultado;
}


int comparar(int vector1[], int n1, int vector2[], int n2) {
    int minimo = MIN(n1, n2);
    for (int i = 0; i < minimo; i++){
        if (vector1[i]>vector2[i]){
            return 1;
        }else if (vector1[i]<vector2[i]){
            return -1;
        }
    }
    if (n1>n2){
        return 1;
    }else if(n1<n2){
        return -1;
    }
    return 0;
}


void seleccion(int vector[], int n) {
    if (n==0){
        return;
    }
    int max = maximo(vector, n);
    swap(&vector[max], &vector[n-1]);
    seleccion(vector, n-1);
}