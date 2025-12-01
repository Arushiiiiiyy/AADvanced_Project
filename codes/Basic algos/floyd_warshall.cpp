// CLI Floyd–Warshall: reads edge-list files and writes all-pairs shortest-path matrices
// Usage: ./floydwarshall <edge_file1> [edge_file2 ...]

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <limits>
#include <algorithm>
#include <filesystem>

using namespace std;
namespace fs = std::filesystem;

static const double INF = 1e18;

struct Edge { int u, v; double w; };

// Read edge list of form: u v [w]. Lines starting with # or % are ignored.
bool read_edge_list(const string &path, vector<Edge> &edges, int &max_node) {
    ifstream in(path);
    if (!in) return false;
    string line;
    max_node = -1;
    while (getline(in, line)) {
        if (line.empty()) continue;
        if (line[0] == '#' || line[0] == '%') continue;
        istringstream iss(line);
        int u, v; double w = 1.0;
        if (!(iss >> u >> v)) continue; // skip malformed
        if (iss >> w) { /* weight provided */ }
        edges.push_back({u, v, w});
        // treat as undirected graph: also add reverse edge
        edges.push_back({v, u, w});
        max_node = max(max_node, max(u, v));
    }
    return true;
}

void write_output(const string &inpath, const vector<vector<double>> &dist, bool neg_cycle) {
    fs::path p(inpath);
    string stem = p.stem().string();
    string outname = string("floydwarshall_output_") + stem + string(".txt");
    ofstream out(outname);
    if (!out) {
        cerr << "Could not open output file " << outname << " for writing\n";
        return;
    }
    out << "Floyd–Warshall All-Pairs shortest distances\n";
    if (neg_cycle) {
        out << "Warning: Negative-weight cycle detected (dist[i][i] < 0 for some i). Results may be invalid.\n";
    }
    int V = (int)dist.size();
    for (int i = 0; i < V; ++i) {
        for (int j = 0; j < V; ++j) {
            if (dist[i][j] >= INF/2) out << "INF";
            else out << dist[i][j];
            if (j + 1 < V) out << '\t';
        }
        out << '\n';
    }
    out.close();
    cout << "Wrote " << outname << " (nodes 0.." << (V?V-1:0) << ")\n";
}

void process_file(const string &path) {
    vector<Edge> edges;
    int max_node = -1;
    if (!read_edge_list(path, edges, max_node)) {
        cerr << "Warning: could not open input file '" << path << "' - skipping.\n";
        return;
    }
    int V = max_node + 1;
    if (V <= 0) V = 1;

    // initialize dist matrix
    vector<vector<double>> dist(V, vector<double>(V, INF));
    for (int i = 0; i < V; ++i) dist[i][i] = 0.0;
    for (const auto &e : edges) {
        if (e.u >=0 && e.u < V && e.v >=0 && e.v < V) {
            dist[e.u][e.v] = min(dist[e.u][e.v], e.w);
        }
    }

    // Floyd–Warshall
    // Warn user for large V (costly)
    if (V > 800) {
        cerr << "Warning: V=" << V << "; Floyd–Warshall is O(V^3) and may be slow. Proceeding anyway.\n";
    }
    for (int k = 0; k < V; ++k) {
        for (int i = 0; i < V; ++i) {
            if (dist[i][k] >= INF/2) continue;
            for (int j = 0; j < V; ++j) {
                if (dist[k][j] >= INF/2) continue;
                double nd = dist[i][k] + dist[k][j];
                if (nd < dist[i][j]) dist[i][j] = nd;
            }
        }
    }

    // Detect negative cycles
    bool neg_cycle = false;
    for (int i = 0; i < V; ++i) if (dist[i][i] < 0) { neg_cycle = true; break; }

    write_output(path, dist, neg_cycle);
}

int main(int argc, char **argv) {
    if (argc < 2) {
        cerr << "Usage: " << argv[0] << " <edge_file1> [edge_file2 ...]\n";
        cerr << "Each input edge file should contain lines: u v [w] (weight optional; default=1)" << endl;
        return 1;
    }
    for (int i = 1; i < argc; ++i) process_file(string(argv[i]));
    return 0;
}
