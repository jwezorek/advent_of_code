
#include "../util/util.h"
#include "../util/vec2.h"
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

    using point = aoc::vec2<int>;
    using point_set = aoc::vec2_set<int>;

    struct instruction {
        bool left;
        int distance;
    };

    enum direction {
        north = 0,
        east,
        south,
        west
    };

    point direction_delta(direction dir) {
        const static std::array<point, 4> deltas = {{
            {0,-1},{1,0},{0,1},{-1,0}
        }};
        return deltas[static_cast<int>(dir)];
    }

    direction turn(direction curr, bool left) {
        return static_cast<direction>(
            (curr + (left ? -1 : 1) + 4) % 4
        );
    }

    instruction parse_instruction(const std::string& str) {
        auto num = aoc::extract_numbers(str);
        return {
            str.front() == 'L',
            num.front()
        };
    }

    int manhattan_dist_from_origin(const point& pt) {
        return std::abs(pt.x) + std::abs(pt.y);
    }

    int follow_instructions(const std::vector<instruction>& instrs) {
        using state = std::tuple<point, direction>;
        return manhattan_dist_from_origin(
            std::get<0>(
                r::fold_left(
                    instrs,
                    state{ {0,0}, north },
                    [](const state& s, const instruction& instr)->state {
                        auto [loc, dir] = s;
                        dir = turn(dir, instr.left);
                        loc = loc + instr.distance * direction_delta(dir);
                        return { loc,dir };
                    }
                )
            )
        );
    }

    int distance_to_first_crossing(const std::vector<instruction>& instrs) {
        point_set visited;
        point loc = { 0,0 };
        direction dir = north;
        for (const auto& instr : instrs) {
            dir = turn(dir, instr.left);
            auto delta = direction_delta(dir);
            for (int i = 0; i < instr.distance; ++i) {
                loc = loc + delta;
                if (visited.contains(loc)) {
                    return std::abs(loc.x) + std::abs(loc.y);
                }
                visited.insert(loc);
            }
        }
        return -1;
    }
}

void aoc::y2016::day_01(const std::string& title) {

    auto inp = aoc::extract_alphanumeric(
            aoc::file_to_string(
                aoc::input_path(2016, 1)
            )
        ) | rv::transform(
            parse_instruction
        ) | r::to<std::vector>();

    std::println("--- Day 1: {} ---", title);
    std::println("  part 1: {}", follow_instructions(inp) );
    std::println("  part 2: {}", distance_to_first_crossing(inp) );
    
}
