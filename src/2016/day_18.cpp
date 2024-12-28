
#include "../util/util.h"
#include "y2016.h"
#include <filesystem>
#include <functional>
#include <print>
#include <ranges>
#include <format>
#include <unordered_set>

namespace r = std::ranges;
namespace rv = std::ranges::views;

/*------------------------------------------------------------------------------------------------*/

namespace {
    std::string pad_row(const std::string& row) {
        return std::format(".{}.", row);
    }

    char next_char(char left, char center, char right) {
        if (left == '^' && center == '^' && right == '.') {
            return '^';
        }
        if (left == '.' && center == '^' && right == '^') {
            return '^';
        }
        if (left == '^' && center == '.' && right == '.') {
            return '^';
        }
        if (left == '.' && center == '.' && right == '^') {
            return '^';
        }
        return '.';
    }

    std::string next_row(const std::string& row) {
        return pad_row(row) | rv::adjacent<3> |
            rv::transform(
                [](auto&& triplet)->char {
                    const auto& [left, center, right] = triplet;
                    return next_char(left, center, right);
                }
            ) | r::to<std::string>();
    }

    int count_safe_tiles(const std::string& row) {
        return r::count_if(
            row,
            [](char ch) {return ch == '.'; }
        );
    }

    int count_safe_tiles(const std::string& initial_row, int n) {
        auto row = initial_row;
        int count = count_safe_tiles(row);
        for (int i = 1; i < n; ++i) {
            row = next_row(row);
            count += count_safe_tiles(row);
        }
        return count;
    }
}

void aoc::y2016::day_18(const std::string& title) {

    auto inp = trim(
        aoc::file_to_string(
            aoc::input_path(2016, 18)
        )
    );

    std::println("--- Day 18: {} ---", title);
    std::println("  part 1: {}", count_safe_tiles( inp , 40) );
    std::println("  part 2: {}", count_safe_tiles( inp, 400000 ));
    
}
