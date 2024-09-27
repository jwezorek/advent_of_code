#pragma once

#include <vector>
#include <string>

namespace aoc {

    struct knot_hash_state {
        int pos;
        int skip_size;

        knot_hash_state();
    };

    void knot_hash_iteration(std::vector<int>& list, knot_hash_state& state, const std::vector<int>& lengths);
    std::vector<int> knot_hash_to_values(const std::string& inp);
    std::string knot_hash(const std::string& inp);
}