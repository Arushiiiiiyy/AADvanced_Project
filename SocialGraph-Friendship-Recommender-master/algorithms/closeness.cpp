#include "closeness.h"
#include "../LogManager.h"
#include <queue>
#include <limits>
#include <cmath>

using namespace std;

unordered_map<int, double> calculate_closeness_scores(const SocialNetwork& network) {
    
    // Clear logs and set algorithm info
    LogManager::clear();
    LogManager::setAlgorithm(
        "Closeness Centrality",
        "Finding well-connected users who can reach everyone in the network quickly by measuring average shortest path distances.",
        -1  // No specific target user
    );
    
    const auto& allUsers = network.get_all_users();
    int N = allUsers.size();
    if (N <= 1) return {};

    unordered_map<int, double> scores;

    // --- Loop 1: Run BFS from every single user (All-Pairs-Shortest-Path) ---
    for (int startUser : allUsers) {
        
        // 1. Setup BFS for distance calculation
        queue<int> q;
        // map<UserID, DistanceFromStartNode>
        unordered_map<int, int> distance; 
        
        q.push(startUser);
        distance[startUser] = 0;
        
        int sum_of_distances = 0;
        int reachable_nodes = 0;

        // 2. Run the BFS
        while (!q.empty()) {
            int currentUser = q.front();
            q.pop();

            // Add this node's distance to the total sum
            sum_of_distances += distance[currentUser];
            
            // Check neighbors
            for (int neighbor : network.get_friends(currentUser)) {
                // If distance map does not contain the neighbor, it's unvisited
                if (distance.find(neighbor) == distance.end()) {
                    distance[neighbor] = distance[currentUser] + 1;
                    q.push(neighbor);
                    reachable_nodes++;
                }
            }
        }
        
        // --- 3. Calculate Closeness Score (Handle disconnected components) ---
        // Use the actual number of reachable nodes, not the total network size
        // This prevents isolated users from getting artificially high scores
        if (sum_of_distances > 0 && reachable_nodes > 0) {
            // Closeness = (reachable_nodes) / sum of distances to reachable nodes
            // Normalized by total network size to penalize disconnected components
            double rawCloseness = (double)reachable_nodes / sum_of_distances;
            // Apply penalty for not reaching all nodes
            double connectivityRatio = (double)reachable_nodes / (N - 1);
            scores[startUser] = rawCloseness * connectivityRatio;
        } else {
            // Isolated node or error
            scores[startUser] = 0.0;
        }
    }
    
    // --- Log High Closeness Users ---
    // Find max score for normalization
    double maxScore = 0.0;
    for (const auto& [user, score] : scores) {
        if (score > maxScore) maxScore = score;
    }
    
    // Log users with high closeness (top 30%)
    if (maxScore > 0) {
        for (const auto& [user, score] : scores) {
            if (score >= maxScore * 0.3) {
                LogManager::log("visit", user, -1, score);
            }
        }
    }
    
    return scores;
}