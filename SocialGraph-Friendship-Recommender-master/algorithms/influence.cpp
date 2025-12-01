#include "influence.h"
#include "utils.h"
#include "pagerank.h"
#include "../LogManager.h"
#include <queue>
#include <vector>
#include <utility>
#include <unordered_map> 

using namespace std;
using ScorePair = pair<double, int>;

vector<int> recommend_by_influence(const SocialNetwork& network, int userID, int top_k) {
    
    // Clear logs and set algorithm info
    LogManager::clear();
    LogManager::setAlgorithm(
        "Influence Recommendation",
        "Combining PageRank (importance) with Jaccard Similarity (shared interests) to find influential users with similar tags.",
        userID
    );
    LogManager::log("source", userID);
    
    // --- Caclulate PageRank scores ONCE (without logging) ---
    unordered_map<int, double> pagerank_scores = calculate_pagerank(network, 0.85, 100, false);

    priority_queue<ScorePair> pq;
    const auto& myFriends = network.get_friends(userID);
    const auto& myTags = network.get_tags(userID);

    for (int candidateID : network.get_all_users()) {
        if (candidateID == userID) continue;
        if (myFriends.count(candidateID)) continue;

        // --- Calculate Score ---
        const auto& candidateTags = network.get_tags(candidateID);
        double jaccard = jaccard_similarity(myTags, candidateTags);
        
        // Log candidates being evaluated
        LogManager::log("scan", candidateID, userID);
        
        // Get the pre-calculated PageRank score
        double pagerank = pagerank_scores[candidateID]; 
        
        // Final Score: PageRank * Jaccard
        double finalScore = pagerank * jaccard;

        if (finalScore > 0) {
            // Log if there's a tag match
            if (jaccard > 0) {
                LogManager::log("visit", candidateID, userID, jaccard);
            }
            pq.push({finalScore, candidateID});
        }
    }

    // --- Format Output ---
    vector<int> recommendations;
    while (!pq.empty() && recommendations.size() < static_cast<size_t>(top_k)) {
        int recID = pq.top().second;
        recommendations.push_back(recID);
        LogManager::log("match", recID, userID, pq.top().first);
        pq.pop(); 
    }
    return recommendations;
}