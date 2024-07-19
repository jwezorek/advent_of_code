#include "../util/util.h"
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
    int row_and_col_to_index(int row, int col) {
        row -= 1; // Input is one-based not zero-based...
        col -= 1;
        int n = row + col + 1;
        auto tri = (n * (n + 1)) / 2;
        return tri - row;
    }

    int64_t nth_code(int64_t n) {
        int64_t code = 20151125;
        for (int i = 0; i < n-1; ++i) {
            code = (code * 252533) % 33554393;
        }
        return code;
    }
}

void aoc::y2015::day_25(const std::string& title) {

    auto nums = aoc::extract_numbers(
        aoc::file_to_string(aoc::input_path(2015, 25))
    );

    std::println("--- Day 25: {} ---", title);
    std::println("  part 1: {}",
        nth_code(row_and_col_to_index(nums[0], nums[1]))
    );
    std::println("  part 2: {}",
        "<xmas freebie>"
    );
}