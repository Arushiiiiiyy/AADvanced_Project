#pragma once

#include "../SocialNetwork.h"
#include <vector>


//Recommends users using a hybrid rank-based approach.
//network The populated SocialNetwork object.
//userID The user to generate recommendations for.
//top_k The number of recommendations to return.
//w_influence The weight given to the influence rank.
//w_proximity The weight given to the proximity rank.
//Returns a vector of recommended user IDs, sorted by hybrid score.
 
vector<int> recommend_by_hybrid(const SocialNetwork& network, int userID, int top_k, double w_influence, double w_proximity);