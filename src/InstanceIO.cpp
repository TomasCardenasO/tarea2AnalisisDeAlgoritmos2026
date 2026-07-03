#include "InstanceIO.h"

#include <algorithm>
#include <cctype>
#include <cmath>
#include <fstream>
#include <iostream>
#include <sstream>

using namespace std;

EdgeListInstance leerEdgeList(const string& path) {
    EdgeListInstance inst;

    ifstream in(path);
    if (!in) {
        cerr << "leerEdgeList: no se pudo abrir " << path << "\n";
        return inst;
    }

    string line;
    bool symmetric = false;

    while (getline(in, line)) {
        if (line.empty()) continue;
        if (line[0] == '%') {
            string low = line;
            transform(low.begin(), low.end(), low.begin(),
                      [](unsigned char c) { return tolower(c); });
            if (low.find("symmetric") != string::npos) symmetric = true;
            continue;
        }
        break; // primera linea de datos: dimensiones
    }

    int rows = 0, cols = 0;
    long long nnz = 0;
    {
        istringstream iss(line);
        iss >> rows >> cols >> nnz;
    }
    inst.n = max(rows, cols);
    inst.edges.reserve(static_cast<size_t>(nnz) * (symmetric ? 2 : 1));

    while (getline(in, line)) {
        if (line.empty() || line[0] == '%') continue;
        istringstream iss(line);
        int i, j;
        double w = 1.0;
        if (!(iss >> i >> j)) continue;
        if (!(iss >> w)) w = 1.0; // formato 'pattern' sin peso

        int wi = static_cast<int>(llround(w));
        inst.edges.push_back({i - 1, j - 1, wi});
        if (symmetric && i != j) inst.edges.push_back({j - 1, i - 1, wi});
    }

    return inst;
}
