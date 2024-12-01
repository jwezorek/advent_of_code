
#include "../util/util.h"
#include "y2024.h"
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

void aoc::y2024::day_07(const std::string& title) {

    auto inp = aoc::file_to_string_vector(
            aoc::input_path(2024, 7)
        ); 

    std::println("--- Day 7: {} ---", title);
    std::println("  part 1: {}", 0);
    std::println("  part 2: {}", 0);
    
}
