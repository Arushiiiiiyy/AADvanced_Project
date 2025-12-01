#pragma once

#include "../SocialNetwork.h"
#include <unordered_map>


//Calculates the PageRank score for all nodes in the graph.
//network The populated SocialNetwork object.
//dampingFactor (d) How much to trust the random walk. (0.85 is standard).
//iterations The number of iterations to run. (More is more accurate).
//Return an std::unordered_map<int, double> mapping UserID to PageRank score.
 
std::unordered_map<int, double> calculate_pagerank(
    const SocialNetwork& network,
    double dampingFactor = 0.85,
    int iterations = 100,
    bool log_results = true
);