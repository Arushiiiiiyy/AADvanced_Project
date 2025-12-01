#include "pagerank.h"
#include "../LogManager.h"
#include <vector>
#include <cmath>
#include <unordered_map>

using namespace std;

unordered_map<int, double> calculate_pagerank(
    const SocialNetwork& network,
    double dampingFactor,
    int iterations,
    bool log_results) {

    // Clear logs and set algorithm info only if logging is enabled
    if (log_results) {
        LogManager::clear();
        LogManager::setAlgorithm(
            "PageRank",
            "Measuring user importance by analyzing the network structure - users connected to many important users get higher scores.",
            -1  // No specific target user
        );
    }

    const auto& allUsers = network.get_all_users();
    int N = allUsers.size();
    if (N == 0) {
        return {}; // Handle empty graph
    }

    // Initialize scores: everyone starts with 1/N
    unordered_map<int, double> scores;
    for (int userID : allUsers) {
        scores[userID] = 1.0 / N;
    }

    // --- Power Iteration Loop ---
    for (int i = 0; i < iterations; ++i) {
        unordered_map<int, double> newScores; // Scores for this iteration
        double total_S_contribution = 0.0; // For handling "sink" nodes (no friends)

        // 1. Calculate contributions from all nodes
        for (int userID : allUsers) {
            int degree = network.get_degree(userID);
            
            if (degree == 0) {
                // This is a "sink" node. It contributes its score to everyone.
                total_S_contribution += scores[userID];
            } else {
                // This node gives its score to all its friends
                double contribution = scores[userID] / degree;
                for (int friendID : network.get_friends(userID)) {
                    newScores[friendID] += contribution;
                }
            }
        }
        
        // 2. Combine scores with the "damping factor"
        for (int userID : allUsers) {
            // (1-d)/N is the "random jump" probability
            // d * (newScores[userID] + ... ) is the "follow a link" probability
            newScores[userID] = (1.0 - dampingFactor) / N + dampingFactor * (newScores[userID] + total_S_contribution / N);
        }

        // 3. Update scores for the next iteration
        scores = newScores;
    }
    
    // --- Log High PageRank Users (only if logging enabled) ---
    if (log_results) {
        // Find max score for normalization
        double maxScore = 0.0;
        for (const auto& [user, score] : scores) {
            if (score > maxScore) maxScore = score;
        }
        
        // Log users with high PageRank (top 30%)
        if (maxScore > 0) {
            for (const auto& [user, score] : scores) {
                if (score >= maxScore * 0.3) {
                    LogManager::log("visit", user, -1, score);
                }
            }
        }
    }

    return scores;
}