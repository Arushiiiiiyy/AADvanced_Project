#include <string>
#include <unordered_set>
#include <algorithm> // For std::min

/**
 * @brief Calculates the Jaccard Similarity Index between two sets of tags.
 *
 * @param set_a The first set of tags (e.g., for node A).
 * @param set_b The second set of tags (e.g., for node B).
 * @return double The Jaccard score (0.0 to 1.0).
 */
// double jaccard_similarity(const std::unordered_set<std::string>& set_a,
//                           const std::unordered_set<std::string>& set_b) {
    
//     // Iterate over the smaller set for efficiency
//     const auto& smaller_set = (set_a.size() < set_b.size()) ? set_a : set_b;
//     const auto& larger_set = (set_a.size() < set_b.size()) ? set_b : set_a;
    
//     double intersection = 0;
//     for (const std::string& item : smaller_set) {
//         if (larger_set.count(item)) {
//             intersection++;
//         }
//     }

//     // |A U B| = |A| + |B| - |A ∩ B|
//     double union_size = set_a.size() + set_b.size() - intersection;

//     [cite_start]// Handle the divide-by-zero case: J(Ø, Ø) is 0 [cite: 362]
//     if (union_size == 0) {
//         return 0.0;
//     }

//     // J(A, B) = |A ∩ B| [cite_start]/ |A ∪ B| [cite: 322]
//     return intersection / union_size;
// }
double jaccard_similarity(const std::unordered_set<std::string>& set_a,
                          const std::unordered_set<std::string>& set_b) {

    const auto& smaller_set = (set_a.size() < set_b.size()) ? set_a : set_b;
    const auto& larger_set  = (set_a.size() < set_b.size()) ? set_b : set_a;

    double intersection = 0.0;
    for (const std::string& item : smaller_set) {
        if (larger_set.count(item)) {
            intersection++;
        }
    }

    double union_size = set_a.size() + set_b.size() - intersection;

    // Handle the divide-by-zero case: J(Ø, Ø) is 0
    if (union_size == 0) {
        return 0.0;
    }

    return intersection / union_size;
}