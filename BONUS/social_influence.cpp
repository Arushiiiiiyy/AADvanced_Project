// Social Influence and Information Diffusion Simulation
// Models how ideas, trends, and behaviors spread through networks

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <random>
#include <algorithm>
#include <iomanip>
#include <cmath>

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

// Threshold Model: node adopts if fraction of neighbors adopted >= threshold
struct DiffusionResult {
    int total_adopters;
    int cascade_size;
    vector<int> adopters_per_step;
    bool global_cascade;
};

DiffusionResult simulate_threshold_model(const Graph& adj, double threshold, 
                                          int initial_adopters = 1) {
    int N = adj.size();
    vector<bool> adopted(N, false);
    
    // Setup random number generation
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> node_dist(0, N - 1);
    
    // Seed initial adopters
    for (int i = 0; i < initial_adopters; ++i) {
        int seed = node_dist(gen);
        adopted[seed] = true;
    }
    
    DiffusionResult result;
    result.total_adopters = initial_adopters;
    result.cascade_size = 0;
    
    bool changed = true;
    int step = 0;
    
    while (changed && step < 100) {
        changed = false;
        result.adopters_per_step.push_back(result.total_adopters);
        
        vector<bool> next_adopted = adopted;
        
        for (int i = 0; i < N; ++i) {
            if (!adopted[i] && !adj[i].empty()) {
                // Count adopted neighbors
                int adopted_neighbors = 0;
                for (int neighbor : adj[i]) {
                    if (adopted[neighbor]) {
                        adopted_neighbors++;
                    }
                }
                
                // Check if threshold is met
                double fraction = (double)adopted_neighbors / adj[i].size();
                if (fraction >= threshold) {
                    next_adopted[i] = true;
                    changed = true;
                    result.total_adopters++;
                    result.cascade_size++;
                }
            }
        }
        
        adopted = next_adopted;
        step++;
    }
    
    // Global cascade if > 25% of network adopted
    result.global_cascade = (result.total_adopters > N * 0.25);
    
    return result;
}

// Linear Threshold with weighted influence
DiffusionResult simulate_viral_marketing(const Graph& adj, int seed_count = 5) {
    int N = adj.size();
    vector<double> influence(N, 0.0);  // Cumulative influence on each node
    vector<bool> adopted(N, false);
    
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> node_dist(0, N - 1);
    uniform_real_distribution<> threshold_dist(0.1, 0.5);  // Individual thresholds
    
    // Assign random thresholds to each person
    vector<double> thresholds(N);
    for (int i = 0; i < N; ++i) {
        thresholds[i] = threshold_dist(gen);
    }
    
    // Seed high-degree influencers (hubs)
    vector<pair<int, int>> degree_nodes;
    for (int i = 0; i < N; ++i) {
        degree_nodes.push_back({adj[i].size(), i});
    }
    sort(degree_nodes.rbegin(), degree_nodes.rend());
    
    DiffusionResult result;
    result.total_adopters = 0;
    
    for (int i = 0; i < min(seed_count, N); ++i) {
        int seed = degree_nodes[i].second;
        adopted[seed] = true;
        result.total_adopters++;
    }
    
    bool changed = true;
    int step = 0;
    
    while (changed && step < 100) {
        changed = false;
        result.adopters_per_step.push_back(result.total_adopters);
        
        // Calculate influence from adopted neighbors
        for (int i = 0; i < N; ++i) {
            if (!adopted[i]) {
                influence[i] = 0.0;
                int adopted_neighbors = 0;
                
                for (int neighbor : adj[i]) {
                    if (adopted[neighbor]) {
                        adopted_neighbors++;
                    }
                }
                
                if (!adj[i].empty()) {
                    influence[i] = (double)adopted_neighbors / adj[i].size();
                }
            }
        }
        
        // Check if influence exceeds threshold
        for (int i = 0; i < N; ++i) {
            if (!adopted[i] && influence[i] >= thresholds[i]) {
                adopted[i] = true;
                changed = true;
                result.total_adopters++;
            }
        }
        
        step++;
    }
    
    result.cascade_size = result.total_adopters - seed_count;
    result.global_cascade = (result.total_adopters > N * 0.25);
    
    return result;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        cerr << "Usage: " << argv[0] << " <graph_file.txt> [mode]" << endl;
        cerr << "Modes: threshold (default), viral" << endl;
        cerr << "Example: " << argv[0] << " small_world_analysis_data/data_proof_WS.txt threshold" << endl;
        return 1;
    }
    
    string filename = argv[1];
    string mode = (argc > 2) ? argv[2] : "threshold";
    
    cout << "\n=== Social Influence & Information Diffusion ===" << endl;
    cout << "Loading network from: " << filename << endl;
    
    int N;
    Graph adj = load_graph(filename, N);
    
    cout << "Network size: " << N << " nodes" << endl;
    
    if (mode == "threshold") {
        cout << "\n--- Threshold Model Simulation ---" << endl;
        cout << "Model: Node adopts if ≥ threshold fraction of neighbors adopted" << endl;
        
        vector<double> thresholds = {0.1, 0.2, 0.3, 0.4, 0.5};
        
        cout << "\nResults:" << endl;
        cout << "Threshold | Total Adopters | Cascade Size | Global Cascade?" << endl;
        cout << "----------|----------------|--------------|----------------" << endl;
        
        for (double thresh : thresholds) {
            int total_adopters = 0;
            int num_simulations = 10;
            
            for (int sim = 0; sim < num_simulations; ++sim) {
                DiffusionResult result = simulate_threshold_model(adj, thresh, 3);
                total_adopters += result.total_adopters;
            }
            
            double avg_adopters = (double)total_adopters / num_simulations;
            double penetration = avg_adopters / N * 100;
            bool cascade = (penetration > 25);
            
            cout << "   " << fixed << setprecision(1) << thresh 
                 << "    |      " << (int)avg_adopters 
                 << " (" << setprecision(1) << penetration << "%)  |     " 
                 << (int)(avg_adopters - 3) << "      |      "
                 << (cascade ? "YES ✓" : "NO ✗") << endl;
        }
        
        cout << "\n=== Key Insights ===" << endl;
        cout << "• Lower thresholds → easier adoption → larger cascades" << endl;
        cout << "• Small-world shortcuts enable rapid spread across network" << endl;
        cout << "• High clustering creates locally reinforcing neighborhoods" << endl;
        
    } else if (mode == "viral") {
        cout << "\n--- Viral Marketing Simulation ---" << endl;
        cout << "Strategy: Seed influencers (high-degree nodes)" << endl;
        
        vector<int> seed_counts = {1, 3, 5, 10, 20};
        
        cout << "\nResults:" << endl;
        cout << "Seeds | Total Reach | Penetration | ROI (reach/seed)" << endl;
        cout << "------|-------------|-------------|-------------------" << endl;
        
        for (int seeds : seed_counts) {
            if (seeds > N) continue;
            
            int total_reach = 0;
            int num_simulations = 5;
            
            for (int sim = 0; sim < num_simulations; ++sim) {
                DiffusionResult result = simulate_viral_marketing(adj, seeds);
                total_reach += result.total_adopters;
            }
            
            double avg_reach = (double)total_reach / num_simulations;
            double penetration = avg_reach / N * 100;
            double roi = avg_reach / seeds;
            
            cout << "  " << setw(3) << seeds 
                 << " |     " << setw(5) << (int)avg_reach 
                 << " (" << fixed << setprecision(1) << penetration << "%) |   "
                 << setprecision(1) << penetration << "%     |    "
                 << setprecision(1) << roi << "x" << endl;
        }
        
        cout << "\n=== Key Insights ===" << endl;
        cout << "• Targeting hubs (high-degree nodes) maximizes reach" << endl;
        cout << "• Small-world networks: efficient for viral campaigns" << endl;
        cout << "• Few seeds can trigger large cascades via shortcuts" << endl;
    }
    
    cout << endl;
    return 0;
}
