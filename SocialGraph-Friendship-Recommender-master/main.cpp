#include <iostream>
#include <string>
#include <vector>
#include <limits>
#include <unordered_set>
#include <map> 
#include <utility>
#include <algorithm>
#include "SocialNetwork.h"
#include "algorithms/influence.h"
#include "algorithms/proximity.h"
#include "algorithms/hybrid.h"
#include "algorithms/bfs.h"
#include "algorithms/communities.h" 
#include "algorithms/closeness.h" 
#include "algorithms/betweenness.h"
#include "WebExporter.h"

using namespace std;

// Forward declaration
int get_int_input(const string& prompt);


//Helper function to print a vector of recommendations.
 
template<typename T>
void print_recommendations(const vector<T>& recs) {
    if (recs.empty()) {
        cout << "  No recommendations found." << endl;
        return;
    }
    cout << "  [ ";
    for (size_t i = 0; i < recs.size(); ++i) {
        cout << recs[i] << (i == recs.size() - 1 ? " " : ", ");
    }
    cout << "]" << endl;
}


//Safely gets an integer from the user.
 
int get_int_input(const string& prompt) {
    int value;
    while (true) {
        cout << prompt;
        if (cin >> value) {
            // Good input, clear the newline character from the buffer
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return value;
        } else {
            // Check for EOF or stream failure
            if (cin.eof()) {
                cout << "\n  Error: End of input detected. Exiting." << endl;
                exit(1);
            }
            // Bad input (e.g., "abc")
            cout << "  Error: Invalid input. Please enter a number." << endl;
            cin.clear(); // Clear the error flag
            cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Discard the bad input
        }
    }
}


//Pauses the program and waits for the user to press Enter.
 
void pause_for_enter() {
    cout << "\nPress Enter to return to the main menu...";
    cin.get(); // Waits for user to press Enter
}


//Handles the logic for creating a new user.
 
void create_new_user(SocialNetwork& network) {
    cout << "\n--- Create New User ---" << endl;
    
    // 1. Get New User ID (and check if it's taken)
    int newUserID = -1;
    while (true) {
        newUserID = get_int_input("Enter a new User ID (e.g., 100, 200, etc.): ");
        if (newUserID <= 0) {
             cout << "  Error: User ID must be a positive number." << endl;
        } else if (network.user_exists(newUserID)) {
            cout << "  Error: User " << newUserID << " already exists. Please pick another ID." << endl;
        } else {
            break; // Valid, available ID
        }
    }

    // 2. Get Tags
    unordered_set<string> newTags;
    cout << "Enter your personality tags. Type 'done' (or just press Enter) when finished." << endl;
    
    string tag;
    while (true) {
        cout << "> ";
        getline(cin, tag); // Read a whole line

        if (tag == "done" || tag.empty()) {
            break; // User is finished
        }
        newTags.insert(tag);
    }

    // 3. Add the user
    network.add_user(newUserID, newTags);

    cout << "\nSuccess! User " << newUserID << " has been added." << endl;
    cout << "Total users in network: " << network.get_all_users().size() << endl;
    
    pause_for_enter();
}


//Handles the logic for getting recommendations.
 
void get_recommendations(const SocialNetwork& network) {
    cout << "\n--- Get Recommendations ---" << endl;
    
    // --- 1. Get User ID ---
    int userID = -1;
    while (true) {
        userID = get_int_input("Please enter the User ID you want to get recommendations for: ");
        if (network.user_exists(userID)) {
            break; // User is valid
        } else {
            cout << "  Error: User " << userID << " not found. Please try again." << endl;
        }
    }

    // --- 2. Recommendation Sub-Menu Loop ---
    while (true) {
        cout << "\n--- Recommendation Menu (User " << userID << ") ---" << endl;
        cout << "1. Run Influence Algorithm" << endl;
        cout << "2. Run Proximity Algorithm" << endl;
        cout << "3. Run Hybrid Algorithm" << endl;
        cout << "4. Go Back to Main Menu" << endl;
        
        int typeChoice = get_int_input("Enter your choice (1-4): ");

        if (typeChoice == 4) {
            break; // Exit this sub-menu loop
        }

        if (typeChoice < 1 || typeChoice > 3) {
            cout << "  Error: Invalid choice. Please select 1, 2, 3, or 4." << endl;
            continue; // Re-ask
        }

        // --- 3. Get 'k' Value ---
        int k = -1;
        while (k <= 0) {
            k = get_int_input("How many recommendations would you like (e.g., 5): ");
            if (k <= 0) {
                cout << "  Error: Please enter a positive number." << endl;
            }
        }

        // --- 4. Run Algorithm & Print Results ---
        vector<int> recommendations;
        cout << "\n--- Generating Recommendations ---" << endl;

        if (typeChoice == 1) {
            cout << "Running INFLUENCE algorithm for User " << userID << "..." << endl;
            recommendations = recommend_by_influence(network, userID, k);
        } else if (typeChoice == 2) {
            cout << "Running PROXIMITY algorithm for User " << userID << "..." << endl;
            recommendations = recommend_by_proximity(network, userID, k);
        } else if (typeChoice == 3) {
            cout << "Running HYBRID algorithm for User " << userID << "..." << endl;
            recommendations = recommend_by_hybrid(network, userID, k, 0.5, 0.5);
        }

        cout << "\nTop " << k << " Recommendations:" << endl;
        print_recommendations(recommendations);
    }
}


//Finds and prints the user with the highest Closeness Centrality score.
 
void find_most_central_user(const SocialNetwork& network) {
    cout << "\n--- Finding Most Central User (Closeness Centrality) ---" << endl;
    cout << "Analyzing network speed..." << endl;
    
    // 1. Run the Closeness algorithm
    unordered_map<int, double> closeness_scores = calculate_closeness_scores(network);

    // 2. Find the user with the highest score
    int mostCentralUser = -1;
    double maxScore = -1.0;

    for (auto const& [userID, score] : closeness_scores) {
        if (score > maxScore) {
            maxScore = score;
            mostCentralUser = userID;
        }
    }

    // 3. Print Results
    cout << "\n--------------------------------------------------" << endl;
    if (mostCentralUser != -1) {
        // Closeness = (N - 1) / SumOfDistances. We need the inverse to show the average path length (in hops).
        int N = network.get_all_users().size();
        double sum_of_distances = (N - 1) / maxScore;
        double avg_distance = sum_of_distances / (N - 1);

        cout << "The most efficient broadcaster is: User " << mostCentralUser << endl;
        cout << "  Closeness Score (Higher is Better): " << maxScore << endl;
        cout << "  Average Path Length (in hops): " << avg_distance << endl;
    } else {
        cout << "  Network is empty or too small for analysis." << endl;
    }
    cout << "--------------------------------------------------" << endl;
    
    pause_for_enter();
}


//Handles the logic for adding or removing friendships.
 
void manage_friendships(SocialNetwork& network) {
    cout << "\n--- Manage Friendships ---" << endl;

    // 1. Get User A
    int userA = -1;
    while (true) {
        userA = get_int_input("Enter the first User ID: ");
        if (network.user_exists(userA)) break;
        cout << "  Error: User " << userA << " not found. Please try again." << endl;
    }

    // 2. Get User B
    int userB = -1;
    while (true) {
        userB = get_int_input("Enter the second User ID: ");
        if (network.user_exists(userB)) break;
        cout << "  Error: User " << userB << " not found. Please try again." << endl;
    }

    if (userA == userB) {
        cout << "  Error: Cannot manage friendship with yourself." << endl;
        pause_for_enter();
        return;
    }

    // 3. Get Action
    cout << "\nSelect Action for (" << userA << " and " << userB << "):" << endl;
    cout << "1. Add Friendship" << endl;
    cout << "2. Remove Friendship" << endl;
    cout << "3. --- Go Back ---" << endl;

    int choice = get_int_input("Enter your choice (1-3): ");

    if (choice == 1) {
        network.add_friendship(userA, userB);
    } else if (choice == 2) {
        network.remove_friendship(userA, userB);
    } else {
        return;
    }
    pause_for_enter();
}


//Handles the logic for finding the shortest path.
 
void find_shortest_path(const SocialNetwork& network) {
    cout << "\n--- Find Shortest Path (Degrees of Separation) ---" << endl;
    
    // 1. Get Start User
    int startUser = -1;
    while (true) {
        startUser = get_int_input("Enter the Start User ID: ");
        if (network.user_exists(startUser)) break;
        cout << "  Error: User " << startUser << " not found. Please try again." << endl;
    }

    // 2. Get End User
    int endUser = -1;
    while (true) {
        endUser = get_int_input("Enter the End User ID: ");
        if (network.user_exists(endUser)) break;
        cout << "  Error: User " << endUser << " not found. Please try again." << endl;
    }

    // 3. Run BFS
    vector<int> path = get_shortest_path(network, startUser, endUser);

    // 4. Print Path
    if (path.empty()) {
        cout << "\nNo path found between User " << startUser << " and User " << endUser << "." << endl;
    } else {
        cout << "\nPath found! (Length: " << path.size() - 1 << " hops)" << endl;
        cout << "  ";
        for (size_t i = 0; i < path.size(); ++i) {
            cout << path[i] << (i == path.size() - 1 ? "" : " -> ");
        }
        cout << endl;
    }
    pause_for_enter();
}

//Handles the logic for detecting communities.
 
void analyze_communities(const SocialNetwork& network) {
    cout << "\n--- Analyzing Network Communities ---" << endl;
    cout << "Running Label Propagation Algorithm..." << endl;

    // Clear previous log history and set algorithm info
    LogManager::clear();
    LogManager::setAlgorithm(
        "Community Detection",
        "Using Label Propagation to identify clusters of densely connected users. Each node iteratively adopts the most common label among its neighbors.",
        -1
    );

    // 1. Run the algorithm
    unordered_map<int, int> community_map = detect_communities(network);

    // 2. Post-process the results to print them nicely
    // We want to group users by their final community ID
    // map<CommunityID, vector<UserID>>
    map<int, vector<int>> communities; 
    for (auto const& [userID, communityID] : community_map) {
        communities[communityID].push_back(userID);
    }

    cout << "\nFound " << communities.size() << " communities:" << endl;
    cout << "---------------------------------" << endl;
    int i = 1;
    for (auto const& [communityID, members] : communities) {
        cout << "Community " << i++ << " (Label ID: " << communityID << "):" << endl;
        cout << "  Members: [ ";
        for (int userID : members) {
            cout << userID << " ";
        }
        cout << "]" << endl;
    }
    cout << "---------------------------------" << endl;
    pause_for_enter();
}


//Finds and prints the users with the highest Betweenness Centrality.
 
void find_bridge_users(const SocialNetwork& network) {
    cout << "\n--- Finding 'Bridge' Users (Betweenness Centrality) ---" << endl;
    cout << "Analyzing network connections..." << endl;

    // 1. Run the Betweenness algorithm
    unordered_map<int, double> betweenness_scores = calculate_betweenness_scores(network);

    // 2. Find the top 5 users
    // We put them in a vector<pair<score, userID>> to sort them
    vector<pair<double, int>> sorted_scores;
    for (auto const& [userID, score] : betweenness_scores) {
        sorted_scores.push_back({score, userID});
    }
    
    // Sort in descending order (highest score first)
    sort(sorted_scores.rbegin(), sorted_scores.rend());

    // 3. Print Results
    cout << "\n--------------------------------------------------" << endl;
    cout << "Top 5 'Bridge' Users (Connecting Communities):" << endl;
    
    size_t k = 5; // How many top users to show
    for (size_t i = 0; i < k && i < sorted_scores.size(); ++i) {
        int userID = sorted_scores[i].second;
        double score = sorted_scores[i].first;
        cout << "  " << (i + 1) << ". User " << userID << " (Score: " << score << ")" << endl;
    }
    cout << "--------------------------------------------------" << endl;
    
    pause_for_enter();
}

// --- Main Function ---
int main() {
    // --- Store filenames ---
    string edgeFile = "data/synthetic_edges.txt";
    string tagFile = "data/synthetic_tags.txt";

    // 1. Load Data
    SocialNetwork network;
    network.load_data(edgeFile, tagFile);
    
    cout << "--- Friendship Recommender System Loaded ---" << endl;
    cout << "Total users in network: " << network.get_all_users().size() << endl;

while (true) {
        cout << "\n--- Main Menu ---" << endl;
        cout << "1. Get Recommendations for a User" << endl;
        cout << "2. Create New User" << endl;
        cout << "3. Manage Friendships" << endl;
        cout << "4. Find Shortest Path Between Users" << endl;
        cout << "5. Analyze Network Communities" << endl;
        cout << "6. Find Most Central User" << endl;
        cout << "7. Find 'Bridge' Users" << endl;
        cout << "8. Save Network (Manual)" << endl; 
        cout << "9. Save and Exit" << endl;
        cout << "10. Export to Web Dashboard" << endl;
        
        int choice = get_int_input("Enter your choice (1-10): ");

        if (choice == 1) {
            get_recommendations(network);
        } else if (choice == 2) {
            create_new_user(network);
        } else if (choice == 3) {
            manage_friendships(network);
        } else if (choice == 4) {
            find_shortest_path(network);
        } else if (choice == 5) {
            analyze_communities(network);
        } else if (choice == 6) {
            find_most_central_user(network);
        } else if (choice == 7) {
            find_bridge_users(network); 
        } else if (choice == 8) {
            cout << "\nSaving network data..." << endl;
            network.save_data(edgeFile, tagFile);
            pause_for_enter();
        } else if (choice == 9) {
            cout << "\nSaving all changes before exiting..." << endl;
            network.save_data(edgeFile, tagFile);
            break; // Exit
        }
        else if (choice == 10) {
            cout << "\nGenerating Dashboard Data..." << endl;
            WebExporter::export_graph(network);
        } 
        else {
            cout << "  Error: Invalid choice. Please select 1-9." << endl;
            continue;
        }
    }
    cout << "\nThank you for using the Recommender System. Goodbye!" << endl;
    return 0;
}