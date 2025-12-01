#pragma once

#include "../SocialNetwork.h"
#include <vector>

//Recommends users based on influence (Degree * Jaccard).
//network The populated SocialNetwork object.
//userID The user to generate recommendations for.
//top_k The number of recommendations to return.
//Returns a vector of recommended user IDs, sorted by score.
 
vector<int> recommend_by_influence(const SocialNetwork& network, int userID, int top_k);