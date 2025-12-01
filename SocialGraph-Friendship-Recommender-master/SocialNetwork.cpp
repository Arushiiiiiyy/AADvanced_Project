#include "SocialNetwork.h"
#include <fstream>   // For reading files (ifstream)
#include <sstream>   // For splitting strings (stringstream)
#include <iostream>  // For error messages (cerr)
// using namespace std;

// Define the static members
const unordered_set<int> SocialNetwork::EMPTY_FRIEND_SET = {};
const unordered_set<string> SocialNetwork::EMPTY_TAG_SET = {};

void SocialNetwork::save_data(const string& edgeFile, const string& tagFile) const {
    
    // --- 1. Save Edges ---
    ofstream edgeStream(edgeFile);
    if (!edgeStream.is_open()) {
        cerr << "  Error: Could not open " << edgeFile << " for writing." << endl;
        return;
    }

    // Use a set to avoid saving "1 2" and "2 1"
    set<pair<int, int>> savedEdges;

    for (const auto& [userA, friends] : adjList) {
        for (int userB : friends) {
            // Ensure we only save (smaller, larger) pairs
            int u = min(userA, userB);
            int v = max(userA, userB);
            
            if (savedEdges.find({u, v}) == savedEdges.end()) {
                edgeStream << u << " " << v << "\n";
                savedEdges.insert({u, v});
            }
        }
    }
    edgeStream.close();

    // --- 2. Save Tags ---
    ofstream tagStream(tagFile);
    if (!tagStream.is_open()) {
        cerr << "  Error: Could not open " << tagFile << " for writing." << endl;
        return;
    }

    // We can iterate through all users, even those with no tags,
    // but it's cleaner to just iterate the tag map.
    for (const auto& [userID, tags] : userTags) {
        if (tags.empty()) continue; // Don't save users with no tags

        tagStream << userID;
        for (const string& tag : tags) {
            tagStream << " " << tag;
        }
        tagStream << "\n";
    }
    tagStream.close();

    cout << "  Success: Network data saved to files." << endl;
}

void SocialNetwork::load_data(const string& edgeFile, const string& tagFile) {
    // Clear existing data to prevent duplication if called multiple times
    adjList.clear();
    userTags.clear();
    allUserIDs.clear();
    
    ifstream edgeStream(edgeFile);
    string line;
    int userA, userB;

    if (!edgeStream.is_open()) {
        cerr << "Error: Could not open edge file " << edgeFile << endl;
        return;
    }

    // --- Load Edges ---
    while (getline(edgeStream, line)) {
        if (line.empty() || line[0] == '#') continue; // Skip comments/empty lines

        stringstream ss(line);
        if (ss >> userA >> userB) {
            // CRITICAL: Add edge in both directions for undirected graph
            adjList[userA].insert(userB);
            adjList[userB].insert(userA);

            // Add both users to the master user list
            allUserIDs.insert(userA);
            allUserIDs.insert(userB);
        }
    }
    edgeStream.close();

    // --- Load Tags ---
    ifstream tagStream(tagFile);
    if (!tagStream.is_open()) {
        cerr << "Error: Could not open tag file " << tagFile << endl;
        return;
    }

    while (getline(tagStream, line)) {
        if (line.empty() || line[0] == '#') continue; // Skip comments/empty lines

        stringstream ss(line);
        int userID;
        string tag;
        
        if (ss >> userID) {
            // This user is now in the network, even if they have 0 friends
            allUserIDs.insert(userID);
            
            while (ss >> tag) {
                userTags[userID].insert(tag);
            }
        }
    }
    tagStream.close();
}

bool SocialNetwork::user_exists(int userID) const {
    return allUserIDs.count(userID);
}

void SocialNetwork::add_friendship(int userA, int userB) {
    // Only add if both users exist
    if (!allUserIDs.count(userA) || !allUserIDs.count(userB)) {
        cout << "  Error: One or both users do not exist." << endl;
        return;
    }
    
    adjList[userA].insert(userB);
    adjList[userB].insert(userA);
    cout << "  Success: Friendship added between " << userA << " and " << userB << "." << endl;
}

void SocialNetwork::remove_friendship(int userA, int userB) {
    // Only remove if both users exist
    if (!allUserIDs.count(userA) || !allUserIDs.count(userB)) {
        cout << "  Error: One or both users do not exist." << endl;
        return;
    }

    adjList[userA].erase(userB);
    adjList[userB].erase(userA);
    cout << "  Success: Friendship removed between " << userA << " and " << userB << "." << endl;
}

const unordered_set<int>& SocialNetwork::get_friends(int userID) const {
    // Use .find() for safe access
    auto it = adjList.find(userID);
    if (it == adjList.end()) {
        return EMPTY_FRIEND_SET; // Return safe, empty set
    }
    return it->second; // Return reference to the real friend list
}

const unordered_set<string>& SocialNetwork::get_tags(int userID) const {
    // Use .find() for safe access
    auto it = userTags.find(userID);
    if (it == userTags.end()) {
        return EMPTY_TAG_SET; // Return safe, empty set
    }
    return it->second; // Return reference to the real tag list
}

const unordered_set<int>& SocialNetwork::get_all_users() const {
    return allUserIDs;
}

int SocialNetwork::get_degree(int userID) const {
    // This is now safe because get_friends handles non-existent users
    return get_friends(userID).size();
}

void SocialNetwork::add_user(int userID, const unordered_set<string>& tags) {
    // 1. Add to the master list
    allUserIDs.insert(userID);

    // 2. Add/Overwrite their tags
    userTags[userID] = tags;

    // 3. Ensure they have an (empty) entry in the friend list
    //    This makes sure get_friends(userID) works correctly.
    if (adjList.find(userID) == adjList.end()) {
        adjList[userID] = {}; // Create an empty friend set for them
    }
}