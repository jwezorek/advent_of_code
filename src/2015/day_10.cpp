#include "../util.h"
#include "y2015.h"
#include <filesystem>
#include <functional>
#include <print>
#include <ranges>
#include <unordered_set>

namespace r = std::ranges;
namespace rv = std::ranges::views;

/*------------------------------------------------------------------------------------------------*/

namespace {

    std::vector<int> look_and_say(const std::vector<int>& inp) {
        auto summarized_groups = inp | rv::chunk_by(
                [](auto lhs, auto rhs) {
                    return lhs == rhs;
                }
            ) | rv::transform(
                [](auto rng) {
                    return std::array<int, 2>{{
                        static_cast<int>(r::distance(rng)), rng[0]
                    }}; 
                }
            ) | r::to<std::vector>();
        return summarized_groups | rv::join | r::to<std::vector>();
    }

    std::vector<int> look_and_say_n_times(const std::vector<int>& inp, int n) {
        auto series = inp;
        for (int i = 0; i < n; ++i) {
            series = look_and_say(series);
        }
        return series;
    }
}

void aoc::y2015::day_10(const std::string& title) {

    auto series = aoc::file_to_string(
            aoc::input_path(2015, 10)
        ) | rv::transform(
            [](char ch)->int {
                return ch - '0';
            }
        ) | r::to<std::vector>();

    std::println("--- Day 10: {} ---", title);
    std::println("  part 1: {}",
        look_and_say_n_times(series,40).size()
    );
    std::println("  part 2: {}",
        look_and_say_n_times(series, 50).size()
    );
}