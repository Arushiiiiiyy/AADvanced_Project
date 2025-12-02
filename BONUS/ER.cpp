#include <vector>
#include <random>
#include <iostream>

using namespace std;

using Graph = vector<vector<int>>;

Graph erdos_renyi(int N, double p) {
    // N: number of nodes, p: probability of an edge existing
    
    Graph adj(N);

    // Setup random number generation
    random_device rd;
    mt19937 gen(rd());
    uniform_real_distribution<> dis(0.0, 1.0);

    // Iterate over all unique pairs of nodes (i, j) where i < j
    for (int i = 0; i < N; ++i) {
        for (int j = i + 1; j < N; ++j) {
            // If random number is less than p, create an edge
            if (dis(gen) < p) {
                // Add edges (undirected)
                adj[i].push_back(j);
                adj[j].push_back(i);
            }
        }
    }

    return adj;
}

// Example usage:
/*
int main() {
    int N = 100; // 100 nodes
    double p = 0.04; // Edge probability (to achieve similar avg degree to WS)

    Graph er_graph = erdos_renyi(N, p);

    cout << "Erdos-Renyi Graph Generated (" << N << " nodes, p=" << p << ")\n";
    return 0;
}
*/