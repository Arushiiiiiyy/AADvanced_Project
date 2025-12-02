#include <vector>
#include <random>
#include <numeric>
#include <iostream>

using namespace std;

using Graph = vector<vector<int>>;

Graph barabasi_albert(int N, int m) {
    // N: final number of nodes, m: number of edges each new node adds
    
    Graph adj(N);
    
    // Setup random number generation
    random_device rd;
    mt19937 gen(rd());
    
    // Keep track of all current edges (used for preferential selection)
    // This list will contain 'degree' number of entries per node
    vector<int> edge_list; 
    
    // 1. Initial Graph (m0 = m)
    // Start with m fully connected nodes (a clique)
    int initial_nodes = m;
    for (int i = 0; i < initial_nodes; ++i) {
        for (int j = i + 1; j < initial_nodes; ++j) {
            adj[i].push_back(j);
            adj[j].push_back(i);
            
            // Add nodes to the edge_list based on their new degree
            edge_list.push_back(i);
            edge_list.push_back(j);
        }
    }
    
    // 2. Add remaining nodes (N - m) with preferential attachment
    for (int i = initial_nodes; i < N; ++i) {
        // Sample m nodes from the current edge_list to connect to (with replacement)
        // Since we are sampling from edge_list, the probability of selecting a node 
        // is proportional to its degree (preferential attachment)
        
        // Setup uniform distribution over the current size of the edge_list
        uniform_int_distribution<> edge_dist(0, edge_list.size() - 1);
        
        // Track unique nodes chosen to ensure no duplicates in the m new edges
        vector<int> chosen_neighbors;
        
        while (chosen_neighbors.size() < m) {
            int selected_index = edge_dist(gen);
            int neighbor = edge_list[selected_index];
            
            // Check if this neighbor is already selected
            if (find(chosen_neighbors.begin(), chosen_neighbors.end(), neighbor) == chosen_neighbors.end()) {
                
                // Add the edge (i, neighbor)
                adj[i].push_back(neighbor);
                adj[neighbor].push_back(i);

                // Add both i and neighbor to the edge_list for future selection
                edge_list.push_back(i);
                edge_list.push_back(neighbor);
                
                chosen_neighbors.push_back(neighbor);
            }
            // If the node was a duplicate, the loop continues to find the next unique neighbor
        }
    }

    return adj;
}

// Example usage:
/*
int main() {
    int N = 100; // 100 nodes
    int m = 3;   // New nodes attach to 3 existing nodes

    Graph ba_graph = barabasi_albert(N, m);

    cout << "Barabasi-Albert Graph Generated (" << N << " nodes, m=" << m << ")\n";
    return 0;
}
*/