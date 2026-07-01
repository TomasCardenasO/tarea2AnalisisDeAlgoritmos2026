#ifndef GRAPH_H
#define GRAPH_H

#include <string>
#include <vector>
#include <limits>

// ============================================================================
//  Estructura de un grafo simple representado con matriz de adyacencia.
//
//  - adj  : matriz de adyacencia (la que tu entregas o la que se lee del data)
//  - dist : "matriz de soluciones", una COPIA de la matriz de adyacencia.
//           Sobre esta copia aplicaras Floyd-Warshall. printDist() la imprime.
//
//  Convencion de pesos:
//  - Si existe arista (i,j) se guarda su peso.
//  - Si NO existe arista se guarda NO_EDGE (infinito).
// ============================================================================
class Graph {
public:
    static const double NO_EDGE;   // centinela "no hay arista" (infinito)

    int n;              // numero de vertices
    std::string name;   // nombre del grafo

    std::vector<std::vector<double>> adj;   // matriz de adyacencia
    std::vector<std::vector<double>> dist;  // matriz de soluciones (copia de adj)

    Graph();
    explicit Graph(int n, const std::string& name = "");


    void setAdjacency(const std::vector<std::vector<double>>& matrix);
    void addEdge(int i, int j, double w);
    void copyToSolution();   // dist = adj

    // -------- Acceso a la matriz de soluciones --------
    // Referencia: manipulala directamente afuera (los cambios quedan guardados).
    std::vector<std::vector<double>>& getSolution();
    const std::vector<std::vector<double>>& getSolution() const;

    // Copia: trabajala aparte y devuelvela luego con setSolution().
    std::vector<std::vector<double>> getSolutionCopy() const;

    // Recibe de vuelta la matriz manipulada afuera y la almacena
    void setSolution(const std::vector<std::vector<double>>& solution);

    // -------- Impresion --------
    // limit < 0 -> matriz COMPLETA; limit >= 0 -> esquina limit x limit (def 12).
    void printGraph(int limit = 12) const;   // matriz de adyacencia
    void printDist(int limit = 12) const;    // matriz de soluciones

    // -------- Lectura del dataset --------
    // archivo .mtx -> 1 grafo; carpeta -> 1 grafo por cada .mtx.
    static std::vector<Graph> setData(const std::string& path);

private:
    static Graph readMatrixMarket(const std::string& filePath);
};

#endif // GRAPH_H
