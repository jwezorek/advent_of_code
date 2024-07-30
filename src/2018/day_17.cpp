
#include "../util/util.h"
#include "../util/vec2.h"
#include "y2018.h"
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

    enum tile_type {
        wall,
        falling_water,
        standing_water
    };

    using reservoir = aoc::vec2_map<int, tile_type>;

    struct drawing_cmd {
        bool horz;
        int where;
        int start;
        int end;
    };

    drawing_cmd parse_command(const std::string& str) {
        auto vals = aoc::extract_numbers(str);
        return {
            str.front() == 'y',
            vals[0],
            vals[1],
            vals[2]
        };
    }

    void do_commands(reservoir& res, const std::vector<drawing_cmd>& cmds) {
        for (const auto& cmd : cmds) {
            for (int i = cmd.start; i <= cmd.end; ++i) {
                auto pt = (cmd.horz) ? point{ i, cmd.where } : point{ cmd.where,i };
                res[pt] = wall;
            }
        }
    }

    reservoir make_reservoir(const std::vector<drawing_cmd>& cmds) {
        ::reservoir reservoir;
        do_commands(reservoir, cmds);
        return reservoir;
    }

    std::tuple<int, int, int, int> bounds(const reservoir& res) {
        auto [x1, x2] = 
            r::minmax(res | rv::keys | rv::transform([](auto&& p) {return p.x; }));
        auto [y1, y2] =
            r::minmax(res | rv::keys | rv::transform([](auto&& p) {return p.y; }));
        return { x1, y1, x2 - x1 + 1, y2 - y1 + 1 };
    }

    void display(const reservoir& res) {
        auto [orig_x, orig_y, wd, hgt] = bounds(res);
        for (int y = 0; y < hgt; ++y) {
            for (int x = 0; x < wd; ++x) {
                point pt = { orig_x + x, orig_y + y };
                char tile = '.';
                if (res.contains(pt)) {
                    switch (res.at(pt)) {
                    case wall:
                        tile = '#';
                        break;
                    case falling_water:
                        tile = '|';
                        break;
                    case standing_water:
                        tile = '~';
                        break;
                    }
                }
                std::print("{}", tile);
            }
            std::println("");
        }
    }
}

void aoc::y2018::day_17(const std::string& title) {

    auto reservoir = make_reservoir(
        aoc::file_to_string_vector(
            aoc::input_path(2018, 17)
        ) | rv::transform(
            parse_command
        ) | r::to<std::vector>()
    );

    display(reservoir);

    std::println("--- Day 17: {} ---", title);
    std::println("  part 1: {}", 0);
    std::println("  part 2: {}", 0);
    
}
