// Corre Floyd-Warshall sobre el dataset real Chebyshev2 (networkrepository.com)
// y escribe la solucion en un CSV con una fila por cada par ordenado de
// nodos: nodo1,nodo2,distancia. Los nodos se numeran igual que en el
// archivo de entrada (1-indexados). Los pares sin camino se marcan con la
// palabra "inf".
//
// Chebyshev2 es una matriz de algebra lineal (no una red de distancias): sus
// valores numericos no representan costos de camino y su diagonal tiene una
// entrada negativa, lo que produce un ciclo de peso negativo que alcanza a
// casi todo el grafo (las distancias divergen a -infinito tras las n
// pasadas de relajacion). Por eso se ignoran los pesos originales y los
// lazos de la diagonal, y se trata el grafo como no ponderado: cada arista
// i->j (i != j) presente en el archivo se agrega con peso 1.
//
// Compilacion: g++ -std=c++17 -O2 -Isrc -o correrChebyshev2 real_datasets/correrChebyshev2.cpp src/Graph.cpp src/WarshallLib.cpp
// Ejecucion  : mkdir -p results && ./correrChebyshev2

#include <algorithm>
#include <cctype>
#include <chrono>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#include "Graph.h"
#include "WarshallLib.h"

using namespace std;
using Clock = chrono::high_resolution_clock;

// Lee el archivo .mtx e ignora los pesos originales: retorna un Graph donde
// cada arista i->j (i != j) tiene peso 1. Los lazos (diagonal) se descartan.
static Graph cargarComoNoPonderado(const string& path) {
    ifstream in(path);
    if (!in) return Graph();

    string line;
    while (getline(in, line)) {
        if (line.empty() || line[0] == '%') continue;
        break; // primera linea de datos: dimensiones
    }

    int rows = 0, cols = 0;
    {
        istringstream iss(line);
        iss >> rows >> cols;
    }
    int n = max(rows, cols);

    Graph g(n, "Chebyshev2");
    while (getline(in, line)) {
        if (line.empty() || line[0] == '%') continue;
        istringstream iss(line);
        int i, j;
        if (!(iss >> i >> j)) continue;
        if (i == j) continue; // se descartan los lazos
        g.addEdge(i - 1, j - 1, 1.0);
    }
    g.copyToSolution();
    return g;
}

int main() {
    const string DATASET = "Chebyshev2/Chebyshev2.mtx";
    const string SALIDA = "results/chebyshev2_distancias.csv";

    Graph g = cargarComoNoPonderado(DATASET);
    if (g.n == 0) {
        cerr << "No se pudo cargar " << DATASET << "\n";
        return 1;
    }

    auto t0 = Clock::now();
    floydWarshall(g);
    auto t1 = Clock::now();
    double segundos = chrono::duration<double>(t1 - t0).count();

    ofstream out(SALIDA);
    if (!out) {
        cerr << "No se pudo crear " << SALIDA << "\n";
        return 1;
    }
    out << "nodo1,nodo2,distancia\n";

    const auto& dist = g.getSolution();
    for (int i = 0; i < g.n; ++i) {
        for (int j = 0; j < g.n; ++j) {
            out << (i + 1) << "," << (j + 1) << ",";
            if (dist[i][j] == Graph::NO_EDGE) out << "inf";
            else out << dist[i][j];
            out << "\n";
        }
    }

    cout << "n = " << g.n << "\n";
    cout << "Tiempo de ejecucion de Floyd-Warshall: " << segundos << " s\n";
    cout << "Solucion escrita en " << SALIDA << "\n";

    return 0;
}
