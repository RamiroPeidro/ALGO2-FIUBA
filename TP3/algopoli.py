#!/usr/bin/python3
import sys
import csv
from grafo import Grafo
from collections import deque, Counter
from math import inf
from random import shuffle
from statistics import mode

from grafo_lib import armar_camino, camino_minimo_dijkstra, camino_minimo_bfs, dfs, bfs, pagerank

# /*********************************************************************
#  *               		     ALGOPOLI                                  *
#  *		 Facultad de Ingenieria de la Universidad de Buenos Aires  *
#  *											    Peidro, Ramiro     *
#  *												Vazquez, Rodrigo   *
#  *********************************************************************/
CAMINO_MINIMO = "min_seguimientos"
CFC = "cfc"
CICLO = "divulgar_ciclo"
RUMOR = "divulgar"
COMUNIDADES = "comunidades"
MAS_IMP = "mas_imp"
PERSECUCION = "persecucion"


def crear_grafo(ruta):
    grafo = Grafo(dirigido=True)
    with open(ruta, 'r') as f:
        csv_reader = csv.reader(f)
        for linea in csv_reader:
            linea = linea[0].split('\t')
            vertice1, vertice2 = linea
            grafo.arista_agregar(vertice1, vertice2)
    return grafo


def imprimir_camino(camino):
    for i in range(len(camino)):
        if(i == len(camino)-1):
            print(camino[i])
            break
        print(camino[i], end=" -> ")


def opcion_camino_minimo(linea, grafo):
    try:
        comando, origen, destino = linea
    except:
        print("Error de invocacion")
        return

    camino = camino_minimo_bfs(grafo, origen, destino)
    imprimir_camino(camino) if camino != [] else print(
        "Seguimiento imposible")


def _cfc(v, grafo, visitados, orden, mb, pila, apilados, todas_cfc):
    global orden_contador

    visitados.add(v)
    orden[v] = orden_contador
    mb[v] = orden[v]
    orden_contador += 1
    pila.appendleft(v)
    apilados.add(v)
    # DFS
    for w in grafo.adyacentes(v):
        if w not in visitados:

            _cfc(w, grafo, visitados, orden, mb, pila, apilados, todas_cfc)

    if w in apilados:
        mb[v] = min(mb[v], mb[w])

    if orden[v] == mb[v] and len(pila) > 0:
        nueva_cfc = []
        while True:
            w = pila.popleft()
            apilados.remove(w)
            nueva_cfc.append(w)
            if w == v:
                break
        todas_cfc.append(nueva_cfc)


def opcion_cfc(grafo):
    '''Imprime cada conjunto de vértices entre los cuales todos están conectados con todos.'''
    visitados = set()
    global orden_contador
    orden_contador = 0
    orden = {}
    mb = {}
    pila = deque()
    apilados = set()
    v = "0"
    todas_cfc = []
    _cfc(v, grafo, visitados, orden, mb, pila, apilados, todas_cfc)
    for i in range(len(todas_cfc)):
        print(f"CFC {i+1}:", end='')
        for item in todas_cfc[i]:
            if item == todas_cfc[i][-1]:
                print(f" {item}")
            else:
                print(f" {item},", end='')


def _dfs_n_ciclo(grafo, v, origen, camino, n, visitados):

    visitados.add(v)
    if n == len(camino):
        if origen in grafo.adyacentes(v):
            return camino + [origen]
        return camino[:-1]

    for w in grafo.adyacentes(v):
        if w not in visitados:
            camino = _dfs_n_ciclo(grafo, w, origen, camino+[w], n, visitados)
        if len(camino) > 1 and camino[-1] == origen:
            return camino
    visitados.remove(v)
    return camino[:-1]


def dfs_n_ciclo(grafo, origen, n):
    visitados = set()
    camino = [origen]
    camino = _dfs_n_ciclo(grafo, origen, origen, camino, n, visitados)
    return camino


def opcion_n_ciclo(linea, grafo):
    '''Permite encontrar un camino simple que empiece y termine en el delincuente pasado
    por parámetro, de largo n. En caso de no encontrarse un ciclo de ese largo y dicho
    comienzo, imprimir No se encontro recorrido.'''
    try:
        comando, delincuente, n = linea
    except:
        print("Error de invocacion")
        return
    visitados = set()
    n = int(n)
    camino = dfs_n_ciclo(grafo, delincuente, n)
    if len(camino) > 1:
        imprimir_camino(camino)
    else:
        print("No se encontro recorrido")


def opcion_rumor(linea, grafo):
    '''Imprime una lista con todos los delincuentes a los cuales les termina
    llegando un rumor que comienza en el delincuente pasado por parámetro,
    y a lo sumo realiza n saltos (luego, se empieza a tergiversar el mensaje),
    teniendo en cuenta que todos los delincuentes transmitirán el rumor a sus allegados.'''
    try:
        comando, delincuente, n = linea
        n = int(n)
    except:
        print("Error de invocacion")
        return
    padres, orden = bfs(grafo, delincuente)
    vertices_afectados = []
    for v in orden.keys():
        if orden[v] <= n and orden[v] > 0:
            vertices_afectados.append(v)
    for item in vertices_afectados:
        if item == vertices_afectados[-1]:
            print(f"{item}")
        else:
            print(f"{item}, ", end='')


def imprimir_importantes(grafo, k):
    resultado = []
    k = int(k)
    for i in range(k):
        resultado.append(grafo[i][0])
    [print(resultado[i], end=', ') for i in range(len(resultado)) if i < k-1]
    print(resultado[k-1])


def obtener_importantes_de_grafo(grafo, k):
    return [grafo[i][0] for i in range(int(k))]


def opcion_mas_importantes(linea, grafo):
    try:
        comando, k = linea
    except:
        print("Error de invocacion")
        return

    grafo_importantes = pagerank(grafo)
    imprimir_importantes(grafo_importantes, k)
    return grafo_importantes


def opcion_persecucion(linea, grafo, mas_importantes):
    comando, delincuentes, k = linea
    delincuentes = linea[1].split(',')
    grafo_importantes = mas_importantes if mas_importantes != None else pagerank(
        grafo)
    lista_importantes = obtener_importantes_de_grafo(grafo_importantes, k)

    minimo_camino = {}
    for delincuente in delincuentes:
        # minimo_camino.setdefault(delincuente, []).append([camino_minimo_dijkstra(grafo, delincuente, id) for id in lista_importantes])
        minimo_camino[delincuente] = [camino_minimo_dijkstra(
            grafo, delincuente, id) for id in lista_importantes]

    minimo = inf
    camino_final = None
    for key in minimo_camino.keys():
        for camino in minimo_camino[key]:

            if len(camino) < minimo:
                camino_final = camino
                minimo = len(camino_final)

            elif len(camino) == minimo:
                if lista_importantes.index(camino[-1]) < lista_importantes.index(camino_final[-1]):
                    camino_final = camino

    imprimir_camino(camino_final)
    return grafo_importantes


def imprimir_comunidades(label, n):

    valor_labels = set(label.values())
    #print("valor de labels", valor_labels)
    cant_comunidades = len(valor_labels)
    miembros = []
    for i in valor_labels:
        miembros.append([k for k, v in label.items() if v == i])

    j = 1
    for comunidad in miembros:
        if len(comunidad) >= n:
            print(f"Comunidad {j}: ", end='')
            j += 1
            for item in comunidad:
                if item == comunidad[-1]:
                    print(f"{item}")
                else:
                    print(f"{item}, ", end='')

    return


def opcion_comunidades(linea, grafo):
    '''Imprime un listado de comunidades de al menos n integrantes.'''
    try:
        comando, n = linea
    except:
        print("Error de invocacion")
        return
    n = int(n)
    if n < 0:
        print("Error de invocacion")
        return

    # Algoritmo Label Propagation -> https://algoritmos-rw.github.io/algo2/material/apuntes/label_propagation/
    # 1)    Para cada vértice Vi : Label[Vi]=i
    vertices = grafo.vertices()
    label = {}
    i = 0
    for v in vertices:
        label[v] = i
        i += 1
    # 2)    Determinar un orden aleatorio para los vértices.
    shuffle(vertices)
    # 3) Por cada vertice definir Label[Vi]= max_freq(Label[Vi.adyacentes()])
    for v in vertices:
        label[v] = mode([int(x) for x in grafo.entrantes(v)])

    convergencia = False
    # 4) Criterio de paro
    paro = 0.8  # de los vecinos de cada vértice tiene la misma Label que dicho vértice
    while not convergencia:
        shuffle(vertices)
        for v in vertices:
            labels_de_entrantes = [label[x]for x in grafo.entrantes(v)]
            label[v] = mode(labels_de_entrantes)
            if labels_de_entrantes.count(mode(labels_de_entrantes)) > paro*len(labels_de_entrantes):
                convergencia = True

    imprimir_comunidades(label, n)


def main():
    if len(sys.argv) < 2:
        print("Error en cantidad de parametros")
        return
    ruta = sys.argv[1]
    grafo = crear_grafo(ruta)
    mas_importantes = None
    booleano_importantes = False

    for line in sys.stdin:
        linea_spliteada = line.rstrip('\n').split()
        comando = linea_spliteada[0]
        if comando == CAMINO_MINIMO:
            opcion_camino_minimo(linea_spliteada, grafo)
        if comando == CFC:
            opcion_cfc(grafo)
        if comando == CICLO:
            opcion_n_ciclo(linea_spliteada, grafo)
        if comando == RUMOR:
            opcion_rumor(linea_spliteada, grafo)
        if comando == COMUNIDADES:
            opcion_comunidades(linea_spliteada, grafo)
        if comando == MAS_IMP:
            if not booleano_importantes:
                mas_importantes = opcion_mas_importantes(
                    linea_spliteada, grafo)
                booleano_importantes = True
            else:
                comando, k = linea_spliteada
                imprimir_importantes(mas_importantes, k)

        if comando == PERSECUCION:
            if not booleano_importantes:
                mas_importantes = opcion_persecucion(
                    linea_spliteada, grafo, mas_importantes)
                booleano_importantes = True
            else:
                opcion_persecucion(linea_spliteada, grafo, mas_importantes)


if __name__ == "__main__":
    main()
