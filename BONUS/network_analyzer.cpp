// Network Analyzer - Reads generated graph data and calculates metrics
// Computes: Average Clustering Coefficient (C), Average Shortest Path Length (L)

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <queue>
#include <string>
#include <algorithm>
#include <cmath>
#include <iomanip>

using namespace std;
using Graph = vector<vector<int>>;

// Load graph from adjacency list file
Graph load_graph(const string& filename, int& N) {
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Error: Could not open file " << filename << endl;
        exit(1);
    }
    
    file >> N;
    file.ignore(); // Skip newline after N
    
    Graph adj(N);
    string line;
    
    while (getline(file, line)) {
        if (line.empty()) continue;
        
        stringstream ss(line);
        int node;
        char colon;
        ss >> node >> colon; // Read "node_id:"
        
        int neighbor;
        while (ss >> neighbor) {
            adj[node].push_back(neighbor);
        }
    }
    
    file.close();
    return adj;
}

// Calculate shortest path from source using BFS
vector<int> bfs_shortest_path(const Graph& adj, int source) {
    int N = adj.size();
    vector<int> dist(N, -1);
    queue<int> q;
    
    dist[source] = 0;
    q.push(source);
    
    while (!q.empty()) {
        int u = q.front();
        q.pop();
        
        for (int v : adj[u]) {
            if (dist[v] == -1) {
                dist[v] = dist[u] + 1;
                q.push(v);
            }
        }
    }
    
    return dist;
}

// Calculate Average Shortest Path Length (L)
double calculate_avg_path_length(const Graph& adj) {
    int N = adj.size();
    long long total_dist = 0;
    long long count = 0;
    
    for (int i = 0; i < N; ++i) {
        vector<int> dist = bfs_shortest_path(adj, i);
        
        for (int j = 0; j < N; ++j) {
            if (i != j && dist[j] != -1) {
                total_dist += dist[j];
                count++;
            }
        }
    }
    
    return (count > 0) ? (double)total_dist / count : 0.0;
}

// Calculate clustering coefficient for a single node
double node_clustering_coefficient(const Graph& adj, int node) {
    const vector<int>& neighbors = adj[node];
    int k = neighbors.size();
    
    if (k < 2) return 0.0;
    
    // Count triangles (edges between neighbors)
    int edges_between_neighbors = 0;
    for (int i = 0; i < k; ++i) {
        for (int j = i + 1; j < k; ++j) {
            int u = neighbors[i];
            int v = neighbors[j];
            
            // Check if edge (u, v) exists
            if (find(adj[u].begin(), adj[u].end(), v) != adj[u].end()) {
                edges_between_neighbors++;
            }
        }
    }
    
    // Clustering coefficient = actual edges / possible edges
    int possible_edges = (k * (k - 1)) / 2;
    return (double)edges_between_neighbors / possible_edges;
}

// Calculate Average Clustering Coefficient (C)
double calculate_avg_clustering(const Graph& adj) {
    int N = adj.size();
    double total_clustering = 0.0;
    
    for (int i = 0; i < N; ++i) {
        total_clustering += node_clustering_coefficient(adj, i);
    }
    
    return total_clustering / N;
}

// Calculate degree distribution statistics
void calculate_degree_stats(const Graph& adj) {
    int N = adj.size();
    vector<int> degrees(N);
    
    for (int i = 0; i < N; ++i) {
        degrees[i] = adj[i].size();
    }
    
    double avg_degree = 0.0;
    int max_degree = 0;
    int min_degree = N;
    
    for (int d : degrees) {
        avg_degree += d;
        max_degree = max(max_degree, d);
        min_degree = min(min_degree, d);
    }
    avg_degree /= N;
    
    cout << "  Avg Degree: " << fixed << setprecision(2) << avg_degree << endl;
    cout << "  Min Degree: " << min_degree << endl;
    cout << "  Max Degree: " << max_degree << endl;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        cerr << "Usage: " << argv[0] << " <graph_file.txt>" << endl;
        cerr << "Example: " << argv[0] << " small_world_analysis_data/data_proof_WS.txt" << endl;
        return 1;
    }
    
    string filename = argv[1];
    cout << "\n=== Network Analyzer ===" << endl;
    cout << "Loading graph from: " << filename << endl;
    
    int N;
    Graph adj = load_graph(filename, N);
    
    cout << "\nGraph Properties:" << endl;
    cout << "  Nodes: " << N << endl;
    
    // Calculate edges
    int edges = 0;
    for (const auto& neighbors : adj) {
        edges += neighbors.size();
    }
    edges /= 2; // Undirected graph
    cout << "  Edges: " << edges << endl;
    
    calculate_degree_stats(adj);
    
    cout << "\nCalculating Network Metrics..." << endl;
    
    // Calculate C
    cout << "  Computing Clustering Coefficient (C)..." << flush;
    double C = calculate_avg_clustering(adj);
    cout << " Done!" << endl;
    
    // Calculate L
    cout << "  Computing Avg Shortest Path Length (L)..." << flush;
    double L = calculate_avg_path_length(adj);
    cout << " Done!" << endl;
    
    cout << "\n=== Results ===" << endl;
    cout << "  Average Clustering Coefficient (C): " << fixed << setprecision(6) << C << endl;
    cout << "  Average Shortest Path Length (L): " << fixed << setprecision(6) << L << endl;
    
    // Small-world criteria
    cout << "\n=== Small-World Analysis ===" << endl;
    cout << "  High C (> 0.3): " << (C > 0.3 ? "✓ Yes" : "✗ No") << endl;
    cout << "  Low L (< log(N)): " << (L < log(N) ? "✓ Yes" : "✗ No") 
         << " [log(N) = " << log(N) << "]" << endl;
    
    if (C > 0.3 && L < log(N)) {
        cout << "  → This network exhibits SMALL-WORLD properties!" << endl;
    }
    
    cout << endl;
    return 0;
}
