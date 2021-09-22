from grafo import Grafo
from heapq import heappush as heap_encolar, heappop as heap_desencolar
from collections import deque
import math
from math import inf
'''
*******************************************************************
*               BIBLIOTECA DE ALGORITMOS SOBRE GRAFO              *
******************************************************************
'''
D = 0.8
ITERACIONES = 10


def armar_camino(padre, origen, destino):
    ''' Reconstruye el camino desde el vertice origen hasta destino a partir del diccionar de padres. Devuelve una lista'''
    camino = deque()
    try:
        while padre[destino]:
            camino.appendleft(destino)  # inserta al principio
            destino = padre[destino]
        camino.appendleft(origen)
        return list(camino)
    except:
        return []


def camino_minimo_dijkstra(grafo, origen, destino):
    ''' Devuelve una lista con el camino minimo entre orgien y destino. Calculado por Dijkstra'''
    distancia = {}
    padre = {}
    for v in grafo:
        distancia[v] = inf
    distancia[origen] = 0
    padre[origen] = None
    heap = []
    heap_encolar(heap, (distancia[origen], origen))

    while heap:  # while heap no esta vacio
        _, v = heap_desencolar(heap)
        if v == destino:
            return armar_camino(padre, origen, destino)
        for w in grafo.adyacentes(v):
            if distancia[v]+grafo.peso(v, w) < distancia[w]:
                padre[w] = v
                distancia[w] = distancia[v]+grafo.peso(v, w)
                heap_encolar(heap, (distancia[w], w))
    return None  # si termina el while, no se encontro conexion entre origen y destino


def camino_minimo_bfs(grafo, origen, destino):
    ''' Devuelve una lista con el camino minimo entre orgien y destino. Calculado por BFS'''
    padres, orden = bfs(grafo, origen)
    return armar_camino(padres, origen, destino)


def bfs(grafo, origen):
    '''Recorrido bfs, si origen es None, arranca de un aleatorio'''
    if not origen:
        origen = grafo.vertice_aleatorio()
    padres = {}
    orden = {}
    visitados = set()
    A = origen
    cola = deque()
    padres[A] = None
    orden[A] = 0
    visitados.add(origen)
    cola.append(origen)
    while cola:
        v = cola.popleft()
        for w in grafo.adyacentes(v):
            if w not in visitados:
                visitados.add(w)
                padres[w] = v
                orden[w] = orden[v]+1
                cola.append(w)
    return padres, orden


def dfs(grafo, origen):
    '''Recorrido en pronfundidad a partir del vertice origen. Devuelve diccionario de padres y  diccionario de ordenes de cada vertice visitado'''
    padres = {}
    orden = {}
    visitados = set()
    padres[origen] = None
    orden[origen] = 0
    visitados.add(origen)
    _dfs(grafo, origen, visitados, padres, orden)
    return padres, orden


def _dfs(grafo, v, visitados, padres, orden):
    visitados.add(v)
    for w in grafo.adyacentes(v):
        if w not in visitados:
            padres[w] = v
            orden[w] = orden[v]+1
            _dfs(grafo, w, visitados, padres, orden)


def MST(grafo, origen=None):
    ''' Obtiene el arbol de tendido minimo del grafo. Tiene opcion de armarlo'''
    if not origen:
        origen = grafo.vertice_aleatorio()

    visitados = set()
    visitados.add(origen)
    heap = []
    for v in grafo.adyacentes(origen):
        heap_encolar(heap, (grafo.peso(origen, v), origen, v))
    arbol = grafo(dirigido=True, lista_vertices=grafo.vertices())

    while heap:
        (v, w), peso = heap_desencolar(heap)
        if w not in visitados:
            arbol.arista_agregar(v, w, peso)
            visitados.add(w)
            for x in grafo.adyacentes(w):
                if x not in visitados:
                    heap_encolar(heap, (grafo.peso(w, x), w, x))
    return arbol


def pagerank(grafo):
    pr = {}
    entrantes = {}
    for v in grafo:
        pr[v] = 1/len(grafo)
    i = 0

    while i < 10:
        for v in grafo:
            pr[v] = (1-D)/len(grafo) + D*sum([pr[i]/len(grafo.adyacentes(i))
                                              for i in grafo.entrantes(v)])
        i += 1

    pr_ordenado = sorted(pr.items(), key=lambda x: x[1], reverse=True)
    return pr_ordenado
