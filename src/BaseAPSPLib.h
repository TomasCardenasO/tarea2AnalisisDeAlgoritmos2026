#ifndef BASE_APSP_LIB_H
#define BASE_APSP_LIB_H

#include <vector>
#include "Edge.h"

// Sin camino conocido (misma convencion que demos/algoritmoBase.cpp).
const int INF_BASE = 1000000000;

// Algoritmo Base (APSP): Bellman-Ford ejecutado una vez por cada origen.
// Incluye deteccion anticipada: si una pasada de relajacion no actualiza
// ninguna distancia, se corta antes de completar las n-1 pasadas. Retorna
// matriz vacia si detecta un ciclo de peso negativo.
//
// Misma logica que demos/algoritmoBase.cpp, pero separada en una libreria
// (sin funcion main) para poder invocarla repetidamente desde el arnes de
// experimentos. Si se cambia el algoritmo, hay que actualizar ambas copias.
std::vector<std::vector<int>> algoritmoBaseAPSP(int n, const std::vector<Edge>& edges);

#endif // BASE_APSP_LIB_H
