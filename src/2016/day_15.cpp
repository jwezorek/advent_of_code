
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

    struct disc {
        int index;
        int num_positions;
        int starting_postion;
    };

    disc parse_disc(const std::string& str) {
        auto parts = aoc::extract_numbers(str);
        return {
            parts[0],
            parts[1],
            parts[3]
        };
    }

    int brute_force(const std::vector<disc>& discs) {
        int time = 0;
        bool done = false;
        do {
            done = true;
            for (const auto& disc : discs) {
                if (((time + disc.index + disc.starting_postion) %
                    disc.num_positions) != 0) {
                    done = false;
                    ++time;
                    break;
                }
            }

        } while (!done);

        return time;
    }
}

void aoc::y2016::day_15(const std::string& title) {

    auto inp = aoc::file_to_string_vector(
        aoc::input_path(2016, 15)
    ) | rv::transform(
        parse_disc
    ) | r::to<std::vector>();

    std::println("--- Day 15: {} ---", title);
    std::println("  part 1: {}", brute_force(inp) );

    inp.emplace_back(
        inp.back().index + 1,
        11,
        0
    );
    std::println("  part 2: {}", brute_force(inp) );
    
}
