#include <bits/stdc++.h>
using namespace std;

// Reads an undirected edge list file ("u v" per line), computes unweighted
// shortest-path distances from a source node (default 0) using BFS, and writes
// an output file with human-readable distances.
//
// Usage:
//   ./dijkstra_edgelist <source_node> <edge_file1> [edge_file2 ...]
//
// For each input file `foo_edges.txt` the program will write
// `dijkstra_output_foo_from_<src>.txt` in the same directory.

vector<int> bfs_distances_unweighted(const vector<vector<int>>& adj, int src) {
    int n = adj.size();
    vector<int> dist(n, -1);
    if (src < 0 || src >= n) return dist;
    queue<int> q;
    q.push(src);
    dist[src] = 0;
    while (!q.empty()) {
        int u = q.front(); q.pop();
        for (int v : adj[u]) {
            if (dist[v] == -1) {
                dist[v] = dist[u] + 1;
                q.push(v);
            }
        }
    }
    return dist;
}

bool process_file(const string& path, int src) {
    // Read edges and determine max node id
    ifstream fin(path);
    if (!fin) {
        cerr << "Failed to open " << path << "\n";
        return false;
    }
    vector<pair<int,int>> edges;
    int max_node = -1;
    string line;
    while (getline(fin, line)) {
        if (line.empty() || line[0] == '#') continue;
        stringstream ss(line);
        int u, v;
        if (!(ss >> u >> v)) continue;
        edges.emplace_back(u, v);
        max_node = max(max_node, max(u, v));
    }
    fin.close();

    int V = max_node + 1;
    if (V <= 0) V = 0;
    vector<vector<int>> adj(V);
    for (auto &e : edges) {
        int u = e.first, v = e.second;
        if (u >= 0 && u < V && v >= 0 && v < V) {
            adj[u].push_back(v);
            adj[v].push_back(u);
        }
    }

    vector<int> dist = bfs_distances_unweighted(adj, src);

    // Build output filename based on input name
    // input: /path/to/foo_edges.txt -> base foo (remove _edges.txt if present)
    string base = path;
    // find last '/'
    size_t slash = base.find_last_of('/');
    string filename = (slash == string::npos) ? base : base.substr(slash+1);
    string stem = filename;
    // remove suffix _edges.txt or .txt
    const string sfx = "_edges.txt";
    if (stem.size() > sfx.size() && stem.substr(stem.size()-sfx.size()) == sfx) {
        stem = stem.substr(0, stem.size()-sfx.size());
    } else if (stem.size() > 4 && stem.substr(stem.size()-4) == ".txt") {
        stem = stem.substr(0, stem.size()-4);
    }

    string outname = "dijkstra_output_" + stem + "_from_" + to_string(src) + ".txt";
    ofstream fout(outname);
    if (!fout) {
        cerr << "Failed to open output file " << outname << " for writing\n";
        return false;
    }

    fout << "Shortest Social Distances from person " << src << " :\n";
    for (size_t i = 0; i < dist.size(); ++i) {
        if (dist[i] == -1) fout << "To Person " << i << ": unreachable\n";
        else fout << "To Person " << i << ": " << dist[i] << "\n";
    }
    fout.close();
    cout << "Wrote " << outname << " (nodes 0.." << (V-1) << ")\n";
    return true;
}

int main(int argc, char** argv) {
    if (argc < 3) {
        cerr << "Usage: " << argv[0] << " <source_node> <edge_file1> [edge_file2 ...]\n";
        return 1;
    }
    int src = stoi(argv[1]);
    bool all_ok = true;
    for (int i = 2; i < argc; ++i) {
        string path = argv[i];
        bool ok = process_file(path, src);
        all_ok = all_ok && ok;
    }
    return all_ok ? 0 : 2;
}
