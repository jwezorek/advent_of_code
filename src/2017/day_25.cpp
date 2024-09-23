
#include "../util/util.h"
#include "y2017.h"
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

void aoc::y2017::day_25(const std::string& title) {

    auto inp = aoc::file_to_string_vector(
            aoc::input_path(2017, 25)
        ); 

    std::println("--- Day 25: {} ---", title);
    std::println("  part 1: {}", 0);
    std::println("  part 2: {}", 0);
    
}