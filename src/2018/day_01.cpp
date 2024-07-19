
#include "../util/util.h"
#include "y2018.h"
#include <filesystem>
#include <functional>
#include <print>
#include <ranges>
#include <unordered_set>

namespace r = std::ranges;
namespace rv = std::ranges::views;

/*------------------------------------------------------------------------------------------------*/

namespace {

    auto cycle(const auto& seq) {
        return rv::iota(0) | rv::transform(
            [&](auto i) {
                return seq.at(i % seq.size());
            }
        );
    }

    int find_first_repeated_freq(const std::vector<int>& changes) {
        std::unordered_set<int> freqs;
        int freq = 0;
        for (auto change : cycle(changes)) {
            freq += change;
            if (freqs.contains(freq)) {
                return freq;
            }
            freqs.insert(freq);
        }
        return -1;
    }
}

void aoc::y2018::day_01(const std::string& title) {

    auto inp = aoc::file_to_string_vector(
            aoc::input_path(2018, 1)
        ) | rv::transform(
            [](auto&& str)->int {
                return std::stoi(str);
            }
        ) | r::to<std::vector>();

    std::println("--- Day 1: {} ---", title);
    std::println("  part 1: {}", r::fold_left(inp, 0, std::plus<>()));
    std::println("  part 2: {}", find_first_repeated_freq(inp));
    
}
