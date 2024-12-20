
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

}

void aoc::y2016::day_17(const std::string& title) {

    auto inp = aoc::file_to_string_vector(
            aoc::input_path(2016, 17)
        ); 

    std::println("--- Day 17: {} ---", title);
    std::println("  part 1: {}", 0);
    std::println("  part 2: {}", 0);
    
}