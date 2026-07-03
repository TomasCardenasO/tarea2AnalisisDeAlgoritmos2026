#ifndef GRAPH_H
#define GRAPH_H

#include <string>
#include <vector>
#include <limits>

using namespace std;

// Grafo representado con matriz de adyacencia.
// adj  : matriz original. (matriz de adyacencia)
// dist : copia de adj sobre la que se aplica Floyd-Warshall. (distancias/soluciones)
// Convencion: sin arista (i,j) -> NO_EDGE (infinito).
class Graph {
    public:
        static const double NO_EDGE;   // centinela "no hay arista"

        int n;              // numero de vertices
        string name;        // nombre del grafo

        vector<vector<double>> adj;
        vector<vector<double>> dist;

        Graph();
        explicit Graph(int n, const string& name = "");

        void setAdjacency(const vector<vector<double>>& matrix);
        void addEdge(int i, int j, double w);
        void copyToSolution();   // dist = adj

        // Acceso a la matriz de soluciones
        vector<vector<double>>& getSolution();
        const vector<vector<double>>& getSolution() const;
        vector<vector<double>> getSolutionCopy() const;
        void setSolution(const vector<vector<double>>& solution);

        // limit < 0 -> matriz completa; limit >= 0 -> esquina limit x limit
        void printGraph(int limit = 12) const;
        void printDist(int limit = 12) const;

        // archivo .mtx -> 1 grafo; carpeta -> 1 grafo por cada .mtx
        static vector<Graph> setData(const string& path);

    private:
        static Graph readMatrixMarket(const string& filePath);
};

#endif // GRAPH_H
