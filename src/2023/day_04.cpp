#include "days.h"
#include "../util.h"
#include <print>
#include <ranges>
#include <numeric>
#include <algorithm>
#include <regex>

namespace r = std::ranges;
namespace rv = std::ranges::views;

namespace {

}

/*------------------------------------------------------------------------------------------------*/

void aoc::y2023::day_04(const std::string& title) {

    auto input = aoc::file_to_string_vector(aoc::input_path(2023, 4));

    std::println("--- Day 4: {0} ---\n", title);

    for (const auto& line : input) {
        std::println("{}", line);
    }

}