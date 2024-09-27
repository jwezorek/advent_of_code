
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
    uint64_t do_part_1(uint64_t gen_a, uint64_t gen_b) {
        return 0;
    }
}

void aoc::y2017::day_15(const std::string& title) {

    auto [gen_a, gen_b] = create_tuple<2>(
            extract_numbers(aoc::file_to_string(aoc::input_path(2017, 15)))
        );
    std::println("--- Day 15: {} ---", title);
    std::println("  part 1: {}", do_part_1(gen_a, gen_b));
    std::println("  part 2: {}", 0);
    
}
