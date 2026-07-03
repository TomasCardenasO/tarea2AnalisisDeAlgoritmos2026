#ifndef INSTANCE_IO_H
#define INSTANCE_IO_H

#include <string>
#include <vector>
#include "Edge.h"

// Instancia representada como lista de aristas (0-indexada), preservando el
// orden en que aparecen en el archivo .mtx. Graph::setData (Graph.cpp) solo
// expone una matriz de adyacencia, que no preserva ese orden; esta funcion
// existe especificamente para que el Algoritmo Base (que procesa una
// vector<Edge> secuencial) pueda recibir las aristas en el mismo orden que
// fueron escritas por los generadores (relevante sobre todo para el
// Experimento 4, donde el orden de procesamiento es la variable de interes).
struct EdgeListInstance {
    int n = 0;
    std::vector<Edge> edges;
};

// Lee un archivo Matrix Market (.mtx): comentarios '%', linea de dimensiones
// "rows cols nnz" y luego aristas "i j peso" (1-indexadas). Si el header
// indica "symmetric", se agrega tambien la arista inversa (misma convencion
// que Graph::readMatrixMarket).
EdgeListInstance leerEdgeList(const std::string& path);

#endif // INSTANCE_IO_H
