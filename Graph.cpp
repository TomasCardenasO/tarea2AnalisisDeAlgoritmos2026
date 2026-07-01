#include "Graph.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <cctype>

#if defined(__cpp_lib_filesystem) || (defined(__has_include) && __has_include(<filesystem>))
  #include <filesystem>
  namespace fs = std::filesystem;
  #define GRAPH_HAS_FS 1
#else
  #define GRAPH_HAS_FS 0
#endif

// Infinito: "no hay arista".
const double Graph::NO_EDGE = std::numeric_limits<double>::infinity();

// ---------------------------------------------------------------------------
// Constructores
// ---------------------------------------------------------------------------
Graph::Graph() : n(0), name("") {}

Graph::Graph(int n, const std::string& name) : n(n), name(name) {
    adj.assign(n, std::vector<double>(n, NO_EDGE));
    dist.assign(n, std::vector<double>(n, NO_EDGE));
}

// ---------------------------------------------------------------------------
// Almacena una matriz de adyacencia entregada y crea la matriz de soluciones
// ---------------------------------------------------------------------------
void Graph::setAdjacency(const std::vector<std::vector<double>>& matrix) {
    adj = matrix;
    n = static_cast<int>(matrix.size());
    copyToSolution();   // dist = copia de adj
}

void Graph::addEdge(int i, int j, double w) {
    if (i < 0 || i >= n || j < 0 || j >= n) return;
    adj[i][j] = w;
}

// La matriz de soluciones nace como una copia exacta de la de adyacencia.
void Graph::copyToSolution() {
    dist = adj;
}

// ---------------------------------------------------------------------------
// Acceso a la matriz de soluciones
// ---------------------------------------------------------------------------
// Referencia: permite manipular la matriz directamente desde afuera.
std::vector<std::vector<double>>& Graph::getSolution() {
    return dist;
}

const std::vector<std::vector<double>>& Graph::getSolution() const {
    return dist;
}

// Copia: para trabajarla aparte y luego devolverla con setSolution().
std::vector<std::vector<double>> Graph::getSolutionCopy() const {
    return dist;
}

// Recibe de vuelta la matriz manipulada afuera y la guarda.
void Graph::setSolution(const std::vector<std::vector<double>>& solution) {
    if (static_cast<int>(solution.size()) != n) {
        std::cerr << "setSolution: la matriz recibida (" << solution.size()
                  << ") no coincide con n=" << n << ". No se aplico.\n";
        return;
    }
    dist = solution;
}

// ---------------------------------------------------------------------------
// Impresion
// ---------------------------------------------------------------------------
static void printMatrix(const std::vector<std::vector<double>>& m,
                        int n, int limit) {
    int rows = (limit < 0) ? n : std::min(limit, n);
    int cols = rows;

    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            double v = m[i][j];
            std::cout << std::setw(10);
            if (v == Graph::NO_EDGE)
                std::cout << "INF";
            else
                std::cout << std::setprecision(4) << v;
            std::cout << ' ';
        }
        std::cout << '\n';
    }
    if (limit >= 0 && n > limit) {
        std::cout << "... (mostrando " << limit << "x" << limit
                  << " de " << n << "x" << n
                  << "; usa printGraph(-1)/printDist(-1) para la matriz completa)\n";
    }
}

void Graph::printGraph(int limit) const {
    std::cout << "== Matriz de Adyacencia de \"" << name << "\" ("
              << n << "x" << n << ") ==\n";
    printMatrix(adj, n, limit);
}

void Graph::printDist(int limit) const {
    std::cout << "== Matriz de Distancias/Soluciones de \"" << name << "\" ("
              << n << "x" << n << ") ==\n";
    printMatrix(dist, n, limit);
}

// ---------------------------------------------------------------------------
// Lectura de un archivo Matrix Market (.mtx)
// ---------------------------------------------------------------------------
Graph Graph::readMatrixMarket(const std::string& filePath) {
    std::ifstream in(filePath);
    if (!in) {
        std::cerr << "No se pudo abrir: " << filePath << "\n";
        return Graph();
    }

    std::string line;
    bool symmetric = false;

    // 1) Cabecera y comentarios (lineas que empiezan con '%').
    while (std::getline(in, line)) {
        if (line.empty()) continue;
        if (line[0] == '%') {
            std::string low = line;
            std::transform(low.begin(), low.end(), low.begin(),
                           [](unsigned char c){ return std::tolower(c); });
            if (low.find("symmetric") != std::string::npos) symmetric = true;
            continue;
        }
        break; // primera linea de datos: dimensiones
    }

    // 2) Dimensiones: rows cols nnz
    int rows = 0, cols = 0; long long nnz = 0;
    {
        std::istringstream iss(line);
        iss >> rows >> cols >> nnz;
    }
    int size = std::max(rows, cols);

    // Nombre del grafo = nombre del archivo sin extension ni carpeta.
    std::string base = filePath;
    size_t slash = base.find_last_of("/\\");
    if (slash != std::string::npos) base = base.substr(slash + 1);
    size_t dot = base.find_last_of('.');
    if (dot != std::string::npos) base = base.substr(0, dot);

    Graph g(size, base);

    // 3) Aristas: i j value (1-indexado). Si no hay value se asume 1.
    long long read = 0;
    while (std::getline(in, line)) {
        if (line.empty() || line[0] == '%') continue;
        std::istringstream iss(line);
        int i, j; double w = 1.0;
        if (!(iss >> i >> j)) continue;
        if (!(iss >> w)) w = 1.0;      // formato 'pattern' sin peso
        g.adj[i - 1][j - 1] = w;
        if (symmetric && i != j) g.adj[j - 1][i - 1] = w;
        ++read;
    }
    std::cout << "  -> \"" << base << "\": " << size << " vertices, "
              << read << " aristas leidas"
              << (symmetric ? " (simetrica)" : "") << "\n";

    g.copyToSolution();  // matriz de soluciones = copia de adyacencia
    return g;
}

// ---------------------------------------------------------------------------
// setData: detecta 1 o varios grafos en la ruta dada
// ---------------------------------------------------------------------------
std::vector<Graph> Graph::setData(const std::string& path) {
    std::vector<Graph> graphs;

#if GRAPH_HAS_FS
    std::error_code ec;
    if (fs::is_directory(path, ec)) {
        std::vector<std::string> files;
        for (const auto& entry : fs::recursive_directory_iterator(path, ec)) {
            if (entry.is_regular_file() && entry.path().extension() == ".mtx")
                files.push_back(entry.path().string());
        }
        std::sort(files.begin(), files.end());
        std::cout << "setData: " << files.size()
                  << " archivo(s) .mtx encontrado(s) en la carpeta.\n";
        for (const auto& f : files) {
            Graph g = readMatrixMarket(f);
            if (g.n > 0) graphs.push_back(std::move(g));
        }
        return graphs;
    }
#endif

    std::cout << "setData: cargando un unico archivo.\n";
    Graph g = readMatrixMarket(path);
    if (g.n > 0) graphs.push_back(std::move(g));
    return graphs;
}
