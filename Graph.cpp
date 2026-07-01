#include "Graph.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <cctype>

using namespace std;

// <filesystem> es opcional: si no esta disponible, setData() trata
// cualquier ruta como un archivo unico.
#if defined(__cpp_lib_filesystem) || (defined(__has_include) && __has_include(<filesystem>))
  #include <filesystem>
  namespace fs = std::filesystem;
  #define GRAPH_HAS_FS 1
#else
  #define GRAPH_HAS_FS 0
#endif

const double Graph::NO_EDGE = numeric_limits<double>::infinity();

Graph::Graph() : n(0), name("") {}

Graph::Graph(int n, const string& name) : n(n), name(name) {
    adj.assign(n, vector<double>(n, NO_EDGE));
    dist.assign(n, vector<double>(n, NO_EDGE));
}

void Graph::setAdjacency(const vector<vector<double>>& matrix) {
    adj = matrix;
    n = static_cast<int>(matrix.size());
    copyToSolution();
}

void Graph::addEdge(int i, int j, double w) {
    if (i < 0 || i >= n || j < 0 || j >= n) return;
    adj[i][j] = w;
}

void Graph::copyToSolution() {
    dist = adj;
}

vector<vector<double>>& Graph::getSolution() {
    return dist;
}

const vector<vector<double>>& Graph::getSolution() const {
    return dist;
}

vector<vector<double>> Graph::getSolutionCopy() const {
    return dist;
}

void Graph::setSolution(const vector<vector<double>>& solution) {
    if (static_cast<int>(solution.size()) != n) {
        cerr << "setSolution: la matriz recibida (" << solution.size()
             << ") no coincide con n=" << n << ". No se aplico.\n";
        return;
    }
    dist = solution;
}

// Imprime m (n x n); limit < 0 = completa, si no, esquina limit x limit.
static void printMatrix(const vector<vector<double>>& m,
                        int n, int limit) {
    int rows = (limit < 0) ? n : min(limit, n);
    int cols = rows;

    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            double v = m[i][j];
            cout << setw(10);
            if (v == Graph::NO_EDGE)
                cout << "INF";
            else
                cout << setprecision(4) << v;
            cout << ' ';
        }
        cout << '\n';
    }
    if (limit >= 0 && n > limit) {
        cout << "... (mostrando " << limit << "x" << limit
             << " de " << n << "x" << n
             << "; usa printGraph(-1)/printDist(-1) para la matriz completa)\n";
    }
}

void Graph::printGraph(int limit) const {
    cout << "== Matriz de Adyacencia de \"" << name << "\" ("
         << n << "x" << n << ") ==\n";
    printMatrix(adj, n, limit);
}

void Graph::printDist(int limit) const {
    cout << "== Matriz de Distancias/Soluciones de \"" << name << "\" ("
         << n << "x" << n << ") ==\n";
    printMatrix(dist, n, limit);
}

// Lee un archivo Matrix Market (.mtx): comentarios '%', linea de
// dimensiones y luego aristas "i j peso" (1-indexadas).
Graph Graph::readMatrixMarket(const string& filePath) {
    ifstream in(filePath);
    if (!in) {
        cerr << "No se pudo abrir: " << filePath << "\n";
        return Graph();
    }

    string line;
    bool symmetric = false;

    while (getline(in, line)) {
        if (line.empty()) continue;
        if (line[0] == '%') {
            string low = line;
            transform(low.begin(), low.end(), low.begin(),
                           [](unsigned char c){ return tolower(c); });
            if (low.find("symmetric") != string::npos) symmetric = true;
            continue;
        }
        break; // primera linea de datos: dimensiones
    }

    int rows = 0, cols = 0; long long nnz = 0;
    {
        istringstream iss(line);
        iss >> rows >> cols >> nnz;
    }
    int size = max(rows, cols);

    // Nombre del grafo = nombre de archivo sin ruta ni extension.
    string base = filePath;
    size_t slash = base.find_last_of("/\\");
    if (slash != string::npos) base = base.substr(slash + 1);
    size_t dot = base.find_last_of('.');
    if (dot != string::npos) base = base.substr(0, dot);

    Graph g(size, base);

    long long read = 0;
    while (getline(in, line)) {
        if (line.empty() || line[0] == '%') continue;
        istringstream iss(line);
        int i, j; double w = 1.0;
        if (!(iss >> i >> j)) continue;
        if (!(iss >> w)) w = 1.0;      // formato 'pattern' sin peso
        g.adj[i - 1][j - 1] = w;
        if (symmetric && i != j) g.adj[j - 1][i - 1] = w;
        ++read;
    }
    cout << "  -> \"" << base << "\": " << size << " vertices, "
         << read << " aristas leidas"
         << (symmetric ? " (simetrica)" : "") << "\n";

    g.copyToSolution();
    return g;
}

// Detecta si "path" es archivo o carpeta y construye los grafos correspondientes.
vector<Graph> Graph::setData(const string& path) {
    vector<Graph> graphs;

#if GRAPH_HAS_FS
    error_code ec;
    if (fs::is_directory(path, ec)) {
        vector<string> files;
        for (const auto& entry : fs::recursive_directory_iterator(path, ec)) {
            if (entry.is_regular_file() && entry.path().extension() == ".mtx")
                files.push_back(entry.path().string());
        }
        sort(files.begin(), files.end());
        cout << "setData: " << files.size()
             << " archivo(s) .mtx encontrado(s) en la carpeta.\n";
        for (const auto& f : files) {
            Graph g = readMatrixMarket(f);
            if (g.n > 0) graphs.push_back(move(g));
        }
        return graphs;
    }
#endif

    cout << "setData: cargando un unico archivo.\n";
    Graph g = readMatrixMarket(path);
    if (g.n > 0) graphs.push_back(move(g));
    return graphs;
}
