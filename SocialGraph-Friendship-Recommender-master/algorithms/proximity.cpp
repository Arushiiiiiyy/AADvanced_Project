#include "proximity.h"
#include "../LogManager.h" 
#include <unordered_map>
#include <queue>
#include <utility>
#include <cmath> 

using namespace std;

using ScorePair = pair<double, int>;

vector<int> recommend_by_proximity(const SocialNetwork& network, int userID, int top_k) {
    // 1. CLEAR previous logs and set algorithm info
    LogManager::clear();
    LogManager::setAlgorithm(
        "Proximity Recommendation",
        "Using Adamic-Adar to find friends-of-friends weighted by mutual connections. Higher scores indicate stronger social proximity.",
        userID
    );
    LogManager::log("source", userID); // Special log for the starting user

    unordered_map<int, double> adamicAdarScores;
    const auto& myFriends = network.get_friends(userID);

    // --- Core Algorithm ---
    for (int friendID : myFriends) {
        // LOG: We are looking at a direct friend (The "Bridge")
        LogManager::log("visit", friendID, userID); 

        for (int fofID : network.get_friends(friendID)) {
            if (fofID == userID) continue;
            if (myFriends.count(fofID)) continue;

            // LOG: We found a "Friend of a Friend" (A Candidate)
            LogManager::log("scan", fofID, friendID);

            int commonFriendDegree = network.get_degree(friendID);
            if (commonFriendDegree > 1) {
                adamicAdarScores[fofID] += 1.0 / log(commonFriendDegree);
            }
        }
    }

    // --- Get Top K Results ---
    priority_queue<ScorePair> pq;
    for (auto const& [candidateID, score] : adamicAdarScores) {
        pq.push({score, candidateID});
    }

    vector<int> recommendations;
    while (!pq.empty() && recommendations.size() < static_cast<size_t>(top_k)) {
        // DEFINE 'rec' HERE so we can use it for logging
        int rec = pq.top().second;
        double score = pq.top().first;
        
        recommendations.push_back(rec);
        
        // LOG: This is a final recommendation with score!
        LogManager::log("match", rec, -1, score); 
        
        pq.pop();
    }

    return recommendations;
}