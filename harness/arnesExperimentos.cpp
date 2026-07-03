// Arnes de experimentos: recorre un manifest.csv (generado por los
// programas en generators/), mide el Algoritmo Base y Floyd-Warshall sobre
// cada instancia listada, y escribe un CSV con tiempo promedio, desviacion
// estandar y cuartiles por instancia y algoritmo. El calculo de cuartiles
// usa la funcion quartiles() de la suite uhr sugerida en el enunciado (ver
// uhr/utils.cpp).
//
// Uso:
//   ./arnesExperimentos <manifest.csv> <runs> <salida.csv>
//
// <manifest.csv> : ruta al manifest de un experimento (columnas variables,
//                  pero siempre debe incluir una columna "archivo" con la
//                  ruta del .mtx relativa a la carpeta del manifest).
// <runs>         : repeticiones por instancia (el enunciado exige >= 32).
// <salida.csv>   : archivo a crear (no debe existir previamente).
//
// El CSV de salida repite todas las columnas del manifest de entrada y
// agrega: algoritmo,t_mean_ns,t_stdev_ns,t_Q0,t_Q1,t_Q2,t_Q3,t_Q4
// (una fila por instancia y algoritmo, es decir el doble de filas que el
// manifest de entrada).
//
// Compilacion (desde la raiz del proyecto):
//   g++ -std=c++17 -O2 -Isrc -o arnesExperimentos harness/arnesExperimentos.cpp \
//       src/Graph.cpp src/WarshallLib.cpp src/BaseAPSPLib.cpp src/InstanceIO.cpp

#include <chrono>
#include <cmath>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "Graph.h"
#include "WarshallLib.h"
#include "BaseAPSPLib.h"
#include "InstanceIO.h"
#include "../uhr/utils.cpp"

using namespace std;
using Clock = chrono::high_resolution_clock;

static vector<string> split(const string& line, char sep) {
    vector<string> out;
    string cur;
    istringstream iss(line);
    while (getline(iss, cur, sep)) out.push_back(cur);
    return out;
}

static string dirnameOf(const string& path) {
    size_t pos = path.find_last_of("/\\");
    if (pos == string::npos) return ".";
    return path.substr(0, pos);
}

static int indexOf(const vector<string>& v, const string& target) {
    for (size_t i = 0; i < v.size(); ++i) {
        if (v[i] == target) return static_cast<int>(i);
    }
    return -1;
}

// Ejecuta "runs" repeticiones de fn() y escribe una fila de estadisticas.
template <typename Fn>
static void medirYEscribir(ofstream& out, const string& filaManifest,
                            const string& nombreAlgoritmo, long long runs, Fn fn) {
    vector<double> tiempos(static_cast<size_t>(runs));
    for (long long r = 0; r < runs; ++r) {
        auto t0 = Clock::now();
        fn();
        auto t1 = Clock::now();
        tiempos[static_cast<size_t>(r)] =
            chrono::duration<double, nano>(t1 - t0).count();
    }

    double mean = 0.0;
    for (double t : tiempos) mean += t;
    mean /= static_cast<double>(runs);

    double stdev = 0.0;
    for (double t : tiempos) {
        double dev = t - mean;
        stdev += dev * dev;
    }
    stdev = (runs > 1) ? sqrt(stdev / (runs - 1)) : 0.0;

    vector<double> q;
    quartiles(tiempos, q);

    out << filaManifest << "," << nombreAlgoritmo << ","
        << mean << "," << stdev << ","
        << q[0] << "," << q[1] << "," << q[2] << "," << q[3] << "," << q[4] << "\n";
}

int main(int argc, char** argv) {
    if (argc != 4) {
        cerr << "Uso: " << argv[0] << " <manifest.csv> <runs> <salida.csv>\n";
        return 1;
    }

    string manifestPath = argv[1];
    long long runs = stoll(argv[2]);
    string outPath = argv[3];

    if (runs < 32) {
        cerr << "Advertencia: el enunciado exige al menos 32 corridas por instancia "
             << "(se recibieron " << runs << ").\n";
    }
    if (runs < 4) {
        cerr << "runs debe ser al menos 4 (lo exige el calculo de cuartiles).\n";
        return 1;
    }

    ifstream manifest(manifestPath);
    if (!manifest) {
        cerr << "No se pudo abrir " << manifestPath << "\n";
        return 1;
    }

    string headerLine;
    if (!getline(manifest, headerLine)) {
        cerr << "Manifest vacio: " << manifestPath << "\n";
        return 1;
    }
    vector<string> headers = split(headerLine, ',');
    int archivoIdx = indexOf(headers, "archivo");
    if (archivoIdx < 0) {
        cerr << "El manifest no tiene columna 'archivo': " << manifestPath << "\n";
        return 1;
    }
    string baseDir = dirnameOf(manifestPath);

    ofstream out(outPath);
    if (!out) {
        cerr << "No se pudo crear " << outPath << "\n";
        return 1;
    }
    out << headerLine << ",algoritmo,t_mean_ns,t_stdev_ns,t_Q0,t_Q1,t_Q2,t_Q3,t_Q4\n";

    string line;
    int procesadas = 0;
    while (getline(manifest, line)) {
        if (line.empty()) continue;
        vector<string> fields = split(line, ',');
        if (static_cast<int>(fields.size()) <= archivoIdx) continue;

        string archivo = baseDir + "/" + fields[archivoIdx];
        cerr << "[" << (++procesadas) << "] Procesando " << archivo << " ...\n";

        // --- Floyd-Warshall: representacion matricial (Graph) ---
        auto grafos = Graph::setData(archivo);
        if (grafos.empty()) {
            cerr << "  no se pudo cargar como Graph, se omite.\n";
            continue;
        }
        Graph& g = grafos[0];

        medirYEscribir(out, line, "FloydWarshall", runs, [&]() {
            g.copyToSolution(); // reinicia dist = adj antes de cada corrida; costo O(n^2), despreciable frente a O(n^3)
            floydWarshall(g);
        });

        // --- Algoritmo Base: lista de aristas en el orden del archivo ---
        EdgeListInstance eli = leerEdgeList(archivo);
        if (eli.n == 0) {
            cerr << "  no se pudo cargar como lista de aristas, se omite Algoritmo Base.\n";
            continue;
        }

        medirYEscribir(out, line, "AlgoritmoBase", runs, [&]() {
            auto resultado = algoritmoBaseAPSP(eli.n, eli.edges);
            (void)resultado;
        });
    }

    cout << "Resultados escritos en " << outPath << " (" << procesadas << " instancias)\n";
    return 0;
}
