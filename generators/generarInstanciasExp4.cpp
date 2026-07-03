// Generador de instancias para el Experimento 4 del diseno experimental:
// sensibilidad a la topologia y al orden de procesamiento.
//
// Genera dos archivos que representan el mismo grafo (mismo n, mismo m,
// mismas aristas y mismos pesos): una cadena dirigida 1 -> 2 -> 3 -> ... ->
// n (n vertices, m = n-1 aristas). La diferencia entre ambos archivos es el
// orden en que las aristas quedan escritas:
//
//  * best_case.mtx : aristas en orden creciente (1,2),(2,3),...,(n-1,n).
//    Este es el orden ideal para Bellman-Ford: en una sola pasada sobre la
//    lista de aristas ya se propagan correctamente todas las distancias
//    desde cualquier origen, porque cada arista se relaja en el mismo orden
//    en que la informacion la necesita (orden topologico del grafo).
//
//  * worst_case.mtx : las mismas aristas, en orden decreciente
//    (n-1,n),(n-2,n-1),...,(1,2). Este es el orden clasico de peor caso
//    para Bellman-Ford: en cada pasada la distancia solo avanza un salto,
//    porque la arista que necesita el valor recien actualizado ya fue
//    procesada antes en esa misma pasada. Se requieren hasta n-1 pasadas
//    completas para converger.
//
// El Algoritmo Base incluye deteccion anticipada (corta las pasadas de
// relajacion en cuanto una pasada completa no actualiza ninguna distancia),
// asi que el tiempo en best_case.mtx deberia ser bastante menor que en
// worst_case.mtx (idealmente O(m) contra O(n*m)). Floyd-Warshall, en
// cambio, deberia tardar lo mismo en ambos casos, ya que es determinista y
// no depende del orden de las aristas (siempre recorre los tres bucles
// anidados sobre V).
//
// Para que este experimento tenga sentido, el lector que arma la lista de
// aristas que consume el Algoritmo Base debe preservar el orden en que
// aparecen en el archivo .mtx (leyendolas secuencialmente). La clase Graph
// solo expone una matriz de adyacencia, que no preserva ese orden; si se
// reconstruye la lista de aristas recorriendo la matriz fila por fila, se
// pierde el orden que se codifico aqui y el experimento no mostraria
// diferencias entre ambos casos.
//
// n = 1000 se eligio porque, siendo un grafo disperso (m = n-1), incluso el
// peor caso es barato (el Algoritmo Base hace del orden de O(n^2*m) = O(n^3)
// ~ 10^9 relajaciones en el peor caso, similar al costo de Floyd-Warshall en
// ese mismo n), y a la vez permite una separacion visible gracias a la
// deteccion anticipada (mejor caso ~O(n) contra peor caso ~O(n^2) por cada
// corrida de Bellman-Ford, sumando sobre las n corridas).
//
// Salida: instances/exp4_topologia/{best_case,worst_case}.mtx, mas un
// manifest.csv con columnas caso,n,m,archivo.
//
// El directorio de salida debe existir antes de ejecutar este programa
// (mkdir -p instances/exp4_topologia).
//
// Compilacion:  g++ -std=c++17 -O2 -o generarInstanciasExp4 generarInstanciasExp4.cpp
// Ejecucion  :  mkdir -p instances/exp4_topologia && ./generarInstanciasExp4

#include <cstdint>
#include <fstream>
#include <iostream>
#include <random>
#include <string>
#include <vector>

static const int N_FIJO = 1000;
static const int MAX_PESO = 1000;
static const std::string OUT_DIR = "instances/exp4_topologia";
static const unsigned SEED = 4100u;

int main() {
    int n = N_FIJO;
    int m = n - 1;

    std::mt19937_64 rng(SEED);
    std::uniform_int_distribution<int> pesoDist(1, MAX_PESO);

    // pesos[k] = peso de la arista entre los vertices (k+1) y (k+2), 1-indexado
    // (es decir, la k-esima arista de la cadena, k = 0..n-2).
    std::vector<int> pesos(m);
    for (auto& w : pesos) w = pesoDist(rng);

    std::string manifestPath = OUT_DIR + "/manifest.csv";
    std::ofstream manifest(manifestPath);
    if (!manifest) {
        std::cerr << "No se pudo crear " << manifestPath
                  << ". Crea antes el directorio con: mkdir -p " << OUT_DIR << "\n";
        return 1;
    }
    manifest << "caso,n,m,archivo\n";

    // --- Mejor caso: orden creciente (1,2),(2,3),...,(n-1,n) ---
    {
        std::string fname = "best_case.mtx";
        std::string outPath = OUT_DIR + "/" + fname;
        std::ofstream out(outPath);
        if (!out) {
            std::cerr << "No se pudo abrir para escritura: " << outPath << "\n";
            return 1;
        }
        out << "%%MatrixMarket matrix coordinate real general\n";
        out << "% Instancia Experimento 4 (mejor caso empirico): cadena 1->2->...->n, "
            << "n=" << n << ", m=" << m << ", orden ascendente, seed=" << SEED << "\n";
        out << n << " " << n << " " << m << "\n";
        for (int k = 0; k < m; ++k) {
            out << (k + 1) << " " << (k + 2) << " " << pesos[k] << "\n";
        }
        manifest << "mejor," << n << "," << m << "," << fname << "\n";
        std::cout << "Generada instancia mejor caso (n=" << n << ", m=" << m
                  << ") -> " << outPath << "\n";
    }

    // --- Peor caso: mismas aristas, orden descendente (n-1,n),...,(1,2) ---
    {
        std::string fname = "worst_case.mtx";
        std::string outPath = OUT_DIR + "/" + fname;
        std::ofstream out(outPath);
        if (!out) {
            std::cerr << "No se pudo abrir para escritura: " << outPath << "\n";
            return 1;
        }
        out << "%%MatrixMarket matrix coordinate real general\n";
        out << "% Instancia Experimento 4 (peor caso empirico): mismo grafo que "
            << "best_case.mtx (cadena 1->2->...->n), n=" << n << ", m=" << m
            << ", orden descendente, seed=" << SEED << "\n";
        out << n << " " << n << " " << m << "\n";
        for (int k = m - 1; k >= 0; --k) {
            out << (k + 1) << " " << (k + 2) << " " << pesos[k] << "\n";
        }
        manifest << "peor," << n << "," << m << "," << fname << "\n";
        std::cout << "Generada instancia peor caso (n=" << n << ", m=" << m
                  << ") -> " << outPath << "\n";
    }

    std::cout << "Manifest escrito en " << manifestPath << "\n";
    return 0;
}
