
#include "../util/util.h"
#include "../util/concat.h"
#include "y2017.h"
#include <filesystem>
#include <functional>
#include <print>
#include <ranges>
#include <unordered_set>

namespace r = std::ranges;
namespace rv = std::ranges::views;

/*------------------------------------------------------------------------------------------------*/

namespace {
    int sum_of_digits_matching_next_digit(const std::vector<int>& inp, int step) {
        int sum = 0;
        int n = static_cast<int>(inp.size());
        for (auto i = 0; i < n; ++i) {
            auto val = inp.at(i);
            auto next_val = inp.at((i + step) % n);
            if (val == next_val) {
                sum += val;
            }
        }
        return sum;
    }
}

void aoc::y2017::day_01(const std::string& title) {
    
    auto inp = aoc::file_to_string(
            aoc::input_path(2017, 1)
        ) | rv::transform(
            [](char ch)->int {
                return ch - '0';
            }
        ) | r::to<std::vector>();
    
    std::println("--- Day 1: {} ---", title);
    std::println("  part 1: {}", sum_of_digits_matching_next_digit(inp, 1));
    std::println("  part 2: {}", sum_of_digits_matching_next_digit(inp, inp.size()/2));
    
}
