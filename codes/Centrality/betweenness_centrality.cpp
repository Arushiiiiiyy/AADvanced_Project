#include <iostream>
#include <vector>
#include <queue>
#include <stack>
#include <fstream>
#include <sstream>
#include <iomanip>
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
    vector<pair<int, int>> edges; 
    n = 0;
    while (fin >> u >> v) { 
        edges.push_back({u, v}); 
        n = max(n, max(u, v) + 1);
    }
    fin.close();

    vector<vector<int>> adj(n);
    for (auto& e : edges) { 
        adj[e.first].push_back(e.second); 
        adj[e.second].push_back(e.first);
    }
    return adj;
}

void save_centrality(const vector<double>& vals, const string& out) {
    ofstream fout(out);
    if (!fout.is_open()) {
        cerr << "Error opening output file: " << out << endl;
        exit(1);
    }
    fout << "node,betweenness" << endl;
    for (size_t i = 0; i < vals.size(); ++i)
        fout << i << "," << fixed << setprecision(6) << vals[i] << endl;
    fout.close();
}

/**
 * Brandes' algorithm for betweenness centrality
 */
vector<double> betweenness(const vector<vector<int>>& adj) {
    int n = adj.size();
    vector<double> bc(n, 0.0);
    for (int s = 0; s < n; ++s) {
        vector<int> pred[n];
        vector<int> dist(n, -1), sigma(n, 0);
        queue<int> q;
        stack<int> S;
        dist[s] = 0; sigma[s] = 1; q.push(s);

        while (!q.empty()) {
            int v = q.front(); q.pop(); S.push(v);
            for (int w : adj[v]) {
                if (dist[w] < 0) {
                    q.push(w); dist[w] = dist[v] + 1;
                }
                if (dist[w] == dist[v] + 1) {
                    sigma[w] += sigma[v]; pred[w].push_back(v);
                }
            }
        }

        vector<double> delta(n, 0.0);
        while (!S.empty()) {
            int w = S.top(); S.pop();
            for (int v : pred[w])
                delta[v] += ((double)sigma[v] / sigma[w]) * (1.0 + delta[w]);
            if (w != s) bc[w] += delta[w];
        }
    }
    for (double& x : bc) x /= 2.0;
    return bc;
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
    vector<double> vals = betweenness(adj);
    auto end = chrono::high_resolution_clock::now();
    chrono::duration<double> elapsed = end - start;

    cout << "Time taken: " << elapsed.count() << " seconds." << endl;

    // Save time file with same base as output_csv + "_time.txt"
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
