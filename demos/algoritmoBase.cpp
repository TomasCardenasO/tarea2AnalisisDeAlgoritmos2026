#include <iostream>
#include <vector>
#include <iomanip>

using namespace std;

const int INF = 1e9; // sin camino conocido

struct Edge {
    int src;
    int dest;
    int weight;
};

// Algoritmo base (APSP): Bellman-Ford ejecutado una vez por cada origen.
// Retorna matriz vacia si detecta un ciclo de peso negativo.
vector<vector<int>> algoritmoBaseAPSP(int n, const vector<Edge>& edges) {
    vector<vector<int>> distMatrix(n, vector<int>(n, INF));

    for (int i = 0; i < n; ++i) {
        int source = i;
        distMatrix[source][source] = 0;

        // Relajar las aristas hasta n-1 veces (largo maximo de un camino simple).
        // Si una pasada completa no actualiza ninguna distancia, el algoritmo ya
        // convergio y no hace falta seguir iterando (deteccion anticipada).
        for (int j = 1; j <= n - 1; ++j) {
            bool huboActualizacion = false;
            for (const auto& edge : edges) {
                if (distMatrix[source][edge.src] != INF &&
                    distMatrix[source][edge.src] + edge.weight < distMatrix[source][edge.dest]) {
                    distMatrix[source][edge.dest] = distMatrix[source][edge.src] + edge.weight;
                    huboActualizacion = true;
                }
            }
            if (!huboActualizacion) break;
        }

        // Si aun se puede relajar, hay un ciclo de peso negativo.
        for (const auto& edge : edges) {
            if (distMatrix[source][edge.src] != INF &&
                distMatrix[source][edge.src] + edge.weight < distMatrix[source][edge.dest]) {
                cout << "  [Alerta] Se ha detectado un ciclo de peso negativo." << endl;
                return {};
            }
        }
    }

    return distMatrix;
}

void printMatrix(const vector<vector<int>>& matrix, int n) {
    if (matrix.empty()) {
        cout << "  Matriz no disponible debido a un ciclo negativo." << endl;
        return;
    }

    for (int i = 0; i < n; ++i) {
        cout << "  Desde nodo " << i << ": ";
        for (int j = 0; j < n; ++j) {
            if (matrix[i][j] == INF) {
                cout << setw(5) << "INF";
            } else {
                cout << setw(5) << matrix[i][j];
            }
        }
        cout << endl;
    }
}

int main() {
    cout << "### PRUEBA 1: Grafo sin ciclos negativos ###" << endl;
    int n1 = 4; // 4 nodos (0, 1, 2, 3)
    vector<Edge> edges1 = {
        {0, 1, 4},
        {0, 2, 5},
        {1, 2, -2}, // Arista con peso negativo válida
        {1, 3, 6},
        {2, 3, 3}
    };
    /*
      Resultados esperados para la Prueba 1:
      Desde 0: 0 a 0 = 0, 0 a 1 = 4, 0 a 2 = 2 (0->1->2), 0 a 3 = 5 (0->1->2->3)
      Desde 1: 1 a 0 = INF, 1 a 1 = 0, 1 a 2 = -2, 1 a 3 = 1
      Desde 2: 2 a 0 = INF, 2 a 1 = INF, 2 a 2 = 0, 2 a 3 = 3
      Desde 3: Todo INF excepto a sí mismo.
    */
    vector<vector<int>> result1 = algoritmoBaseAPSP(n1, edges1);
    printMatrix(result1, n1);
    cout << endl;

    cout << "### PRUEBA 2: Grafo con ciclo negativo ###" << endl;
    int n2 = 3; // 3 nodos (0, 1, 2)
    vector<Edge> edges2 = {
        {0, 1, 1},
        {1, 2, -1},
        {2, 0, -1} // Ciclo: 0 -> 1 -> 2 -> 0 con peso total 1 - 1 - 1 = -1
    };
    /*
      Resultados esperados para la Prueba 2:
      El algoritmo debe detenerse y reportar la alerta del ciclo negativo.
    */
    vector<vector<int>> result2 = algoritmoBaseAPSP(n2, edges2);
    printMatrix(result2, n2);
    cout << endl;

    return 0;
}
