#define _POSIX_C_SOURCE 200809L
#include "strutil.h"
#include "calc_helper.h"
#include <assert.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pila.h"
#include <math.h>


typedef struct calc_token calc_token_t;


void calculadora(char* linea){
    pilanum_t* pila = pilanum_crear();
    char** procesada = dc_split(linea);

    calc_token_t tok;    
    int i = 0;
    bool correcto = true;
    while(procesada[i]){
        bool error = calc_parse(procesada[i] ,&tok);
            
        if(!error){
            fprintf(stdout, "ERROR\n");
            correcto = false;
            break;
        }

        if (tok.type == TOK_NUM){
            apilar_num(pila, tok.value);
        }

        else if(tok.type == TOK_OPER){

            if(tok.oper.op != OP_RAIZ && tok.oper.op != OP_TERN){

                if(!pila_ver_tope(pila)){
                    fprintf(stdout, "ERROR\n");
                    correcto = false;
                    break;
                }
                calc_num dato1 = *(calc_num*)pila_ver_tope(pila);
                desapilar_num(pila, &dato1);
                if(!pila_ver_tope(pila)){
                    fprintf(stdout, "ERROR\n");
                    correcto = false;
                    break;
                }
                calc_num dato2 = *(calc_num*)pila_ver_tope(pila);
                desapilar_num(pila, &dato2);


                if(tok.oper.op == OP_ADD){
                    calc_num resultado = (calc_num)((double)dato1 + (double)dato2);
                    apilar_num(pila, resultado); 
                }

                if(tok.oper.op == OP_SUB){
                    calc_num resultado = (calc_num)((double)dato2 - (double)dato1);
                    apilar_num(pila, resultado); 
                }

                if(tok.oper.op == OP_MUL){
                    calc_num resultado = (calc_num)((double)dato2 * (double)dato1);
                    apilar_num(pila, resultado); 
                }

                if(tok.oper.op== OP_DIV){
                    if (dato1 == 0){
                        fprintf(stdout, "ERROR\n");
                        correcto = false;
                        i++;
                        break;
                    }
                    calc_num resultado = (calc_num)((double)dato2 / (double)dato1);
                    apilar_num(pila, resultado); 
                }

                if(tok.oper.op == OP_POW){
                    if(dato1 < 0){
                        fprintf(stdout, "ERROR\n");
                        correcto = false;
                        i++;
                        break;
                    }
                    double resultado = pow((double)dato2,(double)dato1);
                    calc_num res = (calc_num) resultado;
                    apilar_num(pila, res); 
                }

                if(tok.oper.op == OP_LOG){
                    if(dato1 < 2){
                        fprintf(stdout, "ERROR\n");
                        correcto = false;
                        i++;
                        break;
                    };
                    calc_num resultado = (calc_num)(log10((double)dato2)/log10((double)dato1));
                    apilar_num(pila, resultado); 
                }

            } else if(tok.oper.op == OP_RAIZ) {
                if(pila_esta_vacia(pila)){
                    fprintf(stdout, "ERROR\n");
                    correcto = false;
                    i++;
                    break;
                }
                calc_num dato = *(calc_num*)pila_ver_tope(pila);
                desapilar_num(pila, &dato);
                if(dato < 0){
                    fprintf(stdout, "ERROR\n");
                    correcto = false;
                    i++;
                    break;
                }
                double resultado = sqrt((double)dato);
                calc_num res = (calc_num) resultado;
                apilar_num(pila, res); 

            } else if(tok.oper.op == OP_TERN){
                if(pila_esta_vacia(pila)) {
                    fprintf(stdout, "ERROR\n");
                    correcto = false;
                    break;
                }

                calc_num dato1 = *(calc_num*)pila_ver_tope(pila);
                desapilar_num(pila, &dato1);
                if(pila_esta_vacia(pila)){
                    fprintf(stdout, "ERROR\n");
                    correcto = false;
                    i++;
                    break;
                }
                calc_num dato2 = *(calc_num*)pila_ver_tope(pila);
                desapilar_num(pila, &dato2);

                if(pila_esta_vacia(pila)){
                    fprintf(stdout, "ERROR\n");
                    correcto = false;
                    i++;
                    break;
                }
                calc_num dato3 = *(calc_num*)pila_ver_tope(pila);
                desapilar_num(pila, &dato3);

                calc_num resultado = (calc_num)((double)dato3 ? (double)dato2 : (double)dato1);
                apilar_num(pila, resultado);
            }
        }
        i++;
    }
    calc_num dato;
    if(correcto){ 
        int n = 0;
        bool imprimir = true;
        while(!pila_esta_vacia(pila)){
            if(n==1){
                fprintf(stdout, "ERROR\n");
                imprimir =false;
                break;
            }
            dato = *(calc_num*) pila_ver_tope(pila);
            desapilar_num(pila, &dato);
            n++;
        }
    if(imprimir) fprintf(stdout, "%li\n", dato);
    }
    
    pilanum_destruir(pila);
    free_strv(procesada);
}

int main(int argc, char const *argv[]) {
    char* linea = NULL;
    size_t capacidad = 0;
    size_t lineas = 0;
    while ((lineas = getline(&linea, &capacidad, stdin)) !=-1){
        calculadora(linea);
    }
    free(linea);
    return 0;
}