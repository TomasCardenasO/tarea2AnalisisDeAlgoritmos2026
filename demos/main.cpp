#include "Graph.h"
#include <iostream>
#include <vector>

using namespace std;

// Floyd-Warshall: caminos minimos entre todos los pares de vertices.
int floydWarshall(Graph& graph){
    int start;
    int dest;
    int intermediate;
    vector<vector<double>> solution = graph.getSolutionCopy();

    // Diagonal a 0 (distancia de un vertice a si mismo).
    for (int i = 0; i < graph.n; i++)
    {
        if(solution[i][i] > 0)
        {
            solution[i][i] = 0;
        }
    }
    for ( intermediate = 0; intermediate < graph.n; intermediate++){
        vector<double>& fila_inter = solution[intermediate];
        for ( start = 0; start < graph.n; start++){
            vector<double>& row_start = solution[start];
            double weight = row_start[intermediate];
            if (weight == Graph::NO_EDGE) continue;
            for( dest = 0; dest < graph.n; dest++){
                double nw_weight = weight + fila_inter[dest];
                if (nw_weight < row_start[dest])
                    row_start[dest] = nw_weight;
            }
        }
    }
    // Diagonal negativa tras el algoritmo => ciclo de peso negativo.
    for (int i = 0; i < graph.n; i++){
        if (solution[i][i] < 0) cout << "Ciclo negativo en el nodo " << i << "\n";
    }

    graph.setSolution(solution);
    return 0;
}

int main(int argc, char** argv) {

    const double I = Graph::NO_EDGE;

    vector<vector<double>> m = {
        //   0    1    2    3
        {    0,   3,   I,   7 },   // 0
        {    8,   0,   2,   I },   // 1
        {    5,   I,   0,   1 },   // 2
        {    2,   I,   I,   0 }    // 3
    };
    vector<vector<double>> m1 = {
        //   0    1    2    3
        {    0,   9,   I,   10 },   // 0
        {    I,   0,   7,   I },   // 1
        {    I,   I,   0,   11 },   // 2
        {    I,   I,   I,   0 }    // 3
    };

    // Graph demo(4, "grafo_prueba");
    // demo.setAdjacency(m);          // almacena adj y copia a la matriz de soluciones

    // cout << "############ prueba ############\n";
    // demo.printGraph(-1);           // imprime matriz de adyacencia completa
    // cout << "\n";
    // floydWarshall(demo);             // ejecuta el algoritmo de Floyd-Warshall
    // demo.printDist(-1);            // imprime matriz de soluciones (copia de adj)
    // cout << "\n";


    Graph demo1(4, "grafo_prueba_1");
    demo1.setAdjacency(m1);          // almacena adj y copia a la matriz
    // cout << "############ prueba 1 ############\n";
    // demo1.printGraph(-1);           // imprime matriz de adyacencia completa
    // cout << "\n";
    floydWarshall(demo1);             // ejecuta el algoritmo de Floyd-Warshall
    demo1.printDist(-1);            // imprime matriz de soluciones (copia de adj)
    cout << "\n";

    return 0;
}
