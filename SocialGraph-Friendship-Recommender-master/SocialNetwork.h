#pragma once // Prevents this file from being included multiple times
using namespace std;
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <set>       // For std::set
#include <utility>   // For std::pair
#include <algorithm> // For std::min and std::max

class SocialNetwork {
public:
    // --- Public Interface ---

    
    //Loads graph edges and user tags from data files.
    //edgeFile Path to the edge list file (e.g., "1 2").
    //tagFile Path to the tags file (e.g., "1 tagA tagB").
     
    void load_data(const string& edgeFile, const string& tagFile);

    
    //Saves the current network state (edges and tags) back to the files.
     
    void save_data(const string& edgeFile, const string& tagFile) const;

    
    //Adds an undirected friendship edge between two users.
     
    void add_friendship(int userA, int userB);
    
    
    //Removes an undirected friendship edge between two users.
     
    void remove_friendship(int userA, int userB);

    
    //Adds a new user to the network with their tags.
     // If user ID already exists, this will overwrite their tags.
    //userID The new user's ID.
    //tags A set of strings representing the user's tags.
     
    void add_user(int userID, const unordered_set<string>& tags);

    
    //Checks if a user ID exists in the network.
    //userID The user's ID.
     //true if the user exists, false otherwise.
     
    bool user_exists(int userID) const;

    
    //Gets a user's friend list by constant reference.
    //userID The user's ID.
     //A const reference to the user's friend set.
    // Returns a static empty set if user does not exist.
     
    const unordered_set<int>& get_friends(int userID) const;

    
    //Gets a user's tag list by constant reference.
    //userID The user's ID.
     //A const reference to the user's tag set.
    // Returns a static empty set if user does not exist.
     
    const unordered_set<string>& get_tags(int userID) const;

    
    //Gets a list of all users in the network.
     //A const reference to the set of all user IDs.
     
    const unordered_set<int>& get_all_users() const;

    
    //Gets the degree (friend count) of a user.
    //userID The user's ID.
     //The number of friends, or 0 if user does not exist.
     
    int get_degree(int userID) const;

private:
    // --- Private Data Members ---
    
    // Using unordered_set for O(1) average lookup
    unordered_map<int, unordered_set<int>> adjList;
    unordered_map<int, unordered_set<string>> userTags;

    // Master list of all users
    unordered_set<int> allUserIDs;

    // Static empty sets to return for non-existent users (safe getter)
    static const unordered_set<int> EMPTY_FRIEND_SET;
    static const unordered_set<string> EMPTY_TAG_SET;
};