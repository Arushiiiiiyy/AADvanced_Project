Friendship Recommendation System

This is a dynamic, persistent C++ application that models a social network and provides advanced friend recommendations. It's built "from scratch" using only standard C++ libraries.

The application is fully interactive, running from a robust menu system. All changes, such as adding new users or new friendships, are automatically saved to the data files upon exiting the program.

How to Compile
Navigate to the project's root directory.
This will compile all .cpp files and create a single executable named recommender.
make
To clean up all build files (all .o files and the executable), run:
make clean

How to Run
This project uses a user-friendly, interactive menu. You do not need any command-line arguments.
Run the executable:
./recommender
You will be greeted with the main menu, from which you can navigate to all features.

Core Features

This project includes a wide range of advanced graph algorithms and application features.
Advanced Recommendation Algorithms

The core recommendation engine has been upgraded from simple counts to more advanced, well-known data science algorithms.

Influence (PageRank + Jaccard): Recommends users based on a combination of shared interests (Jaccard Similarity) and true influence. 

Influence is no longer a simple friend count (Degree) but is now calculated using the PageRank algorithm, which measures transitive popularity.

Proximity (Adamic-Adar): Recommends "friends-of-friends." This is no longer a simple "common friend" count. It now uses the Adamic-Adar index, which gives more weight to common friends who are "closer" (i.e., have fewer friends themselves), resulting in much smarter recommendations.

Hybrid (Weighted Sum): A robust hybrid model that combines the normalized PageRank/Jaccard score and the Adamic-Adar score.

Dynamic Graph Engine

The social network is not static. You can modify it in real-time.

Create New Users: The "Create New User" menu allows you to add a new user to the graph. You can give them a new User ID and assign them a custom list of personality tags. This is a perfect demonstration of solving the "cold start" problem (new users with 0 friends).

Manage Friendships: The "Manage Friendships" menu allows you to dynamically add or remove a friendship (an edge) between any two users in the graph.

Persistence (Save on Exit)

This is a stateful application. You do not lose your work when you quit.

Automatic Save: All changes—including new users you create and new friendships you add or remove—are automatically saved back to the data/ files when you select "Save and Exit" from the main menu. The next time you launch the program, your modified graph is loaded.

Graph Analysis Tools

Shortest Path (BFS): The "Find Shortest Path" feature allows you to find the "degrees of separation" between any two users. It uses a Breadth-First Search (BFS) algorithm implemented from scratch to find and display the shortest path (e.g., 1 -> 5 -> 12 -> 11).

Robust User Interface

Interactive Menu: All features are accessible through a robust, multi-layered, interactive menu system with full input validation to prevent crashes from bad input.

Recommendation Sub-Menu: When getting recommendations, you are placed in a sub-menu. This allows you to select a user and then run all three recommendation algorithms (Influence, Proximity, Hybrid) on that same user without having to go back to the main menu.

Code Structure

main.cpp: Contains all the logic for the interactive menu system and user input validation.

SocialNetwork.h / .cpp: The core class that holds the graph data (adjacency lists) and tag data. It manages all data modifications (adding users/friends) and file I/O (loading and saving).

algorithms/: A directory containing all "from scratch" algorithm implementations.

pagerank.h / .cpp: Implements the PageRank algorithm.

proximity.h / .cpp: Implements the Adamic-Adar algorithm.

influence.h / .cpp: Implements the PageRank * Jaccard score.

hybrid.h / .cpp: Implements the combined hybrid model.

bfs.h / .cpp: Implements the Breadth-First Search algorithm for shortest paths.

utils.h: Contains helper functions like jaccard_similarity.

data/: Contains the graph edge list and tag files.

Makefile: The build script.
