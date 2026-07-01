#include "Graph.h"
#include <iostream>
#include <vector>


int floydWarshall(Graph& graph){
    int start;
    int dest;
    int intermediate;
    std::vector<std::vector<double>> solution = graph.getSolutionCopy();

    // Inicializamos la diagonal de la matriz de soluciones a 0
    for (int i = 0; i < graph.n; i++)
    {
       if(solution[i][i] > 0)
       {
           solution[i][i] = 0;
       }
    }
    // Seleccionamos vertice intermedio
    for ( intermediate = 0; intermediate < graph.n; intermediate++){
        std::vector<double>& fila_inter = solution[intermediate];
        // Seleccionamos vertice de inicio
        for ( start = 0; start < graph.n; start++){
            std::vector<double>& row_start = solution[start];
            double weight = row_start[intermediate];
            if (weight == Graph::NO_EDGE) continue;
            // Seleccionamos vertice de destino
            for( dest = 0; dest < graph.n; dest++){
                double nw_weight = weight + fila_inter[dest];
                if (nw_weight < row_start[dest])
                    row_start[dest] = nw_weight;
            }
        }
    }
     for (int i = 0; i < graph.n; i++){
        if (solution[i][i] < 0) std::cout << "Ciclo negativo en el nodo " << i << "\n";
     }

    graph.setSolution(solution);
    return 0;
}

int main(int argc, char** argv) {

    const double I = Graph::NO_EDGE;

    std::vector<std::vector<double>> m = {
        //   0    1    2    3
        {    0,   3,   I,   7 },   // 0
        {    8,   0,   2,   I },   // 1
        {    5,   I,   0,   1 },   // 2
        {    2,   I,   I,   0 }    // 3
    };
    std::vector<std::vector<double>> m1 = {
        //   0    1    2    3
        {    0,   9,   I,   10 },   // 0
        {    I,   0,   7,   I },   // 1
        {    I,   I,   0,   11 },   // 2
        {    I,   I,   I,   0 }    // 3
    };

    // Graph demo(4, "grafo_prueba");
    // demo.setAdjacency(m);          // almacena adj y copia a la matriz de soluciones

    // std::cout << "############ prueba ############\n";
    // demo.printGraph(-1);           // imprime matriz de adyacencia completa
    // std::cout << "\n";
    // floydWarshall(demo);             // ejecuta el algoritmo de Floyd-Warshall
    // demo.printDist(-1);            // imprime matriz de soluciones (copia de adj)
    // std::cout << "\n";


    Graph demo1(4, "grafo_prueba_1");
    demo1.setAdjacency(m1);          // almacena adj y copia a la matriz
    // std::cout << "############ prueba 1 ############\n";
    // demo1.printGraph(-1);           // imprime matriz de adyacencia completa
    // std::cout << "\n";
    floydWarshall(demo1);             // ejecuta el algoritmo de Floyd-Warshall
    demo1.printDist(-1);            // imprime matriz de soluciones (copia de adj)
    std::cout << "\n";







    return 0;
}
