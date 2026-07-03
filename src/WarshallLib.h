#ifndef WARSHALL_LIB_H
#define WARSHALL_LIB_H

#include "Graph.h"

// Floyd-Warshall: caminos minimos entre todos los pares de vertices.
// Misma logica que demos/main.cpp, pero separada en una libreria (sin
// funcion main) para poder invocarla repetidamente desde el arnes de
// experimentos.
int floydWarshall(Graph& graph);

#endif // WARSHALL_LIB_H
