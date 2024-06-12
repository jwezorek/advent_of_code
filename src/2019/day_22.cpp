#include "../util.h"
#include "y2019.h"
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

void aoc::y2019::day_22(const std::string& title) {

    auto inp = aoc::file_to_string_vector(aoc::input_path(2019, 2)) | rv::transform(
        [](auto&& str) {return std::stoi(str); }
    ) | r::to<std::vector<int>>();

    std::println("--- Day 1: {} ---", title);
    std::println("  part 1: {}",
        0
    );
    std::println("  part 2: {}",
        0
    );
}