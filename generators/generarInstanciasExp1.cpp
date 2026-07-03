// Generador de instancias para el Experimento 1 del diseno experimental:
// variacion del tamano de entrada en grafos densos.
//
// Para cada n de una lista fija se genera un digrafo completo (sin lazos),
// es decir m = n*(n-1) = Theta(n^2), con pesos enteros positivos aleatorios
// uniformes en [1, MAX_PESO]. Se usan solo pesos positivos para evitar
// ciclos negativos, de forma que la solucion se pueda comparar directamente
// contra la de Floyd-Warshall.
//
// Salida: un archivo .mtx (formato Matrix Market, mismo formato que lee
// Graph::setData / Graph::readMatrixMarket) por cada n, mas un manifest.csv
// con las columnas n,m,archivo para que el arnes de experimentos sepa que
// instancias recorrer.
//
// El directorio de salida (OUT_DIR) debe existir antes de ejecutar este
// programa (crearlo con "mkdir -p instances/exp1_densos").
//
// Compilacion:  g++ -std=c++17 -O2 -o generarInstanciasExp1 generarInstanciasExp1.cpp
// Ejecucion  :  mkdir -p instances/exp1_densos && ./generarInstanciasExp1

#include <cstdint>
#include <fstream>
#include <iostream>
#include <random>
#include <string>
#include <vector>

static const int MAX_PESO = 1000;      // pesos en [1, MAX_PESO]
static const std::string OUT_DIR = "instances/exp1_densos";

// Tamanos de entrada para el experimento 1 (grafos densos, m ~= n^2). Se usa
// un rango en escala lineal (paso ~25) que permite observar con claridad la
// separacion entre O(n^4) del Algoritmo Base y O(n^3) de Floyd-Warshall,
// manteniendo acotado el tiempo total de los experimentos: el Algoritmo Base
// es el que domina el costo, ya que en n=200 ejecuta del orden de 1.6*10^9
// relajaciones por corrida. Si la maquina usada para medir es mas lenta o
// mas rapida de lo esperado, conviene calibrar primero con una corrida unica
// en n=200 antes de lanzar el barrido completo con las 32 o mas repeticiones
// que pide el enunciado.
static const std::vector<int> N_VALUES = {
    20, 40, 60, 80, 100, 125, 150, 175, 200
};

static void generarInstanciaDensa(int n, unsigned seed, const std::string& outPath) {
    std::mt19937_64 rng(seed);
    std::uniform_int_distribution<int> pesoDist(1, MAX_PESO);

    long long m = static_cast<long long>(n) * (n - 1); // grafo completo, sin lazos

    std::ofstream out(outPath);
    if (!out) {
        std::cerr << "No se pudo abrir para escritura: " << outPath << "\n";
        return;
    }
    out << "%%MatrixMarket matrix coordinate real general\n";
    out << "% Instancia Experimento 1 (denso, m ~= n^2): n=" << n
        << ", pesos uniformes en [1," << MAX_PESO << "], seed=" << seed << "\n";
    out << n << " " << n << " " << m << "\n";

    for (int i = 1; i <= n; ++i) {
        for (int j = 1; j <= n; ++j) {
            if (i == j) continue;          // sin lazos: se evita inflar m mas alla de n(n-1)
            out << i << " " << j << " " << pesoDist(rng) << "\n";
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
        std::string fname = "dense_n" + std::to_string(n) + ".mtx";
        std::string outPath = OUT_DIR + "/" + fname;

        unsigned seed = 1000u + static_cast<unsigned>(n); // reproducible por tamano
        generarInstanciaDensa(n, seed, outPath);

        long long m = static_cast<long long>(n) * (n - 1);
        manifest << n << "," << m << "," << fname << "\n";

        std::cout << "Generada instancia n=" << n << " (m=" << m << ") -> "
                  << outPath << "\n";
    }

    std::cout << "Manifest escrito en " << manifestPath << "\n";
    return 0;
}
