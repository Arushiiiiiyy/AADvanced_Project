#include <iostream>
#include <vector>
#include <random>
#include <algorithm>
#include <cmath>

using namespace std;

// Represents the graph using an Adjacency List (vector of vectors)
using Graph = vector<vector<int>>;

Graph watts_strogatz(int N, int K, double p) {
    // N: number of nodes, K: neighbors per node, p: rewiring probability
    
    Graph adj(N);
    
    // Setup random number generation
    random_device rd;
    mt19937 gen(rd());
    uniform_real_distribution<> dis(0.0, 1.0);
    uniform_int_distribution<> node_dist(0, N - 1);

    // 1. Initial Regular Ring Lattice (High C, High L)
    // Each node i connects to K neighbors (K/2 on each side)
    for (int i = 0; i < N; ++i) {
        for (int j = 1; j <= K / 2; ++j) {
            int neighbor1 = (i + j) % N;
            int neighbor2 = (i - j + N) % N;

            // Add edges (undirected)
            adj[i].push_back(neighbor1);
            adj[neighbor1].push_back(i);
            adj[i].push_back(neighbor2);
            adj[neighbor2].push_back(i);
        }
    }
    
    // Remove duplicate edges created by wrapping and ensure unique neighbors
    for (int i = 0; i < N; ++i) {
        sort(adj[i].begin(), adj[i].end());
        adj[i].erase(unique(adj[i].begin(), adj[i].end()), adj[i].end());
    }

    // 2. Rewiring Edges (Introduces Shortcuts)
    for (int i = 0; i < N; ++i) {
        // Iterate only over the first K/2 original neighbors (to avoid double counting rewires)
        for (int j = 0; j < K / 2; ++j) {
            // Check if we rewire this edge with probability p
            if (dis(gen) < p) {
                
                int original_neighbor = (i + j + 1) % N;
                
                // Find and remove the original edge (i, original_neighbor)
                // We use erase-remove idiom to find and remove the neighbor from i's list
                adj[i].erase(remove(adj[i].begin(), adj[i].end(), original_neighbor), adj[i].end());
                // And from the original_neighbor's list
                adj[original_neighbor].erase(remove(adj[original_neighbor].begin(), adj[original_neighbor].end(), i), adj[original_neighbor].end());
                
                // Find a new random node (new_neighbor)
                int new_neighbor = node_dist(gen);
                
                // Loop to ensure the new connection is not a self-loop and not a duplicate
                bool is_duplicate;
                do {
                    is_duplicate = (new_neighbor == i);
                    if (!is_duplicate) {
                        for (int neighbor : adj[i]) {
                            if (neighbor == new_neighbor) {
                                is_duplicate = true;
                                break;
                            }
                        }
                    }
                    if (is_duplicate) {
                        new_neighbor = node_dist(gen); // Pick a new node if invalid
                    }
                } while (is_duplicate);

                // Add the new edge (i, new_neighbor)
                adj[i].push_back(new_neighbor);
                adj[new_neighbor].push_back(i);
            }
        }
    }

    return adj;
}

// Example usage:
/*
int main() {
    int N = 100; // 100 nodes
    int K = 4;   // 4 neighbors
    double p = 0.1; // 10% rewiring (small-world range)

    Graph ws_graph = watts_strogatz(N, K, p);

    cout << "Watts-Strogatz Graph Generated (" << N << " nodes, p=" << p << ")\n";
    // Code for calculating L and C would go here, which requires more complex graph algorithms (e.g., BFS)
    return 0;
}
*/