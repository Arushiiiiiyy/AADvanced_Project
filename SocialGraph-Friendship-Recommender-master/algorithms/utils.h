#pragma once

#include <unordered_set>
#include <string>


//Calculates Jaccard Similarity between two sets of strings.
//setA First set of tags.
//setB Second set of tags.
//Return Jaccard Index (Intersection over Union) as a double.
 
inline double jaccard_similarity(const unordered_set<string>& setA, const unordered_set<string>& setB) {
    if (setA.empty() && setB.empty()) return 1.0; // Two empty sets are identical

    // Ensure we iterate over the smaller set for efficiency
    const auto& smallerSet = (setA.size() < setB.size()) ? setA : setB;
    const auto& largerSet = (setA.size() < setB.size()) ? setB : setA;

    int intersection = 0;
    for (const auto& tag : smallerSet) {
        if (largerSet.count(tag)) {
            intersection++;
        }
    }

    int unionSize = setA.size() + setB.size() - intersection;
    if (unionSize == 0) return 0.0; // Avoid division by zero if one set is empty

    return static_cast<double>(intersection) / unionSize;
}