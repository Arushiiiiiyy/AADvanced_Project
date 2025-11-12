#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
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
        adj[e.second].push_back(e.first);  // Undirected graph
    }
    return adj;
}

void save_centrality(const vector<int>& degs, const string& out) {
    ofstream fout(out);
    if (!fout.is_open()) {
        cerr << "Error opening output file: " << out << endl;
        exit(1);
    }
    fout << "node,degree" << endl;
    for (size_t i = 0; i < degs.size(); ++i)
        fout << i << "," << degs[i] << endl;
    fout.close();
}

vector<int> degree_centrality(const vector<vector<int>>& adj) {
    vector<int> degrees(adj.size());
    for (size_t i = 0; i < adj.size(); ++i)
        degrees[i] = adj[i].size();
    return degrees;
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

    // Timing start
    auto start = chrono::high_resolution_clock::now();
    vector<int> degs = degree_centrality(adj);
    auto end = chrono::high_resolution_clock::now();

    chrono::duration<double> elapsed = end - start;
    cout << "Time taken: " << elapsed.count() << " seconds." << endl;

    // Save time to a file named like output_csv but with _time.txt suffix
    string time_file = output_csv.substr(0, output_csv.find_last_of('.')) + "_time.txt";
    ofstream tfile(time_file);
    if (!tfile.is_open()) {
        cerr << "Error opening time file: " << time_file << endl;
        return 1;
    }
    tfile << elapsed.count() << endl;
    tfile.close();

    save_centrality(degs, output_csv);

    return 0;
}
