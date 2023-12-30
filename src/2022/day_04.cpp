#include "../util.h"
#include "y2022.h"
#include <filesystem>
#include <functional>
#include <print>
#include <ranges>
#include <unordered_map>
#include <unordered_set>

namespace r = std::ranges;
namespace rv = std::ranges::views;

/*------------------------------------------------------------------------------------------------*/

namespace {
    struct interval {
        int from; 
        int to;
    };

    bool is_in_interval(const interval& intv, int v) {
        return v >= intv.from && v <= intv.to;
    }

    bool is_contained_by(const interval& container, const interval& containee) {
        return is_in_interval(container, containee.from) &&
            is_in_interval(container, containee.to);
    }

    bool overlaps(const interval& lhs, const interval& rhs) {
        if (is_in_interval(rhs, lhs.from)) {
            return true;
        }
        if (is_in_interval(rhs, lhs.to)) {
            return true;
        }
        if (is_in_interval(lhs, rhs.from)) {
            return true;
        }
        if (is_in_interval(lhs, rhs.to)) {
            return true;
        }
        return false;
    }

    interval string_to_interval(const std::string& str) {
        auto from_to = aoc::split(str, '-');
        return { std::stoi(from_to[0]), std::stoi(from_to[1]) };
    }

    std::tuple<interval, interval> parse_line_of_input(const std::string& inp_line) {
        auto pieces = aoc::split(inp_line, ',');
        return { string_to_interval(pieces[0]), string_to_interval(pieces[1]) };
    }
}

void aoc::y2022::day_04(const std::string& title) {
    auto input = file_to_string_vector(input_path(2022, 4));

    int part_1 = r::fold_left(
        input |
            rv::transform(
                [](const std::string& inp_line)->int {
                    auto [intv1, intv2] = parse_line_of_input(inp_line);
                    return (is_contained_by(intv1, intv2) || is_contained_by(intv2, intv1)) ? 1 : 0;
                }
            ),
        0,
        std::plus<>()
    );

    int part_2 = r::fold_left(
        input |
            rv::transform(
                [](const std::string& inp_line)->int {
                    auto [intv1, intv2] = parse_line_of_input(inp_line);
                    return overlaps(intv1,intv2) ? 1 : 0;
                }
            ),
        0,
        std::plus<>()
    ); 

    std::println("--- Day 4: {} ---", title);
    std::println("  part 1: {}", part_1);
    std::println("  part 2: {}", part_2);
}