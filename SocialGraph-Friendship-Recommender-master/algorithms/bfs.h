#pragma once

#include "../SocialNetwork.h"
#include <vector>


//Finds the shortest path between two users using BFS.
//network The populated SocialNetwork object.
//startUser The user ID to start the search from.
//endUser The user ID to find a path to.
//Returns a std::vector<int> representing the path from start to end.
//If no path is found, returns an empty vector.

vector<int> get_shortest_path(const SocialNetwork& network, int startUser, int endUser);