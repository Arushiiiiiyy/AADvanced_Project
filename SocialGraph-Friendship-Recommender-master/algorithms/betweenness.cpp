#include "betweenness.h"
#include "../LogManager.h"
#include <stack>
#include <queue>
#include <vector>
#include <unordered_map>

using namespace std;

unordered_map<int, double> calculate_betweenness_scores(const SocialNetwork& network) {
    
    // Clear logs and set algorithm info (no specific target user for centrality metrics)
    LogManager::clear();
    LogManager::setAlgorithm(
        "Betweenness Centrality",
        "Finding bridge users who connect different parts of the network by measuring how often they appear on shortest paths between other users.",
        -1  // No specific target user
    );
    
    const auto& allUsers = network.get_all_users();
    unordered_map<int, double> betweenness_scores;
    
    // Initialize scores for all users to 0.0
    for (int userID : allUsers) {
        betweenness_scores[userID] = 0.0;
    }

    // --- Main Loop: Run this process from every user as a 'source' (s) ---
    for (int s : allUsers) {
        
        // --- Pass 1: Forward BFS ---
        // (This BFS counts shortest paths)
        
        stack<int> S; // Stores nodes in order of discovery for Pass 2
        
        // map: User -> List of its parents in the shortest path
        unordered_map<int, vector<int>> predecessors;
        // map: User -> Number of shortest paths from 's' to this user (use double to prevent overflow)
        unordered_map<int, double> shortest_path_count;
        // map: User -> Distance from 's'
        unordered_map<int, int> distance;
        
        queue<int> Q;

        // Initialize for the source node 's'
        shortest_path_count[s] = 1;
        distance[s] = 0;
        Q.push(s);

        while (!Q.empty()) {
            int v = Q.front();
            Q.pop();
            S.push(v); // Add to stack for Pass 2

            for (int w : network.get_friends(v)) {
                
                // Case 1: Neighbor 'w' has not been visited yet
                if (distance.find(w) == distance.end()) {
                    distance[w] = distance[v] + 1;
                    Q.push(w);
                }

                // Case 2: 'w' is on a shortest path from 's'
                if (distance[w] == distance[v] + 1) {
                    // This path (v->w) is a shortest path
                    shortest_path_count[w] += shortest_path_count[v];
                    predecessors[w].push_back(v);
                }
            }
        }

        // --- Pass 2: Backward Propagation ---
        // (This pass calculates the 'dependency' score)

        unordered_map<int, double> dependency;
        
        // Go through the stack (farthest nodes first)
        while (!S.empty()) {
            int w = S.top();
            S.pop();

            // Loop through all parents 'v' of 'w'
            for (int v : predecessors[w]) {
                // Dependency = (paths_through_v / total_paths_to_w) * (1 + dependency_of_w)
                double path_fraction = (double)shortest_path_count[v] / shortest_path_count[w];
                dependency[v] += path_fraction * (1.0 + dependency[w]);
            }

            // Add this node's dependency score to its final betweenness score
            // (Don't add the score for the source node 's' itself)
            if (w != s) {
                betweenness_scores[w] += dependency[w];
            }
        }
    }

    // --- Finalization ---
    // The algorithm counts each path twice (A->B and B->A)
    // For an undirected graph, we must divide all scores by 2.
    for (auto& [user, score] : betweenness_scores) {
        score /= 2.0;
    }
    
    // --- Log High Betweenness Users ---
    // Find max score for normalization
    double maxScore = 0.0;
    for (const auto& [user, score] : betweenness_scores) {
        if (score > maxScore) maxScore = score;
    }
    
    // Log users with significant betweenness (top 30%)
    if (maxScore > 0) {
        for (const auto& [user, score] : betweenness_scores) {
            if (score >= maxScore * 0.3) {
                LogManager::log("visit", user, -1, score);
            }
        }
    }

    return betweenness_scores;
}