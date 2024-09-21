
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
    int do_jump_maze(const std::vector<int>& inp) {

        int n = static_cast<int>(inp.size());
        auto jumps = inp;
        int loc = 0;
        int count = 0;

        while (loc >= 0 && loc < n) {
            count++;
            auto next_jump = jumps[loc];
            jumps[loc]++;
            loc += next_jump;
        }

        return count;
    }

    int do_jump_maze_2(const std::vector<int>& inp) {

        int n = static_cast<int>(inp.size());
        auto jumps = inp;
        int loc = 0;
        int count = 0;

        while (loc >= 0 && loc < n) {
            count++;
            auto next_jump = jumps[loc];
            jumps[loc] += (next_jump >= 3) ? -1 : 1;
            loc += next_jump;
        }

        return count;
    }
}

void aoc::y2017::day_05(const std::string& title) {

    auto inp = aoc::file_to_string_vector(
            aoc::input_path(2017, 5)
        ) | rv::transform(
            [](auto&& str) {
                return std::stoi(str);
            }
        ) | r::to<std::vector>();

    std::println("--- Day 5: {} ---", title);
    std::println("  part 1: {}", do_jump_maze(inp) );
    std::println("  part 2: {}", do_jump_maze_2(inp) );
    
}
