#pragma once

using namespace std;
#include "../SocialNetwork.h"
#include <unordered_map>
#include <vector>


//Calculates the Betweenness Centrality score for every user.
//network: The populated SocialNetwork object.
//Returns an unordered_map<int, double> mapping UserID to its Betweenness Score.
 
unordered_map<int, double> calculate_betweenness_scores(const SocialNetwork& network);