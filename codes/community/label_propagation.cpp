#include <iostream>
#include <fstream>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <algorithm>    // shuffle, max_element
#include <random>       // random_device, mt19937, shuffle
#include <string>       // std::string
    // Can be used for label counting

// // Type alias for graph representation
// using Graph_LPA = std::unordered_map<int, std::unordered_set<int>>;

// /**
//  * @brief Implements the Label Propagation Algorithm (LPA).
//  *
//  * @param graph An adjacency list (map of node_id to set of neighbors).
//  * [cite_start]@param max_iterations A failsafe to prevent infinite loops. [cite: 214]
//  * @return std::vector<std::unordered_set<int>> The detected community partition.
//  */
// std::vector<std::unordered_set<int>> label_propagation(
//     const Graph_LPA& graph, int max_iterations = 100) {

//     [cite_start]// 1. Initialization: Each node gets a unique label [cite: 136]
//     std::unordered_map<int, int> labels;
//     std::vector<int> nodes;
//     for (const auto& pair : graph) {
//         int node = pair.first;
//         labels[node] = node;
//         nodes.push_back(node);
//     }

//     // Setup for random shuffling
//     std::random_device rd;
//     std::mt19937 g(rd());

//     // 2. Iteration
//     for (int i = 0; i < max_iterations; ++i) {
        
//         [cite_start]// 3. Randomized Update Order [cite: 139, 203]
//         std::shuffle(nodes.begin(), nodes.end(), g);

//         bool label_changed = false; [cite_start]// 6. Convergence check [cite: 143, 213]

//         for (int node : nodes) {
//             auto it = graph.find(node);
//             if (it == graph.end() || it->second.empty()) {
//                 continue; // Node has no neighbors
//             }

//             const std::unordered_set<int>& neighbors = it->second;

//             [cite_start]// 4. Label Adoption: Inspect neighbor labels [cite: 140]
//             [cite_start]// Use a map (or unordered_map) for efficient frequency counting [cite: 206]
//             std::unordered_map<int, int> label_counts;
//             for (int neighbor : neighbors) {
//                 label_counts[labels[neighbor]]++;
//             }

//             // Find the most frequent label(s)
//             int max_freq = 0;
//             std::vector<int> max_labels;
//             for (const auto& pair : label_counts) {
//                 if (pair.second > max_freq) {
//                     max_freq = pair.second;
//                     max_labels.clear();
//                     max_labels.push_back(pair.first);
//                 } else if (pair.second == max_freq) {
//                     max_labels.push_back(pair.first);
//                 }
//             }
            
//             if (max_labels.empty()) continue; // Should not happen if neighbors exist

//             [cite_start]// 5. Tie-Breaking: Choose randomly from tied labels [cite: 142, 211]
//             std::uniform_int_distribution<int> dist(0, max_labels.size() - 1);
//             int new_label = max_labels[dist(g)];

//             if (labels[node] != new_label) {
//                 labels[node] = new_label;
//                 label_changed = true;
//             }
//         }

//         [cite_start]// 6. Convergence: Stop if no node changed its label [cite: 143, 213]
//         if (!label_changed) {
//             break;
//         }
//     }

//     // Group nodes by final label to return the partition
//     std::unordered_map<int, std::unordered_set<int>> communities;
//     for (const auto& pair : labels) {
//         communities[pair.second].insert(pair.first);
//     }

//     std::vector<std::unordered_set<int>> final_partition;
//     for (const auto& pair : communities) {
//         final_partition.push_back(pair.second);
//     }

//     return final_partition;
// }
// #include <bits/stdc++.h>
using namespace std;

using Graph_LPA = unordered_map<int, unordered_set<int>>;
using Partition_LPA = vector<unordered_set<int>>;

// Label Propagation Algorithm
Partition_LPA label_propagation(const Graph_LPA& graph, int max_iterations = 100) {

    unordered_map<int, int> labels;
    vector<int> nodes;
    nodes.reserve(graph.size());

    for (const auto& p : graph) {
        int node = p.first;
        labels[node] = node;   // unique label initially
        nodes.push_back(node);
    }

    random_device rd;
    mt19937 gen(rd());

    for (int iter = 0; iter < max_iterations; ++iter) {

        shuffle(nodes.begin(), nodes.end(), gen);

        bool changed = false;

        for (int node : nodes) {
            auto it = graph.find(node);
            if (it == graph.end() || it->second.empty()) continue;

            const auto &nbrs = it->second;

            unordered_map<int,int> freq;
            for (int nb : nbrs) {
                freq[labels[nb]]++;
            }

            int best_label = labels[node];
            int best_count = -1;

            for (const auto &p : freq) {
                if (p.second > best_count) {
                    best_count = p.second;
                    best_label = p.first;
                }
            }

            if (labels[node] != best_label) {
                labels[node] = best_label;
                changed = true;
            }
        }

        if (!changed) break;
    }

    unordered_map<int, unordered_set<int>> comms_map;
    for (const auto &p : labels) {
        comms_map[p.second].insert(p.first);
    }

    Partition_LPA comms;
    for (const auto &p : comms_map) comms.push_back(p.second);
    return comms;
}

// MAIN: read graph, run LPA, write communities to community_output.txt
int main(int argc, char** argv) {
    if (argc < 2) {
        cerr << "Usage: ./label_propagation <edge_file>\n";
        return 1;
    }

    string edge_file = argv[1];
    ifstream fin(edge_file);
    if (!fin.is_open()) {
        cerr << "Error opening file: " << edge_file << "\n";
        return 1;
    }

    Graph_LPA G;
    int u, v;
    while (fin >> u >> v) {
        G[u].insert(v);
        G[v].insert(u);
    }
    fin.close();

    Partition_LPA communities = label_propagation(G);

    ofstream fout("community_output.txt");
    if (!fout.is_open()) {
        cerr << "Could not open community_output.txt for writing\n";
        return 1;
    }

    for (const auto &comm : communities) {
        bool first = true;
        for (int x : comm) {
            if (!first) fout << ' ';
            fout << x;
            first = false;
        }
        fout << '\n';
    }

    fout.close();
    return 0;
}
