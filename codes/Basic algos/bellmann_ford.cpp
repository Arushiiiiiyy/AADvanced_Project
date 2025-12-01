// Updated: non-interactive Bellman-Ford that accepts multiple edge-list files
// Usage: ./bellmann_ford <source_node> <edge_file1> [edge_file2 ...]

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

struct Edge {
    int u;
    int v;
    double w;
};

static const double INF = 1e18;

// Read an edge-list file. Each non-empty non-comment line should be:
// u v [w]
// If weight w is missing, it's assumed to be 1.0 (unweighted).
// Returns edges vector and sets max_node (largest index seen).
bool read_edge_list(const string &path, vector<Edge> &edges, int &max_node) {
    ifstream in(path);
    if (!in) return false;
    string line;
    max_node = -1;
    while (getline(in, line)) {
        // trim
        if (line.empty()) continue;
        // skip comments
        if (line.size() && (line[0] == '#' || line[0] == '%')) continue;
        istringstream iss(line);
        int u, v; double w = 1.0;
        if (!(iss >> u >> v)) continue; // malformed line
        if (iss >> w) {
            // weight provided
        }
        edges.push_back({u, v, w});
        // also treat as undirected by adding reverse edge if you want; keep as directed here
        max_node = max(max_node, max(u, v));
    }
    return true;
}

void write_output(const string &inpath, int src, const vector<double> &dist, bool negative_cycle) {
    fs::path p(inpath);
    string stem = p.stem().string();
    string outname = string("bellmanford_output_") + stem + string("_from_") + to_string(src) + string(".txt");
    ofstream out(outname);
    if (!out) {
        cerr << "Could not open output file " << outname << " for writing\n";
        return;
    }
    out << "Bellman-Ford shortest distances from node " << src << " :\n";
    if (negative_cycle) {
        out << "Network contains a negative-weight cycle reachable from the source. Distances are invalid.\n";
    }
    for (size_t i = 0; i < dist.size(); ++i) {
        out << "To Person " << i << ": ";
        if (dist[i] >= INF/2)
            out << "unreachable";
        else
            out << dist[i];
        out << '\n';
    }
    out.close();
    cout << "Wrote " << outname << " (nodes 0.." << (dist.size() ? dist.size()-1 : 0) << ")\n";
}

void process_file(const string &path, int src) {
    vector<Edge> edges;
    int max_node = -1;
    if (!read_edge_list(path, edges, max_node)) {
        cerr << "Warning: could not open input file '" << path << "' - skipping.\n";
        return;
    }

    int V = max_node + 1;
    if (src < 0) src = 0;
    if (src >= V) V = src + 1; // ensure source index fits

    vector<double> dist(V, INF);
    dist[src] = 0.0;

    // Relax edges (V-1) times
    for (int i = 0; i < V - 1; ++i) {
        bool changed = false;
        for (const auto &e : edges) {
            if (e.u < 0 || e.u >= V || e.v < 0 || e.v >= V) continue;
            if (dist[e.u] < INF/2 && dist[e.u] + e.w < dist[e.v]) {
                dist[e.v] = dist[e.u] + e.w;
                changed = true;
            }
        }
        if (!changed) break;
    }

    // Check for negative-weight cycles reachable from source
    bool neg_cycle = false;
    for (const auto &e : edges) {
        if (e.u < 0 || e.u >= V || e.v < 0 || e.v >= V) continue;
        if (dist[e.u] < INF/2 && dist[e.u] + e.w < dist[e.v]) {
            neg_cycle = true;
            break;
        }
    }

    write_output(path, src, dist, neg_cycle);
}

int main(int argc, char **argv) {
    if (argc < 3) {
        cerr << "Usage: " << argv[0] << " <source_node> <edge_file1> [edge_file2 ...]\n";
        cerr << "Each input edge file should contain lines: u v [w] (weight optional; default=1)" << endl;
        return 1;
    }

    int src = 0;
    try {
        src = stoi(argv[1]);
    } catch (...) {
        cerr << "Invalid source node: " << argv[1] << "\n";
        return 1;
    }

    for (int i = 2; i < argc; ++i) {
        string path = argv[i];
        process_file(path, src);
    }

    return 0;
}
