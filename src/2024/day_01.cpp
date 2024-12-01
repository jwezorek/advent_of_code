
#include "../util/util.h"
#include "y2024.h"
#include <algorithm>
#include <functional>
#include <print>
#include <ranges>
#include <unordered_map>

namespace r = std::ranges;
namespace rv = std::ranges::views;

/*------------------------------------------------------------------------------------------------*/

namespace {
    int sum_of_distances( std::vector<int> left, std::vector<int> right) {

        r::sort(left);
        r::sort(right);

        return r::fold_left(
            rv::zip(left, right) | rv::transform(
                [](auto&& pair) {
                    auto [lhs, rhs] = pair;
                    return std::abs(lhs - rhs);
                }
            ),
            0,
            std::plus<>()
        );
    }

    int similarity(const std::vector<int>& left, const std::vector<int>& right) {

        std::unordered_map<int, int> right_counts;
        for (auto v : right) {
            ++right_counts[v];
        }

        return r::fold_left(
            left | rv::transform(
                [&](auto lhs) {
                    return lhs * right_counts[lhs];
                }
            ),
            0,
            std::plus<>()
        );
    }
}

void aoc::y2024::day_01(const std::string& title) {

    auto inp = aoc::file_to_string_vector(
            aoc::input_path(2024, 1)
        ) | rv::transform(
            [](auto&& str) { return aoc::extract_numbers(str); }
        ) | r::to<std::vector>();

    auto left = inp | rv::transform([](auto&& v) {return v[0]; }) | r::to<std::vector>();
    auto right = inp | rv::transform([](auto&& v) {return v[1]; }) | r::to<std::vector>();

    std::println("--- Day 1: {} ---", title);
    std::println("  part 1: {}", sum_of_distances(left, right) );
    std::println("  part 2: {}", similarity(left, right) );
    
}
