#include "bfs.h"
#include "../LogManager.h" // <--- Include the logger
#include <queue>
#include <unordered_map>
#include <vector>
#include <unordered_set>
#include <algorithm>

using namespace std;

vector<int> get_shortest_path(const SocialNetwork& network, int startUser, int endUser) {
    // 1. CLEAR previous logs before starting and set algorithm info
    LogManager::clear();
    LogManager::setAlgorithm(
        "Shortest Path (BFS)",
        "Finding shortest path using Breadth-First Search. Explores the network level by level to find the minimum number of hops between users.",
        startUser
    );
    
    vector<int> path;
    if (startUser == endUser) return {startUser};
    
    queue<int> q;
    unordered_set<int> visited;
    unordered_map<int, int> parent_map;

    q.push(startUser);
    visited.insert(startUser);
    parent_map[startUser] = -1;

    // LOG: Starting the search
    LogManager::log("visit", startUser);

    bool found = false;

    while (!q.empty()) {
        int currentUser = q.front();
        q.pop();

        // LOG: We are now processing this node (the "ripple" expands from here)
        LogManager::log("visit", currentUser);

        if (currentUser == endUser) {
            found = true;
            break;
        }

        for (int neighbor : network.get_friends(currentUser)) {
            if (visited.find(neighbor) == visited.end()) {
                visited.insert(neighbor);
                parent_map[neighbor] = currentUser;
                q.push(neighbor);
                
                // LOG: We found a new neighbor to look at later
                LogManager::log("scan", neighbor);
            }
        }
    }

    if (found) {
        int current = endUser;
        while (current != -1) {
            path.push_back(current);
            // LOG: Record the final path nodes so we can color them Gold later
            LogManager::log("path", current);
            current = parent_map[current];
        }
        reverse(path.begin(), path.end());
    }
    
    return path;
}