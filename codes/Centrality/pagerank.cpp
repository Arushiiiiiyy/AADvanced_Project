#include <iostream>
#include <vector>
#include <fstream>
#include <iomanip>
#include <cmath>
#include <chrono>
#include <string>
using namespace std;

vector<vector<int>> read_graph(const string& filename, int& n) {
    ifstream fin(filename);
    if (!fin.is_open()) {
        cerr << "Error opening file: " << filename << endl;
        exit(1);
    }

    int u, v;
    vector<pair<int,int>> edges;
    n = 0;
    while (fin >> u >> v) {
        edges.push_back({u, v});
        n = max(n, max(u, v) + 1);
    }
    fin.close();

    vector<vector<int>> adj(n);
    for (auto& e : edges) {
        adj[e.first].push_back(e.second); // Directed graph
    }
    return adj;
}

void save_centrality(const vector<double>& vals, const string& out) {
    ofstream fout(out);
    if (!fout.is_open()) {
        cerr << "Error opening output file: " << out << endl;
        exit(1);
    }
    fout << "node,pagerank" << endl;
    for (size_t i = 0; i < vals.size(); ++i)
        fout << i << "," << fixed << setprecision(6) << vals[i] << endl;
    fout.close();
}

vector<double> pagerank(const vector<vector<int>>& adj, double d=0.85, int iters=100, double tol=1e-6) {
    int n = adj.size();
    vector<double> x(n, 1.0 / n), x_next(n);

    for (int iter = 0; iter < iters; ++iter) {
        for (int i = 0; i < n; ++i) {
            x_next[i] = (1-d)/n;
        }

        for (int v = 0; v < n; ++v) {
            if (!adj[v].empty()) {
                for (int w : adj[v]) {
                    x_next[w] += d * x[v] / adj[v].size();
                }
            }
        }

        double diff = 0;
        for (int i = 0; i < n; ++i) diff += abs(x_next[i] - x[i]);
        if (diff < tol) break;
        x = x_next;
    }
    return x_next;
}

int main(int argc, char** argv) {
    if (argc < 3) {
        cerr << "Usage: " << argv[0] << " <edges_file> <output_csv>" << endl;
        return 1;
    }

    string edges_file = argv[1];
    string output_csv = argv[2];

    int n;
    vector<vector<int>> adj = read_graph(edges_file, n);

    auto start = chrono::high_resolution_clock::now();
    vector<double> vals = pagerank(adj, 0.85, 200);
    auto end = chrono::high_resolution_clock::now();
    chrono::duration<double> elapsed = end - start;

    cout << "Time taken: " << elapsed.count() << " seconds." << endl;

    // Save time file
    string time_file = output_csv.substr(0, output_csv.find_last_of('.')) + "_time.txt";
    ofstream tfile(time_file);
    if (!tfile.is_open()) {
        cerr << "Error opening time file: " << time_file << endl;
        return 1;
    }
    tfile << elapsed.count() << endl;
    tfile.close();

    save_centrality(vals, output_csv);

    return 0;
}
