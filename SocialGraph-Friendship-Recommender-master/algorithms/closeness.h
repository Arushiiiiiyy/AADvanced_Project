#pragma once

using namespace std;
#include "../SocialNetwork.h"
#include <unordered_map>
#include <vector>


//Calculates the Closeness Centrality score for every user.
//network The populated SocialNetwork object.
//Returns an unordered_map<int, double> mapping UserID to its Closeness Score.
 
unordered_map<int, double> calculate_closeness_scores(const SocialNetwork& network);