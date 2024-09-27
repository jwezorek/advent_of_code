
#include "../util/util.h"
#include "knot_hash.h"
#include "y2017.h"
#include <filesystem>
#include <functional>
#include <print>
#include <ranges>
#include <unordered_set>
#include <span>

namespace r = std::ranges;
namespace rv = std::ranges::views;

/*------------------------------------------------------------------------------------------------*/

namespace {

    int do_part_1(const std::vector<int>& lengths) {
        auto list = rv::iota(0, 256) | r::to<std::vector>();
        aoc::knot_hash_state state;
        aoc::knot_hash_iteration(list, state, lengths);
        return list[0] * list[1];
    }
}

void aoc::y2017::day_10(const std::string& title) {
    auto inp_str = aoc::file_to_string(aoc::input_path(2017, 10));
    auto inp = aoc::split(
            inp_str, ','
        ) | rv::transform(
            [](const std::string& str) {
                return std::stoi(str);
            }
        ) | r::to<std::vector>();

    std::println("--- Day 10: {} ---", title);
    std::println("  part 1: {}", do_part_1(inp) );
    std::println("  part 2: {}", knot_hash(inp_str));
    
}
