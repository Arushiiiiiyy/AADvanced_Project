#pragma once

using namespace std;
#include "../SocialNetwork.h"
#include <unordered_map>
#include <vector>


//Detects communities in the graph using the Label Propagation Algorithm.
//network The populated SocialNetwork object.
//iterations The number of iterations to run (e.g., 10).
//Returns an unordered_map<int, int> mapping UserID to its final CommunityID.
 
unordered_map<int, int> detect_communities(const SocialNetwork& network, int iterations = 10);