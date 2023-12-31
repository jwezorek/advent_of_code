#include "../util.h"
#include "y2021.h"
#include <filesystem>
#include <functional>
#include <print>
#include <ranges>
#include <unordered_map>

namespace r = std::ranges;
namespace rv = std::ranges::views;

/*------------------------------------------------------------------------------------------------*/

void aoc::y2021::day_01(const std::string& title) {
    auto input = aoc::file_to_string_vector(aoc::input_path(2021, 1));
    auto depths = input | rv::transform(
            [](auto&& n) {return std::stoi(n); }
        ) | r::to<std::vector<int>>();

    std::println("--- Day 1: {} ---", title);
    std::println("  part 1: {}", 
        r::fold_left(
            depths | rv::pairwise_transform(
                [](int depth1, int depth2) {
                    return (depth2 > depth1) ? 1 : 0;
                }
            ),
            0,
            std::plus<>()
        )
    );

    std::println("  part 2: {}", 
        r::fold_left(
            depths | 
            rv::slide(3) |
            rv::pairwise_transform(
                [](auto&& lhs, auto&& rhs) {
                    auto depth1 = r::fold_left(lhs, 0, std::plus<>());
                    auto depth2 = r::fold_left(rhs, 0, std::plus<>());
                    return (depth2 > depth1) ? 1 : 0;
                }
            ),
            0,
            std::plus<>()
        )
    );
}