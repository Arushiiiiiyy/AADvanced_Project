// Disease Spread Simulation on Networks (SIR Model)
// Demonstrates how small-world networks affect epidemic spreading

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <queue>
#include <string>
#include <random>
#include <algorithm>
#include <iomanip>

using namespace std;
using Graph = vector<vector<int>>;

enum State { SUSCEPTIBLE, INFECTED, RECOVERED };

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

// SIR Model Simulation
struct SimulationResult {
    int peak_infected;
    int total_infected;
    int time_to_peak;
    int duration;
    vector<int> infected_per_step;
};

SimulationResult simulate_sir(const Graph& adj, double infection_prob, 
                               double recovery_prob, int initial_infected = 1) {
    int N = adj.size();
    vector<State> state(N, SUSCEPTIBLE);
    
    // Setup random number generation
    random_device rd;
    mt19937 gen(rd());
    uniform_real_distribution<> dis(0.0, 1.0);
    uniform_int_distribution<> node_dist(0, N - 1);
    
    // Randomly infect initial nodes
    for (int i = 0; i < initial_infected; ++i) {
        int patient_zero = node_dist(gen);
        state[patient_zero] = INFECTED;
    }
    
    SimulationResult result;
    result.peak_infected = initial_infected;
    result.total_infected = initial_infected;
    result.time_to_peak = 0;
    result.duration = 0;
    
    int step = 0;
    int current_infected = initial_infected;
    
    // Simulation loop
    while (current_infected > 0) {
        result.infected_per_step.push_back(current_infected);
        
        vector<State> next_state = state;
        
        // Process infections
        for (int i = 0; i < N; ++i) {
            if (state[i] == INFECTED) {
                // Try to infect neighbors
                for (int neighbor : adj[i]) {
                    if (state[neighbor] == SUSCEPTIBLE && dis(gen) < infection_prob) {
                        next_state[neighbor] = INFECTED;
                        result.total_infected++;
                    }
                }
                
                // Try to recover
                if (dis(gen) < recovery_prob) {
                    next_state[i] = RECOVERED;
                }
            }
        }
        
        state = next_state;
        
        // Count current infected
        current_infected = count(state.begin(), state.end(), INFECTED);
        
        if (current_infected > result.peak_infected) {
            result.peak_infected = current_infected;
            result.time_to_peak = step;
        }
        
        step++;
        if (step > 1000) break; // Safety limit
    }
    
    result.duration = step;
    return result;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        cerr << "Usage: " << argv[0] << " <graph_file.txt> [infection_prob] [recovery_prob] [num_simulations]" << endl;
        cerr << "Example: " << argv[0] << " small_world_analysis_data/data_proof_WS.txt 0.3 0.1 10" << endl;
        return 1;
    }
    
    string filename = argv[1];
    double infection_prob = (argc > 2) ? atof(argv[2]) : 0.3;
    double recovery_prob = (argc > 3) ? atof(argv[3]) : 0.1;
    int num_simulations = (argc > 4) ? atoi(argv[4]) : 10;
    
    cout << "\n=== Disease Spread Simulation (SIR Model) ===" << endl;
    cout << "Loading network from: " << filename << endl;
    
    int N;
    Graph adj = load_graph(filename, N);
    
    cout << "Network size: " << N << " nodes" << endl;
    cout << "\nSimulation Parameters:" << endl;
    cout << "  Infection probability: " << infection_prob << endl;
    cout << "  Recovery probability: " << recovery_prob << endl;
    cout << "  Number of simulations: " << num_simulations << endl;
    cout << "  Initial infected: 1 (random patient zero)" << endl;
    
    cout << "\nRunning simulations..." << endl;
    
    // Run multiple simulations
    double avg_peak = 0, avg_total = 0, avg_time = 0, avg_duration = 0;
    
    for (int sim = 0; sim < num_simulations; ++sim) {
        SimulationResult result = simulate_sir(adj, infection_prob, recovery_prob, 1);
        
        avg_peak += result.peak_infected;
        avg_total += result.total_infected;
        avg_time += result.time_to_peak;
        avg_duration += result.duration;
        
        cout << "  Simulation " << (sim + 1) << ": " 
             << result.total_infected << " total infected, "
             << "peak=" << result.peak_infected << ", "
             << "duration=" << result.duration << " steps" << endl;
    }
    
    avg_peak /= num_simulations;
    avg_total /= num_simulations;
    avg_time /= num_simulations;
    avg_duration /= num_simulations;
    
    cout << "\n=== Results (Averaged over " << num_simulations << " simulations) ===" << endl;
    cout << "  Total infected (% of population): " << fixed << setprecision(2) 
         << (avg_total / N * 100) << "%" << endl;
    cout << "  Peak infected: " << (int)avg_peak << " nodes" << endl;
    cout << "  Time to peak: " << (int)avg_time << " steps" << endl;
    cout << "  Epidemic duration: " << (int)avg_duration << " steps" << endl;
    
    cout << "\n=== Key Insight ===" << endl;
    cout << "Small-world networks facilitate RAPID disease spread due to:" << endl;
    cout << "  1. Short paths (low L) → fast transmission across network" << endl;
    cout << "  2. High clustering (high C) → local outbreak amplification" << endl;
    cout << "  3. Long-range shortcuts → bridges between communities" << endl;
    
    cout << endl;
    return 0;
}
