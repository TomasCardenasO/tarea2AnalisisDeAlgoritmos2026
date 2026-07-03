#include "WarshallLib.h"
#include <iostream>
#include <vector>

using namespace std;

int floydWarshall(Graph& graph) {
    int start;
    int dest;
    int intermediate;
    vector<vector<double>> solution = graph.getSolutionCopy();

    // Diagonal a 0 (distancia de un vertice a si mismo).
    for (int i = 0; i < graph.n; i++)
    {
        if (solution[i][i] > 0)
        {
            solution[i][i] = 0;
        }
    }
    for (intermediate = 0; intermediate < graph.n; intermediate++) {
        vector<double>& fila_inter = solution[intermediate];
        for (start = 0; start < graph.n; start++) {
            vector<double>& row_start = solution[start];
            double weight = row_start[intermediate];
            if (weight == Graph::NO_EDGE) continue;
            for (dest = 0; dest < graph.n; dest++) {
                double nw_weight = weight + fila_inter[dest];
                if (nw_weight < row_start[dest])
                    row_start[dest] = nw_weight;
            }
        }
    }
    // Diagonal negativa tras el algoritmo => ciclo de peso negativo.
    for (int i = 0; i < graph.n; i++) {
        if (solution[i][i] < 0) cout << "Ciclo negativo en el nodo " << i << "\n";
    }

    graph.setSolution(solution);
    return 0;
}
