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

}

void aoc::y2015::day_25(const std::string& title) {

    auto nums = aoc::file_to_string_vector(aoc::input_path(2015, 25));

    std::println("--- Day 25: {} ---", title);
    std::println("  part 1: {}",
        0
    );
    std::println("  part 2: {}",
        0
    );
}