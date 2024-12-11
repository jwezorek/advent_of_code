
#include "../util/util.h"
#include "y2016.h"
#include <filesystem>
#include <functional>
#include <print>
#include <ranges>
#include <unordered_set>

namespace r = std::ranges;
namespace rv = std::ranges::views;

/*------------------------------------------------------------------------------------------------*/

namespace {

    bool is_triangle(const std::vector<int>& inp) {
        auto sides = inp;
        r::sort(sides);
        return sides[0] + sides[1] > sides[2];
    }

    auto nth_column(const std::vector<std::vector<int>>& inp, int n) {
        return inp | rv::transform(
            [n](auto&& v) {
                return v.at(n);
            }
        );
    }

    std::vector<std::vector<int>> vertical_groups_of_3(
            const std::vector<std::vector<int>>& inp) {
        return rv::iota(0, 3) | rv::transform(
            [&](int n) {
                return nth_column(inp, n);
            }
        ) | rv::join | rv::chunk(3) | rv::transform(
            [](auto triple)->std::vector<int> {
                return triple | r::to<std::vector>();
            }
        ) | r::to<std::vector>();
    }
}

void aoc::y2016::day_03(const std::string& title) {

    auto inp = aoc::file_to_string_vector(
        aoc::input_path(2016, 3)
    ) | rv::transform(
        [](auto&& str) {
            return aoc::extract_numbers(str);
        }
    ) | r::to<std::vector>();

    auto part2_inp = vertical_groups_of_3(inp);

    std::println("--- Day 3: {} ---", title);
    std::println("  part 1: {}", r::count_if(inp, is_triangle) );
    std::println("  part 2: {}", r::count_if(part2_inp, is_triangle));
    
}
