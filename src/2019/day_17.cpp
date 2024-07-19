#include "../util/util.h"
#include "y2019.h"
#include "intcode.h"
#include <filesystem>
#include <functional>
#include <print>
#include <format>
#include <fstream>
#include <ranges>
#include <unordered_set>
#include <boost/functional/hash.hpp>

namespace r = std::ranges;
namespace rv = std::ranges::views;

/*------------------------------------------------------------------------------------------------*/

namespace {

    struct point {
        int x;
        int y;
    };

    using computer = aoc::intcode_computer;

    std::vector<std::string> generate_grid(const std::vector<int64_t>& program) {
        computer rob(program);
        std::stringstream ss;
        rob.run(
            aoc::input_fn{},
            [&ss](int64_t output) {
                ss << static_cast<char>(output);
            }
        );
        auto grid = aoc::split(ss.str(), '\n');
        grid.pop_back(); // teminating EOL makes a blank line at the end...
        return grid;
    }

    std::vector<point> find_intersections(const std::vector<std::string>& grid) {
        int hgt = static_cast<int>(grid.size());
        int wd = static_cast<int>(grid[0].size());

        auto is_floor = [&](int x, int y)->bool {
            if (y < 0 || y >= hgt || x < 0 || x >= wd) {
                return false;
            }
            return grid[y][x] != '.';
        };

        std::vector<point> intersections;
        for (int y = 0; y < hgt; ++y) {
            for (int x = 0; x < wd; ++x) {
                if (is_floor(x,y) && is_floor(x,y-1) && is_floor(x,y+1) &&
                        is_floor(x - 1, y) && is_floor(x + 1, y) ) {
                    intersections.emplace_back(x, y);
                }
            }
        }

        return intersections;
    }

    int calculate_alignment_parameters(const std::vector<point>& intersections) {
        return r::fold_left(
            intersections | rv::transform(
                [](auto&& pt)->int {
                    return pt.x * pt.y;
                }
            ),
            0,
            std::plus<>()
        );
    }

    int64_t do_part_2(const std::vector<int64_t>& program) {
        computer comp(program);

        comp.value(0) = 2;

        std::string cmds = 
            "A,A,B,C,B,C,B,C,A,C\nR,6,L,8,R,8\nR,4,R,6,R,6,R,4,R,4\nL,8,R,6,L,10,L,10\nn\n";
        auto num_cmds = cmds | rv::transform(
                [](char ch)->int64_t {
                    return static_cast<int64_t>(ch);
                }
            ) | r::to<std::vector>();

        aoc::input_buffer buffer(num_cmds);
        comp.run(buffer);

        return comp.output();
    }
}

void aoc::y2019::day_17(const std::string& title) {

    auto program = split(
            aoc::file_to_string(aoc::input_path(2019, 17)), ','
        ) | rv::transform(
            [](auto&& str)->int64_t {
                return aoc::string_to_int64(str);
            }
        ) | r::to<std::vector>();

    auto grid = generate_grid(program);

    std::println("--- Day 17: {} ---", title);

    std::println("  part 1: {}",
        calculate_alignment_parameters(
            find_intersections(grid) 
        )
    );

    std::println("  part 2: {}", do_part_2(program) );
    
}