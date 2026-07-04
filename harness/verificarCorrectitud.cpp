// Verifica que el Algoritmo Base y Floyd-Warshall entreguen la misma
// solucion sobre una instancia real (generada por los programas en
// generators/), comparando las dos matrices de distancia par a par. Sirve
// como marco comparativo de correctitud entre ambas implementaciones,
// ademas de los casos de prueba pequenos de demos/.
//
// Uso: ./verificarCorrectitud <archivo.mtx>
//
// Compilacion: g++ -std=c++17 -O2 -Isrc -o verificarCorrectitud harness/verificarCorrectitud.cpp src/Graph.cpp src/WarshallLib.cpp src/BaseAPSPLib.cpp src/InstanceIO.cpp

#include <cmath>
#include <iostream>
#include <string>

#include "Graph.h"
#include "WarshallLib.h"
#include "BaseAPSPLib.h"
#include "InstanceIO.h"

using namespace std;

int main(int argc, char** argv) {
    if (argc != 2) {
        cerr << "Uso: " << argv[0] << " <archivo.mtx>\n";
        return 1;
    }
    string archivo = argv[1];

    auto grafos = Graph::setData(archivo);
    if (grafos.empty()) {
        cerr << "No se pudo cargar " << archivo << "\n";
        return 1;
    }
    Graph& g = grafos[0];
    floydWarshall(g);
    const auto& distFW = g.getSolution();

    EdgeListInstance eli = leerEdgeList(archivo);
    auto distBase = algoritmoBaseAPSP(eli.n, eli.edges);
    if (distBase.empty()) {
        cerr << "El Algoritmo Base reporto un ciclo negativo; no se puede comparar.\n";
        return 1;
    }

    int n = g.n;
    const double EPS = 1e-6;
    long long discrepancias = 0;

    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            double a = distFW[i][j];
            double b = (distBase[i][j] == INF_BASE) ? Graph::NO_EDGE
                                                      : static_cast<double>(distBase[i][j]);
            bool ambasInfinitas = (a == Graph::NO_EDGE && b == Graph::NO_EDGE);
            if (!ambasInfinitas && fabs(a - b) > EPS) {
                if (discrepancias < 10) {
                    cerr << "Discrepancia en (" << i << "," << j << "): "
                         << "FloydWarshall=" << a << " AlgoritmoBase=" << b << "\n";
                }
                ++discrepancias;
            }
        }
    }

    if (discrepancias == 0) {
        cout << "OK: " << archivo << " (n=" << n << "): ambas implementaciones "
             << "coinciden en las " << (static_cast<long long>(n) * n) << " distancias.\n";
        return 0;
    }

    cout << "FALLO: " << discrepancias << " distancias distintas entre "
         << "Floyd-Warshall y el Algoritmo Base en " << archivo << ".\n";
    return 1;
}
