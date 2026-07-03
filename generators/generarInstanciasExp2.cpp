// Generador de instancias para el Experimento 2 del diseno experimental:
// variacion del tamano de entrada en grafos dispersos.
//
// Para cada n de una lista fija se genera un digrafo donde cada vertice
// tiene exactamente OUT_DEGREE aristas salientes hacia vertices distintos
// elegidos al azar (sin lazos ni aristas repetidas), de modo que
// m = OUT_DEGREE * n = Theta(n), cumpliendo la condicion m = O(n) pedida en
// el enunciado. Los pesos son enteros positivos uniformes en [1, MAX_PESO],
// igual que en el Experimento 1, para evitar ciclos negativos y poder
// comparar contra la solucion de Floyd-Warshall.
//
// Salida: un archivo .mtx por cada n en instances/exp2_dispersos/, mas un
// manifest.csv con columnas n,m,archivo.
//
// El directorio de salida debe existir antes de ejecutar este programa
// (mkdir -p instances/exp2_dispersos).
//
// Compilacion:  g++ -std=c++17 -O2 -o generarInstanciasExp2 generarInstanciasExp2.cpp
// Ejecucion  :  mkdir -p instances/exp2_dispersos && ./generarInstanciasExp2

#include <algorithm>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <random>
#include <string>
#include <vector>

static const int MAX_PESO = 1000;          // pesos en [1, MAX_PESO]
static const int OUT_DEGREE = 4;           // grado de salida constante por vertice
static const std::string OUT_DIR = "instances/exp2_dispersos";

// Tamanos de entrada del experimento 2 (grafos dispersos, m = O(n)). Como
// aqui ambos algoritmos son teoricamente O(n^3) (el Algoritmo Base hace n
// llamadas a Bellman-Ford de costo O(n*m), y con m=k*n eso da O(k*n^3);
// Floyd-Warshall es Theta(n^3) siempre), se puede usar un rango de n mayor
// que en el Experimento 1, donde el grafo denso hacia que el Algoritmo Base
// fuera O(n^4). En el punto mas grande (n=1000) el Algoritmo Base hace del
// orden de 4*10^9 relajaciones en el peor caso, similar al costo de
// Floyd-Warshall en ese mismo n. Conviene calibrar con una corrida unica en
// n=1000 antes del barrido completo con las 32 o mas repeticiones que pide
// el enunciado.
static const std::vector<int> N_VALUES = {
    100, 200, 300, 400, 500, 600, 700, 800, 900, 1000
};

static void generarInstanciaDispersa(int n, unsigned seed, const std::string& outPath) {
    std::mt19937_64 rng(seed);
    std::uniform_int_distribution<int> pesoDist(1, MAX_PESO);

    long long m = static_cast<long long>(n) * OUT_DEGREE;

    std::ofstream out(outPath);
    if (!out) {
        std::cerr << "No se pudo abrir para escritura: " << outPath << "\n";
        return;
    }
    out << "%%MatrixMarket matrix coordinate real general\n";
    out << "% Instancia Experimento 2 (disperso, m = " << OUT_DEGREE << "*n): n=" << n
        << ", grado de salida constante=" << OUT_DEGREE
        << ", pesos uniformes en [1," << MAX_PESO << "], seed=" << seed << "\n";
    out << n << " " << n << " " << m << "\n";

    std::vector<int> candidatos;
    candidatos.reserve(n - 1);
    for (int i = 1; i <= n; ++i) {
        candidatos.clear();
        for (int v = 1; v <= n; ++v) {
            if (v != i) candidatos.push_back(v);
        }
        std::shuffle(candidatos.begin(), candidatos.end(), rng);
        for (int k = 0; k < OUT_DEGREE; ++k) {
            out << i << " " << candidatos[k] << " " << pesoDist(rng) << "\n";
        }
    }
}

int main() {
    std::string manifestPath = OUT_DIR + "/manifest.csv";
    std::ofstream manifest(manifestPath);
    if (!manifest) {
        std::cerr << "No se pudo crear " << manifestPath
                  << ". Crea antes el directorio con: mkdir -p " << OUT_DIR << "\n";
        return 1;
    }
    manifest << "n,m,archivo\n";

    for (int n : N_VALUES) {
        std::string fname = "sparse_n" + std::to_string(n) + ".mtx";
        std::string outPath = OUT_DIR + "/" + fname;

        unsigned seed = 2000u + static_cast<unsigned>(n);
        generarInstanciaDispersa(n, seed, outPath);

        long long m = static_cast<long long>(n) * OUT_DEGREE;
        manifest << n << "," << m << "," << fname << "\n";

        std::cout << "Generada instancia n=" << n << " (m=" << m << ") -> "
                  << outPath << "\n";
    }

    std::cout << "Manifest escrito en " << manifestPath << "\n";
    return 0;
}
