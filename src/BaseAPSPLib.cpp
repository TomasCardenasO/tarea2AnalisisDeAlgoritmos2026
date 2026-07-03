#include "BaseAPSPLib.h"
#include <iostream>

using namespace std;

vector<vector<int>> algoritmoBaseAPSP(int n, const vector<Edge>& edges) {
    vector<vector<int>> distMatrix(n, vector<int>(n, INF_BASE));

    for (int i = 0; i < n; ++i) {
        int source = i;
        distMatrix[source][source] = 0;

        // Relajar las aristas hasta n-1 veces (largo maximo de un camino simple).
        // Si una pasada completa no actualiza ninguna distancia, el algoritmo ya
        // convergio y no hace falta seguir iterando (deteccion anticipada).
        for (int j = 1; j <= n - 1; ++j) {
            bool huboActualizacion = false;
            for (const auto& edge : edges) {
                if (distMatrix[source][edge.src] != INF_BASE &&
                    distMatrix[source][edge.src] + edge.weight < distMatrix[source][edge.dest]) {
                    distMatrix[source][edge.dest] = distMatrix[source][edge.src] + edge.weight;
                    huboActualizacion = true;
                }
            }
            if (!huboActualizacion) break;
        }

        // Si aun se puede relajar, hay un ciclo de peso negativo.
        for (const auto& edge : edges) {
            if (distMatrix[source][edge.src] != INF_BASE &&
                distMatrix[source][edge.src] + edge.weight < distMatrix[source][edge.dest]) {
                cout << "  [Alerta] Se ha detectado un ciclo de peso negativo." << endl;
                return {};
            }
        }
    }

    return distMatrix;
}
