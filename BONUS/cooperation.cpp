// Cooperation and Game Theory on Networks
// Models evolution of cooperation using Prisoner's Dilemma

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <random>
#include <algorithm>
#include <iomanip>

using namespace std;
using Graph = vector<vector<int>>;

enum Strategy { COOPERATE, DEFECT };

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

// Payoff matrix for Prisoner's Dilemma
// Format: payoff[my_strategy][opponent_strategy]
double payoff_matrix[2][2] = {
    {3.0, 0.0},  // I cooperate: (C,C)=3, (C,D)=0
    {5.0, 1.0}   // I defect:    (D,C)=5, (D,D)=1
};

// Calculate payoff for a player
double calculate_payoff(Strategy my_strategy, const vector<Strategy>& strategies, 
                       const vector<int>& neighbors) {
    double total_payoff = 0.0;
    
    for (int neighbor : neighbors) {
        Strategy opponent_strategy = strategies[neighbor];
        total_payoff += payoff_matrix[my_strategy][opponent_strategy];
    }
    
    return total_payoff;
}

// Simulation Result
struct CooperationResult {
    vector<double> cooperation_rate;
    double final_cooperation_rate;
    double avg_payoff;
};

// Evolutionary Game: Players imitate better-performing neighbors
CooperationResult simulate_cooperation(const Graph& adj, double initial_coop_prob, 
                                       int num_generations = 100) {
    int N = adj.size();
    vector<Strategy> strategies(N);
    
    random_device rd;
    mt19937 gen(rd());
    uniform_real_distribution<> prob_dist(0.0, 1.0);
    
    // Initialize strategies randomly
    for (int i = 0; i < N; ++i) {
        strategies[i] = (prob_dist(gen) < initial_coop_prob) ? COOPERATE : DEFECT;
    }
    
    CooperationResult result;
    
    // Run generations
    for (int gen_num = 0; gen_num < num_generations; ++gen_num) {
        // Calculate payoffs for all players
        vector<double> payoffs(N);
        for (int i = 0; i < N; ++i) {
            payoffs[i] = calculate_payoff(strategies[i], strategies, adj[i]);
        }
        
        // Count cooperators
        int cooperators = count(strategies.begin(), strategies.end(), COOPERATE);
        double coop_rate = (double)cooperators / N;
        result.cooperation_rate.push_back(coop_rate);
        
        // Update strategies: each player considers switching to best neighbor's strategy
        vector<Strategy> new_strategies = strategies;
        
        for (int i = 0; i < N; ++i) {
            double my_payoff = payoffs[i];
            int best_neighbor = i;
            double best_payoff = my_payoff;
            
            // Find best-performing neighbor
            for (int neighbor : adj[i]) {
                if (payoffs[neighbor] > best_payoff) {
                    best_payoff = payoffs[neighbor];
                    best_neighbor = neighbor;
                }
            }
            
            // Imitate best neighbor with probability proportional to payoff difference
            if (best_neighbor != i) {
                double payoff_diff = best_payoff - my_payoff;
                double switch_prob = payoff_diff / 10.0;  // Normalization factor
                switch_prob = min(1.0, max(0.0, switch_prob));
                
                if (prob_dist(gen) < switch_prob) {
                    new_strategies[i] = strategies[best_neighbor];
                }
            }
        }
        
        strategies = new_strategies;
    }
    
    // Final statistics
    int final_cooperators = count(strategies.begin(), strategies.end(), COOPERATE);
    result.final_cooperation_rate = (double)final_cooperators / N;
    
    // Calculate average payoff
    double total_payoff = 0.0;
    for (int i = 0; i < N; ++i) {
        total_payoff += calculate_payoff(strategies[i], strategies, adj[i]);
    }
    result.avg_payoff = total_payoff / N;
    
    return result;
}

// Cluster-based cooperation analysis
void analyze_cooperation_clusters(const Graph& adj, const vector<Strategy>& strategies) {
    int N = adj.size();
    
    // Count cooperating neighbors for each node
    vector<double> local_coop_rate(N);
    
    for (int i = 0; i < N; ++i) {
        if (adj[i].empty()) {
            local_coop_rate[i] = 0.0;
            continue;
        }
        
        int coop_neighbors = 0;
        for (int neighbor : adj[i]) {
            if (strategies[neighbor] == COOPERATE) {
                coop_neighbors++;
            }
        }
        local_coop_rate[i] = (double)coop_neighbors / adj[i].size();
    }
    
    // Calculate statistics
    double avg_local_coop = 0.0;
    for (double rate : local_coop_rate) {
        avg_local_coop += rate;
    }
    avg_local_coop /= N;
    
    cout << "  Average local cooperation rate: " << fixed << setprecision(3) 
         << avg_local_coop * 100 << "%" << endl;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        cerr << "Usage: " << argv[0] << " <graph_file.txt> [initial_coop_prob] [generations]" << endl;
        cerr << "Example: " << argv[0] << " small_world_analysis_data/data_proof_WS.txt 0.5 100" << endl;
        return 1;
    }
    
    string filename = argv[1];
    double initial_coop_prob = (argc > 2) ? atof(argv[2]) : 0.5;
    int num_generations = (argc > 3) ? atoi(argv[3]) : 100;
    
    cout << "\n=== Cooperation Evolution (Prisoner's Dilemma) ===" << endl;
    cout << "Loading network from: " << filename << endl;
    
    int N;
    Graph adj = load_graph(filename, N);
    
    cout << "Network size: " << N << " nodes" << endl;
    
    cout << "\nPayoff Matrix (Prisoner's Dilemma):" << endl;
    cout << "           Cooperate  Defect" << endl;
    cout << "Cooperate     3         0    " << endl;
    cout << "Defect        5         1    " << endl;
    
    cout << "\nSimulation Parameters:" << endl;
    cout << "  Initial cooperation probability: " << fixed << setprecision(2) 
         << initial_coop_prob * 100 << "%" << endl;
    cout << "  Generations: " << num_generations << endl;
    cout << "  Update rule: Imitate best-performing neighbor" << endl;
    
    cout << "\nRunning simulation..." << endl;
    
    // Run multiple simulations
    int num_simulations = 10;
    double total_final_coop = 0.0;
    double total_avg_payoff = 0.0;
    
    vector<vector<double>> all_trajectories;
    
    for (int sim = 0; sim < num_simulations; ++sim) {
        CooperationResult result = simulate_cooperation(adj, initial_coop_prob, num_generations);
        
        total_final_coop += result.final_cooperation_rate;
        total_avg_payoff += result.avg_payoff;
        
        all_trajectories.push_back(result.cooperation_rate);
        
        cout << "  Simulation " << (sim + 1) << ": "
             << "Final cooperation = " << setprecision(1) 
             << result.final_cooperation_rate * 100 << "%, "
             << "Avg payoff = " << setprecision(2) << result.avg_payoff << endl;
    }
    
    total_final_coop /= num_simulations;
    total_avg_payoff /= num_simulations;
    
    cout << "\n=== Results (Averaged over " << num_simulations << " simulations) ===" << endl;
    cout << "  Initial cooperation rate: " << setprecision(1) << initial_coop_prob * 100 << "%" << endl;
    cout << "  Final cooperation rate: " << setprecision(1) << total_final_coop * 100 << "%" << endl;
    cout << "  Average payoff: " << setprecision(2) << total_avg_payoff << endl;
    
    // Show trajectory samples
    cout << "\nCooperation Rate Evolution (sample trajectory):" << endl;
    const vector<double>& sample = all_trajectories[0];
    cout << "  Gen 0:   " << setprecision(1) << sample[0] * 100 << "%" << endl;
    cout << "  Gen 25:  " << sample[min(25, (int)sample.size()-1)] * 100 << "%" << endl;
    cout << "  Gen 50:  " << sample[min(50, (int)sample.size()-1)] * 100 << "%" << endl;
    cout << "  Gen 75:  " << sample[min(75, (int)sample.size()-1)] * 100 << "%" << endl;
    cout << "  Gen 99:  " << sample[sample.size()-1] * 100 << "%" << endl;
    
    cout << "\n=== Key Insights ===" << endl;
    cout << "• Small-world networks support cooperation better than random networks" << endl;
    cout << "• High clustering (high C) → cooperators form protective clusters" << endl;
    cout << "• Shortcuts (low L) → successful strategies spread quickly" << endl;
    cout << "• Network structure affects evolutionary outcomes!" << endl;
    
    cout << endl;
    return 0;
}
