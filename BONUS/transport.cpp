// Transport and Navigation Efficiency on Networks
// Analyzes routing efficiency and congestion in small-world networks

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <queue>
#include <string>
#include <random>
#include <algorithm>
#include <iomanip>
#include <map>

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
    file.ignore();
    
    Graph adj(N);
    string line;
    
    while (getline(file, line)) {
        if (line.empty()) continue;
        
        stringstream ss(line);
        int node;
        char colon;
        ss >> node >> colon;
        
        int neighbor;
        while (ss >> neighbor) {
            adj[node].push_back(neighbor);
        }
    }
    
    file.close();
    return adj;
}

// BFS to find shortest path
vector<int> find_shortest_path(const Graph& adj, int source, int target) {
    int N = adj.size();
    vector<int> parent(N, -1);
    vector<bool> visited(N, false);
    queue<int> q;
    
    q.push(source);
    visited[source] = true;
    
    while (!q.empty()) {
        int u = q.front();
        q.pop();
        
        if (u == target) break;
        
        for (int v : adj[u]) {
            if (!visited[v]) {
                visited[v] = true;
                parent[v] = u;
                q.push(v);
            }
        }
    }
    
    // Reconstruct path
    vector<int> path;
    if (parent[target] == -1 && source != target) {
        return path; // No path found
    }
    
    int current = target;
    while (current != -1) {
        path.push_back(current);
        current = parent[current];
    }
    reverse(path.begin(), path.end());
    
    return path;
}

// Calculate betweenness centrality (traffic load on each node)
vector<int> calculate_betweenness(const Graph& adj, int num_samples = 1000) {
    int N = adj.size();
    vector<int> betweenness(N, 0);
    
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> node_dist(0, N - 1);
    
    // Sample random source-target pairs
    for (int i = 0; i < num_samples; ++i) {
        int source = node_dist(gen);
        int target = node_dist(gen);
        
        if (source == target) continue;
        
        vector<int> path = find_shortest_path(adj, source, target);
        
        // Count nodes on path (excluding source and target)
        for (size_t j = 1; j < path.size() - 1; ++j) {
            betweenness[path[j]]++;
        }
    }
    
    return betweenness;
}

// Simulate packet routing with congestion
struct RoutingResult {
    double avg_delivery_time;
    double delivery_success_rate;
    int max_congestion;
    double avg_path_length;
};

RoutingResult simulate_routing(const Graph& adj, int num_packets = 500, 
                                int congestion_limit = 50) {
    int N = adj.size();
    
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> node_dist(0, N - 1);
    
    vector<int> congestion(N, 0);
    vector<int> delivery_times;
    int successful_deliveries = 0;
    int total_path_length = 0;
    
    // Generate random packets
    for (int p = 0; p < num_packets; ++p) {
        int source = node_dist(gen);
        int target = node_dist(gen);
        
        if (source == target) continue;
        
        vector<int> path = find_shortest_path(adj, source, target);
        
        if (path.empty()) continue;
        
        // Check if route is viable (no node over congestion limit)
        bool can_route = true;
        for (int node : path) {
            if (congestion[node] >= congestion_limit) {
                can_route = false;
                break;
            }
        }
        
        if (can_route) {
            // Update congestion
            for (size_t i = 1; i < path.size() - 1; ++i) {
                congestion[path[i]]++;
            }
            
            delivery_times.push_back(path.size() - 1);
            successful_deliveries++;
            total_path_length += (path.size() - 1);
        }
    }
    
    RoutingResult result;
    result.delivery_success_rate = (double)successful_deliveries / num_packets;
    result.avg_delivery_time = delivery_times.empty() ? 0.0 : 
                               (double)total_path_length / successful_deliveries;
    result.max_congestion = *max_element(congestion.begin(), congestion.end());
    result.avg_path_length = result.avg_delivery_time;
    
    return result;
}

// Analyze hub vulnerability (what happens if high-traffic nodes fail?)
void analyze_hub_vulnerability(const Graph& adj) {
    int N = adj.size();
    
    cout << "\n--- Hub Vulnerability Analysis ---" << endl;
    
    // Calculate betweenness
    vector<int> betweenness = calculate_betweenness(adj, 500);
    
    // Find top hubs
    vector<pair<int, int>> hub_ranking;
    for (int i = 0; i < N; ++i) {
        hub_ranking.push_back({betweenness[i], i});
    }
    sort(hub_ranking.rbegin(), hub_ranking.rend());
    
    cout << "\nTop 5 Critical Nodes (by betweenness centrality):" << endl;
    for (int i = 0; i < min(5, N); ++i) {
        cout << "  Node " << hub_ranking[i].second 
             << ": load = " << hub_ranking[i].first 
             << " (degree = " << adj[hub_ranking[i].second].size() << ")" << endl;
    }
    
    // Analyze distribution
    double avg_betweenness = 0.0;
    for (int b : betweenness) {
        avg_betweenness += b;
    }
    avg_betweenness /= N;
    
    int nodes_above_avg = 0;
    for (int b : betweenness) {
        if (b > avg_betweenness) {
            nodes_above_avg++;
        }
    }
    
    cout << "\nTraffic Distribution:" << endl;
    cout << "  Average load: " << fixed << setprecision(1) << avg_betweenness << endl;
    cout << "  Nodes above average: " << nodes_above_avg 
         << " (" << setprecision(1) << (double)nodes_above_avg / N * 100 << "%)" << endl;
    cout << "  Load concentration: " 
         << (nodes_above_avg < N * 0.2 ? "HIGH (vulnerable)" : "LOW (robust)") << endl;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        cerr << "Usage: " << argv[0] << " <graph_file.txt> [mode]" << endl;
        cerr << "Modes: routing (default), vulnerability" << endl;
        cerr << "Example: " << argv[0] << " small_world_analysis_data/data_proof_WS.txt routing" << endl;
        return 1;
    }
    
    string filename = argv[1];
    string mode = (argc > 2) ? argv[2] : "routing";
    
    cout << "\n=== Transport and Navigation Efficiency ===" << endl;
    cout << "Loading network from: " << filename << endl;
    
    int N;
    Graph adj = load_graph(filename, N);
    
    cout << "Network size: " << N << " nodes" << endl;
    
    if (mode == "routing") {
        cout << "\n--- Packet Routing Simulation ---" << endl;
        
        vector<int> packet_loads = {100, 300, 500, 1000};
        
        cout << "\nResults:" << endl;
        cout << "Packets | Success Rate | Avg Path Length | Max Congestion" << endl;
        cout << "--------|--------------|-----------------|---------------" << endl;
        
        for (int load : packet_loads) {
            RoutingResult result = simulate_routing(adj, load, 50);
            
            cout << "  " << setw(4) << load 
                 << "  |    " << fixed << setprecision(1) << result.delivery_success_rate * 100 << "%"
                 << "     |      " << setprecision(2) << result.avg_path_length
                 << "       |      " << result.max_congestion << endl;
        }
        
        cout << "\n=== Key Insights ===" << endl;
        cout << "• Small-world networks: EFFICIENT routing (short paths)" << endl;
        cout << "• Low L → Fast packet delivery" << endl;
        cout << "• BUT: Shortcuts create congestion at hubs" << endl;
        cout << "• Trade-off: Efficiency vs. Robustness" << endl;
        
    } else if (mode == "vulnerability") {
        analyze_hub_vulnerability(adj);
        
        cout << "\n=== Key Insights ===" << endl;
        cout << "• Small-world networks have CRITICAL HUBS" << endl;
        cout << "• High-betweenness nodes = bottlenecks" << endl;
        cout << "• Hub failure → network fragmentation" << endl;
        cout << "• Real examples: Air traffic hubs, internet routers" << endl;
    }
    
    cout << endl;
    return 0;
}
