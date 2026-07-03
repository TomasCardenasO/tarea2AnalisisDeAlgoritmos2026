// Generador de instancias para el Experimento 3 del diseno experimental:
// sensibilidad a la densidad de aristas con cantidad de vertices constante.
//
// Fija n = N_FIJO y genera una serie de grafos con m creciente, desde una
// topologia dispersa hasta el grafo completo (m = n*(n-1)), en pasos
// lineales de un porcentaje de m_max. Para que las instancias formen una
// secuencia anidada (cada una con mas aristas es un superconjunto de la
// anterior, y comparten el mismo peso en las aristas que tienen en comun),
// se genera una unica permutacion aleatoria de los n*(n-1) pares ordenados
// posibles (i != j) junto con un peso fijo por par, y cada instancia de
// salida es el prefijo de esa permutacion de tamano m. Esto aisla la
// variable m: mismo n, mismo generador de aristas, solo cambia cuantas se
// incluyen.
//
// n = 150 se eligio para coincidir con uno de los puntos intermedios del
// Experimento 1 (grafos densos): asi el caso 100% de este experimento
// (grafo completo con n=150) es directamente comparable con la instancia
// dense_n150.mtx generada alli.
//
// Salida: un archivo .mtx por cada fraccion en instances/exp3_densidad/,
// mas un manifest.csv con columnas frac_pct,n,m,archivo.
//
// El directorio de salida debe existir antes de ejecutar este programa
// (mkdir -p instances/exp3_densidad).
//
// Compilacion:  g++ -std=c++17 -O2 -o generarInstanciasExp3 generarInstanciasExp3.cpp
// Ejecucion  :  mkdir -p instances/exp3_densidad && ./generarInstanciasExp3

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <random>
#include <string>
#include <utility>
#include <vector>

static const int N_FIJO = 150;
static const int MAX_PESO = 1000;
static const std::string OUT_DIR = "instances/exp3_densidad";
static const unsigned SEED = 3150u;

// Fracciones lineales de m_max = n*(n-1) a recorrer (de disperso a completo).
static const std::vector<int> PORCENTAJES = {
    10, 20, 30, 40, 50, 60, 70, 80, 90, 100
};

int main() {
    int n = N_FIJO;
    long long mMax = static_cast<long long>(n) * (n - 1);

    std::mt19937_64 rng(SEED);
    std::uniform_int_distribution<int> pesoDist(1, MAX_PESO);

    // Enumerar todos los pares ordenados (i, j), i != j, con 1 <= i,j <= n.
    std::vector<std::pair<int, int>> pares;
    pares.reserve(static_cast<size_t>(mMax));
    for (int i = 1; i <= n; ++i) {
        for (int j = 1; j <= n; ++j) {
            if (i == j) continue;
            pares.push_back({i, j});
        }
    }

    // Un unico peso aleatorio por par, fijo independientemente de cuantas
    // aristas se incluyan despues (garantiza instancias anidadas y
    // consistentes entre si).
    std::vector<int> pesos(pares.size());
    for (auto& w : pesos) w = pesoDist(rng);

    // Permutacion aleatoria del orden de aristas; los prefijos de esta
    // permutacion son las instancias con m creciente.
    std::vector<size_t> orden(pares.size());
    for (size_t k = 0; k < orden.size(); ++k) orden[k] = k;
    std::shuffle(orden.begin(), orden.end(), rng);

    std::string manifestPath = OUT_DIR + "/manifest.csv";
    std::ofstream manifest(manifestPath);
    if (!manifest) {
        std::cerr << "No se pudo crear " << manifestPath
                  << ". Crea antes el directorio con: mkdir -p " << OUT_DIR << "\n";
        return 1;
    }
    manifest << "frac_pct,n,m,archivo\n";

    for (int pct : PORCENTAJES) {
        long long m = llround((pct / 100.0) * static_cast<double>(mMax));
        if (m < 1) m = 1;
        if (m > mMax) m = mMax;

        std::string fname = "n" + std::to_string(n) + "_frac" + std::to_string(pct) + ".mtx";
        std::string outPath = OUT_DIR + "/" + fname;

        std::ofstream out(outPath);
        if (!out) {
            std::cerr << "No se pudo abrir para escritura: " << outPath << "\n";
            continue;
        }
        out << "%%MatrixMarket matrix coordinate real general\n";
        out << "% Instancia Experimento 3 (n fijo, m variable): n=" << n
            << ", m=" << m << " (" << pct << "% de m_max=" << mMax << "), seed=" << SEED << "\n";
        out << n << " " << n << " " << m << "\n";

        for (long long k = 0; k < m; ++k) {
            size_t idx = orden[static_cast<size_t>(k)];
            out << pares[idx].first << " " << pares[idx].second << " " << pesos[idx] << "\n";
        }

        manifest << pct << "," << n << "," << m << "," << fname << "\n";
        std::cout << "Generada instancia frac=" << pct << "% (n=" << n << ", m=" << m
                  << ") -> " << outPath << "\n";
    }

    std::cout << "Manifest escrito en " << manifestPath << "\n";
    return 0;
}
