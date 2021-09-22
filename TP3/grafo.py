from random import choice


class Grafo(object):
    ''' Clase para grafo. Implementacionde diccionario de diccionarios. '''

    def __init__(self, dirigido=False):
        ''' Crea grafo y *dirigido* es el argumento booleano que determina si el grafo es dirigido. '''
        self.grafo = {}
        self.dirigido = dirigido
        self.dict_entrantes = {}

    def vertice_agregar(self, vertice):
        ''' Agrega un vertice al grafo.'''
        self.grafo[vertice] = {}

    def vertice_eliminar(self, vertice):
        ''' Elimina un vertice del grafo. '''
        for v in self.adyacentes(vertice):
            # borra las aristas que unian a ese grafo
            del self.grafo[v][vertice]
        del self.grafo[vertice]

    def vertices(self):
        ''' Devuelve una lista con todos los vertices del grafo. '''
        return list(self.grafo)

    def vertice_aleatorio(self):
        ''' Devuelve un vertice aleatorio del grafo '''
        return choice(self.vertices())

    def hay_arista(self, i, f):
        ''' Determina si dos vertices estan unidos por una arista. '''
        return f in self.grafo[i] if i in self.grafo else False

    def arista_agregar(self, i, f, peso=1):
        ''' Agrega una arista al grafo. Si la arista ya existe, se actualiza el peso.
        *i* y *f* son vertices dentro del grafo y la arista se crea desde i hastas f. Si alguno de los vertices no existe, los agrega.
        *peso* es el valor del peso de la arista, es 1 por defecto.
        '''
        if i not in self.grafo:
            self.vertice_agregar(i)
        if f not in self.grafo:
            self.vertice_agregar(f)

        self.grafo[i][f] = peso
        self.dict_entrantes.setdefault(f, []).append(i)

        if not self.dirigido:
            self.grafo[f][i] = peso
            self.dict_entrantes.setdefault(i, []).append(f)

    def arista_eliminar(self, i, f):
        ''' Elimina una arista del grafo. Si alguno de los vertices no existe, devuelve error.'''
        del self.grafo[i][f]
        if not self.dirigido:
            del self.grafo[f][i]

    def __contains__(self, vertice):
        ''' Determina si un vertice pertenece a un grafo. '''
        return vertice in self.grafo

    def peso(self, i, f):
        ''' Devuelve el peso de la union entre dos vertices'''
        return self.grafo[i][f]  # if grafo.hay_arista(i, f) else 0

    def adyacentes(self, vertice):
        ''' Devuelve una lista de todos los vertices adyacentes al provisto por argumento. '''
        return list(self.grafo[vertice])

    def entrantes(self, vertice):
        ''' Devuelve una lista de todos los vertices entrantes al provisto por argumento. '''
        return self.dict_entrantes[vertice]

    def __len__(self):
        ''' Devuelve la cantidad de vertices dentro del grafo. '''
        return len(self.grafo)

    def __iter__(self):
        ''' Iterador del grafo. '''
        return iter(self.grafo)
