#include <iostream>          // cout, cerr
#include <fstream>           // ifstream
#include <vector>            // vector
#include <unordered_map>     // adjacency lists, sigma, delta
#include <unordered_set>     // storing communities
#include <queue>             // BFS queue
#include <stack>             // dependency accumulation
#include <algorithm>         // max_element, min()
#include <map>               // EdgeBetweenness storage
#include <string>            // filename handling
      // For std::iota

// // --- Type Aliases for Readability ---
// using Node = int;
// using Graph_GN = std::unordered_map<Node, std::unordered_set<Node>>;
// using Partition_GN = std::vector<std::unordered_set<Node>>;
// // Use std::map for edge betweenness. Key is a sorted pair to represent
// // an undirected edge, ensuring (u, v) and (v, u) are treated as the same.
// using Edge_GN = std::pair<Node, Node>;
// using EdgeBetweenness_GN = std::map<Edge_GN, double>;

// // --- Helper 1: Get Connected Components ---
// [cite_start]// (Used to find communities after edge removal) [cite: 26]
// namespace GN_Helpers {
//     void _dfs_component(const Graph_GN& graph, Node node,
//                         std::unordered_set<Node>& visited,
//                         std::unordered_set<Node>& component) {
//         visited.insert(node);
//         component.insert(node);
        
//         auto it = graph.find(node);
//         if (it != graph.end()) {
//             for (Node neighbor : it->second) {
//                 if (visited.find(neighbor) == visited.end()) {
//                     _dfs_component(graph, neighbor, visited, component);
//                 }
//             }
//         }
//     }

//     Partition_GN _get_connected_components(const Graph_GN& graph) {
//         Partition_GN components;
//         std::unordered_set<Node> visited;
        
//         for (const auto& pair : graph) {
//             Node node = pair.first;
//             if (visited.find(node) == visited.end()) {
//                 std::unordered_set<Node> component;
//                 _dfs_component(graph, node, visited, component);
//                 if (!component.empty()) {
//                     components.push_back(component);
//                 }
//             }
//         }
//         return components;
//     }

//     // --- Helper 2: Modularity Calculation ---
//     [cite_start]// (Used as the stopping condition) [cite: 10, 117]
//     double get_modularity(const Graph_GN& original_graph, const Partition_GN& communities) {
//         double m = 0;
//         std::unordered_map<Node, int> degrees;
        
//         for (const auto& pair : original_graph) {
//             Node u = pair.first;
//             int degree = pair.second.size();
//             degrees[u] = degree;
//             m += degree;
//         }
//         m /= 2.0; [cite_start]// Total edges in undirected graph [cite: 231]

//         if (m == 0) return 0.0;

//         std::unordered_map<Node, int> node_community;
//         for (int i = 0; i < communities.size(); ++i) {
//             for (Node node : communities[i]) {
//                 node_community[node] = i;
//             }
//         }

//         double Q = 0.0;
//         for (const auto& pair_i : original_graph) {
//             Node i = pair_i.first;
//             for (const auto& pair_j : original_graph) {
//                 Node j = pair_j.first;

//                 [cite_start]// Check delta function (if i and j are in same community) [cite: 233]
//                 if (node_community.count(i) && node_community.count(j) &&
//                     node_community[i] == node_community[j]) {
                    
//                     [cite_start]// A_ij [cite: 228]
//                     double A_ij = (original_graph.at(i).count(j)) ? 1.0 : 0.0;
//                     [cite_start]// (k_i * k_j) / (2m) [cite: 226, 230]
//                     double expected = (degrees.at(i) * degrees.at(j)) / (2.0 * m);
//                     Q += (A_ij - expected);
//                 }
//             }
//         }
//         return Q / (2.0 * m); [cite_start]// [cite: 226]
//     }

//     // --- Helper 3: Brandes Algorithm for Edge Betweenness ---
//     [cite_start]// (Implements the "Inner Loop") [cite: 37, 107]
//     EdgeBetweenness_GN _brandes_edge_betweenness(const Graph_GN& graph) {
//         EdgeBetweenness_GN betweenness;
//         std::vector<Node> nodes;
//         for(const auto& pair : graph) {
//             nodes.push_back(pair.first);
//             for (Node neighbor : pair.second) {
//                 Edge_GN edge = {std::min(pair.first, neighbor), std::max(pair.first, neighbor)};
//                 betweenness[edge] = 0.0;
//             }
//         }

//         for (Node s : nodes) {
//             [cite_start]// --- Pass 1: BFS from s --- [cite: 44]
//             std::stack<Node> S;
//             std::unordered_map<Node, std::vector<Node>> P;
//             std::unordered_map<Node, double> sigma;
//             std::unordered_map<Node, int> dist;
            
//             for(Node w : nodes) {
//                 P[w] = {};
//                 sigma[w] = 0.0;
//                 dist[w] = -1;
//             }

//             sigma[s] = 1.0;
//             dist[s] = 0;
//             std::queue<Node> Q; [cite_start]// BFS Queue [cite: 80, 110]
//             Q.push(s);

//             while (!Q.empty()) {
//                 Node v = Q.front();
//                 Q.pop();
//                 S.push(v);

//                 if (!graph.count(v)) continue;

//                 for (Node w : graph.at(v)) {
//                     if (dist[w] < 0) {
//                         dist[w] = dist[v] + 1;
//                         Q.push(w);
//                     }
//                     if (dist[w] == dist[v] + 1) {
//                         sigma[w] += sigma[v]; [cite_start]// [cite: 46]
//                         P[w].push_back(v); [cite_start]// Store predecessors [cite: 46, 82]
//                     }
//                 }
//             }

//             [cite_start]// --- Pass 2: Accumulation --- [cite: 49]
//             std::unordered_map<Node, double> delta;
//             for(Node w : nodes) delta[w] = 0.0;

//             while (!S.empty()) {
//                 Node w = S.top();
//                 S.pop();
//                 for (Node v : P[w]) {
//                     double credit = (sigma[v] / sigma[w]) * (1.0 + delta[w]);
                    
//                     Edge_GN edge = {std::min(v, w), std::max(v, w)};
//                     betweenness[edge] += credit;
                    
//                     delta[v] += credit;
//                 }
//             }
//         }

//         // Normalize (divide by 2 for undirected)
//         for (auto& pair : betweenness) {
//             pair.second /= 2.0;
//         }
//         return betweenness;
//     }
// } // namespace GN_Helpers

// /**
//  * @brief Implements the Girvan-Newman algorithm.
//  *
//  * @param graph An adjacency list (map of node_id to set of neighbors).
//  * @return Partition_GN The partition with the highest modularity score.
//  */
// Partition_GN girvan_newman(const Graph_GN& graph) {
//     using namespace GN_Helpers;
    
//     [cite_start]// Create a mutable copy of the graph to simulate edge removal [cite: 126]
//     Graph_GN mutable_graph = graph;
    
//     // Get initial components and modularity
//     Partition_GN initial_components = _get_connected_components(mutable_graph);
//     double best_modularity = get_modularity(graph, initial_components);
//     Partition_GN best_partition = initial_components;

//     int num_edges = 0;
//     for(const auto& pair : mutable_graph) num_edges += pair.second.size();
//     num_edges /= 2;

//     [cite_start]// Outer Loop: Iteratively remove edges [cite: 30-33]
//     for (int i = 0; i < num_edges; ++i) {
        
//         [cite_start]// 1. Compute edge betweenness [cite: 24]
//         EdgeBetweenness_GN betweenness = _brandes_edge_betweenness(mutable_graph);
        
//         if (betweenness.empty()) {
//             break; // No edges left
//         }

//         [cite_start]// 2. Find and remove edge with highest betweenness [cite: 25]
//         auto max_edge_it = std::max_element(betweenness.begin(), betweenness.end(),
//             [](const auto& a, const auto& b) {
//                 return a.second < b.second;
//             });
        
//         Edge_GN edge_to_remove = max_edge_it->first;
//         Node u = edge_to_remove.first;
//         Node v = edge_to_remove.second;

//         mutable_graph[u].erase(v);
//         mutable_graph[v].erase(u);

//         [cite_start]// 3. Recompute connected components [cite: 26]
//         Partition_GN current_components = _get_connected_components(mutable_graph);

//         [cite_start]// 4. Check modularity (stopping condition) [cite: 117]
//         double current_modularity = get_modularity(graph, current_components);

//         if (current_modularity > best_modularity) {
//             best_modularity = current_modularity;
//             best_partition = current_components;
//         }
//     }

//     return best_partition; [cite_start]// Return partition with highest modularity [cite: 118]
// }

using namespace std;
// --- Type Aliases ---
using Node = int;
using Graph_GN = unordered_map<Node, unordered_set<Node>>;
using Partition_GN = vector<unordered_set<Node>>;
using Edge_GN = pair<Node, Node>;
using EdgeBetweenness_GN = map<Edge_GN, double>;

namespace GN_Helpers {

    // DFS to get connected components
    void _dfs_component(const Graph_GN& graph, Node node,
                        unordered_set<Node>& visited,
                        unordered_set<Node>& component) {
        visited.insert(node);
        component.insert(node);

        auto it = graph.find(node);
        if (it != graph.end()) {
            for (Node neighbor : it->second) {
                if (!visited.count(neighbor)) {
                    _dfs_component(graph, neighbor, visited, component);
                }
            }
        }
    }

    Partition_GN _get_connected_components(const Graph_GN& graph) {
        Partition_GN components;
        unordered_set<Node> visited;

        for (const auto& p : graph) {
            Node node = p.first;
            if (!visited.count(node)) {
                unordered_set<Node> comp;
                _dfs_component(graph, node, visited, comp);
                if (!comp.empty()) components.push_back(comp);
            }
        }
        return components;
    }

    // Brandes algorithm for edge betweenness
    EdgeBetweenness_GN _brandes_edge_betweenness(const Graph_GN& graph) {
        EdgeBetweenness_GN betweenness;
        vector<Node> nodes;

        for (const auto& p : graph) {
            nodes.push_back(p.first);
            for (Node nb : p.second) {
                Edge_GN e = {min(p.first, nb), max(p.first, nb)};
                betweenness[e] = 0.0;
            }
        }

        for (Node s : nodes) {

            stack<Node> S;
            unordered_map<Node, vector<Node>> P;
            unordered_map<Node, double> sigma;
            unordered_map<Node, int> dist;

            for (Node w : nodes) {
                P[w] = {};
                sigma[w] = 0.0;
                dist[w] = -1;
            }

            sigma[s] = 1.0;
            dist[s] = 0;
            queue<Node> Q;
            Q.push(s);

            while (!Q.empty()) {
                Node v = Q.front(); Q.pop();
                S.push(v);

                auto it = graph.find(v);
                if (it == graph.end()) continue;

                for (Node w : it->second) {
                    if (dist[w] < 0) {
                        dist[w] = dist[v] + 1;
                        Q.push(w);
                    }
                    if (dist[w] == dist[v] + 1) {
                        sigma[w] += sigma[v];
                        P[w].push_back(v);
                    }
                }
            }

            unordered_map<Node, double> delta;
            for (Node w : nodes) delta[w] = 0.0;

            while (!S.empty()) {
                Node w = S.top(); S.pop();
                for (Node v : P[w]) {
                    if (sigma[w] == 0) continue;
                    double credit = (sigma[v] / sigma[w]) * (1.0 + delta[w]);
                    Edge_GN e = {min(v, w), max(v, w)};
                    betweenness[e] += credit;
                    delta[v] += credit;
                }
            }
        }

        // undirected normalization
        for (auto &p : betweenness) p.second /= 2.0;
        return betweenness;
    }

} // namespace GN_Helpers


// ---- Girvan–Newman main algorithm: returns best partition (no modularity here) ----
Partition_GN girvan_newman(const Graph_GN& graph) {
    using namespace GN_Helpers;

    Graph_GN mutable_graph = graph;

    Partition_GN best_partition = _get_connected_components(mutable_graph);

    int num_edges = 0;
    for (const auto& p : mutable_graph) num_edges += (int)p.second.size();
    num_edges /= 2;

    for (int i = 0; i < num_edges; ++i) {
        EdgeBetweenness_GN bet = _brandes_edge_betweenness(mutable_graph);
        if (bet.empty()) break;

        auto max_it = max_element(
            bet.begin(), bet.end(),
            [](const auto& a, const auto& b) { return a.second < b.second; });

        Edge_GN rem = max_it->first;
        Node u = rem.first, v = rem.second;

        mutable_graph[u].erase(v);
        mutable_graph[v].erase(u);

        // after each removal we just update partition;
        // Python will compute modularity and decide "best"
        best_partition = _get_connected_components(mutable_graph);
    }

    return best_partition;
}

// ---- MAIN: read edges, run GN, write communities to community_output.txt ----
int main(int argc, char** argv) {
    if (argc < 2) {
        cerr << "Usage: ./girwan_newman <edge_file>\n";
        return 1;
    }

    string edge_file = argv[1];
    ifstream fin(edge_file);
    if (!fin.is_open()) {
        cerr << "Error opening file: " << edge_file << "\n";
        return 1;
    }

    Graph_GN G;
    int u, v;
    while (fin >> u >> v) {
        G[u].insert(v);
        G[v].insert(u);
    }
    fin.close();

    Partition_GN communities = girvan_newman(G);

    ofstream fout("community_output.txt");
    if (!fout.is_open()) {
        cerr << "Could not open community_output.txt for writing\n";
        return 1;
    }

    // one line per community: "u v w ..."
    for (const auto &comm : communities) {
        bool first = true;
        for (Node x : comm) {
            if (!first) fout << ' ';
            fout << x;
            first = false;
        }
        fout << '\n';
    }

    fout.close();
    return 0;
}
