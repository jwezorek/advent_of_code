#include "../util.h"
#include "y2020.h"
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

void aoc::y2020::day_19(const std::string& title) {
    auto input = aoc::file_to_string_vector(aoc::input_path(2020, 19));

    std::println("--- Day 19: {} ---", title);
    std::println("  part 1: {}", 0);
    std::println("  part 2: {}", 0);
}