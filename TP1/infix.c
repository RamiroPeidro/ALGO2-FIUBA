#define _POSIX_C_SOURCE 200809L
#include "calc_helper.h"
#include "strutil.h"
#include <assert.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pila.h"
#include <math.h>
#include "cola.h"

typedef struct calc_token calc_token_t;

int precedencia(char* signo){
    if(strcmp(signo, "+") == 0 || strcmp(signo, "-") == 0) return 2;
    else if(strcmp(signo, "*") == 0 || strcmp(signo, "/") == 0) return 3;
    return 4;
}

int asociatividad(char* signo){
    if(strcmp(signo, "+") == 0 || strcmp(signo, "-") == 0 || strcmp(signo, "*") == 0 || strcmp(signo, "/")==0) return 1;
    return 0;
}


void procesar_linea(char* linea){
    pila_t* pila = pila_crear();
    cola_t* cola = cola_crear();
    char** procesada = infix_split(linea);
    int i = 0;
    bool fail = false;

    while(procesada[i]){

        calc_token_t tok; 

        bool error = calc_parse(procesada[i] ,&tok);

        if(!error){
            fprintf(stdout, "ERROR\n");
            fail = true;
            break;
        }

        if (tok.type == TOK_NUM){
            cola_encolar(cola, procesada[i]);
        }

        else if (tok.type == TOK_LPAREN){
            pila_apilar(pila, procesada[i]);
        }

        else if(tok.type == TOK_OPER){

            if(pila_esta_vacia(pila)){
                if(cola_esta_vacia(cola)){
                    fprintf(stdout, "ERROR");
                    break;
                }
                pila_apilar(pila, procesada[i]);
                i++;
                continue;
            }

            if(strcmp((char*)pila_ver_tope(pila), "(") == 0){
                pila_apilar(pila, procesada[i]);
                i++;
                continue;
            }

            while(!pila_esta_vacia(pila) && strcmp((char*)pila_ver_tope(pila), "(") != 0){

                char* tope = (char*)pila_ver_tope(pila);

                if(asociatividad(procesada[i]) == 1 && precedencia(procesada[i]) <= precedencia(tope)){
                    cola_encolar(cola, tope);
                    pila_desapilar(pila);

                }else if(asociatividad(procesada[i]) == 0 && precedencia(procesada[i]) < precedencia(tope)){
                    cola_encolar(cola, pila_desapilar(pila));


                } else {
                    break;
                } 
            }
            pila_apilar(pila, procesada[i]);
        }


        else if(tok.type == TOK_RPAREN){
            bool distinto = true;
            while(distinto){
                if(strcmp((char*)pila_ver_tope(pila), "(") != 0){
                    cola_encolar(cola, pila_desapilar(pila));
                } else{
                    pila_desapilar(pila);
                    distinto = false;
                }
            }
            if(distinto){
                fprintf(stdout, "ERROR\n");
                error = true;
                break;
            }
        }
        i++;
    }

    while(!pila_esta_vacia(pila)){
        if(strcmp((char*)pila_ver_tope(pila), "(") == 0 || strcmp((char*)pila_ver_tope(pila), ")") == 0){
            fprintf(stdout, "ERROR\n");
            fail = true;
            break;
        } else{
            cola_encolar(cola, pila_desapilar(pila));
        }
    }

    if(!fail){
        while(!cola_esta_vacia(cola)){
            fprintf(stdout, "%s ", (char*)cola_desencolar(cola));
        }
    }

    fprintf(stdout, "\n");
    cola_destruir(cola, NULL);
    pila_destruir(pila);
    free_strv(procesada);
}


int main(int argc, char const *argv[]) {
    char* linea = NULL;
    size_t capacidad = 0;
    size_t lineas = 0;
    while ((lineas = getline(&linea, &capacidad, stdin)) !=-1){
        procesar_linea(linea);
    }
    free(linea);
    return 0;
}