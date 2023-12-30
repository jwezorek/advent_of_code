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

namespace {
}

void aoc::y2021::day_03(const std::string& title) {
    auto input = aoc::file_to_string_vector(aoc::input_path(2023, 1));

    std::println("--- Day 3: {} ---", title);
    std::println("  part 1: {}", 0);
    std::println("  part 2: {}", 0);
}