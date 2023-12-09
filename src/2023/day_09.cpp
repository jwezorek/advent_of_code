#include "days.h"
#include "../util.h"
#include <print>
#include <ranges>
#include <numeric>
#include <tuple>
#include <algorithm>
#include <unordered_map>
#include <utility>

namespace r = std::ranges;
namespace rv = std::ranges::views;

namespace {
    std::vector<std::vector<int>> parse_input(const std::vector<std::string>& lines) {
        return lines |
            rv::transform(
                [](const auto& line) {
                    return aoc::extract_numbers(line, true);
                }
            ) | r::to<std::vector<std::vector<int>>>();
    }

    std::vector<int> next_row(const std::vector<int>& row) {
        return row |
            rv::pairwise_transform(
                [](int lhs, int rhs) {
                    return rhs - lhs;
                }
            )| r::to<std::vector<int>>();
    }

    bool all_zeros(const std::vector<int>& row) {
        return r::all_of(row, [](int v) {return v == 0; });
    }

    int find_next_item(const std::vector<int>& input) {
        int sum_of_last_item = 0;
        auto row = input;
        while (!all_zeros(row)) {
            sum_of_last_item += row.back();
            row = next_row(row);
        }
        return sum_of_last_item;
    }
}

/*------------------------------------------------------------------------------------------------*/

void aoc::y2023::day_09(const std::string& title) {

    auto input = parse_input(aoc::file_to_string_vector(aoc::input_path(2023, 9)));

    std::println("--- Day 9: {0} ---\n", title);
    std::println("  part 1: {}",
        r::fold_left(
            input | rv::transform(
                [](auto&& row) {
                    return find_next_item(row);
                }
            ),
            0, std::plus<>()
        )
    );

    std::println("  part 2: {}",
        r::fold_left(
            input | rv::transform(
                [](auto&& row) {
                    r::reverse(row);
                    return find_next_item(row);
                }
            ),
            0, std::plus<>()
        )
    );
}