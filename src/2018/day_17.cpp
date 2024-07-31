
#include "../util/util.h"
#include "../util/vec2.h"
#include "y2018.h"
#include <filesystem>
#include <functional>
#include <print>
#include <ranges>
#include <unordered_set>
#include <stack>
#include <format>

namespace r = std::ranges;
namespace rv = std::ranges::views;

/*------------------------------------------------------------------------------------------------*/

namespace {

    using point = aoc::vec2<int>;

    enum tile_type {
        empty,
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

    tile_type tile_at(const reservoir& res, const point& pt) {
        if (!res.contains(pt)) {
            return empty;
        }
        return res.at(pt);
    }

    bool is_surface(tile_type tile) {
        return tile == wall || tile == standing_water;
    }

    std::tuple<bool, int> find_horz_surface_edge(
        const reservoir& res, const point& start, const point& dir) {
        auto pt = start;
        while (is_surface(tile_at(res, pt + point{ 0,1 })) && tile_at(res, pt + dir) != wall) {
            pt = pt + dir;
        }
        bool hard_edge = is_surface(tile_at(res, pt + point{ 0,1 })) &&
            tile_at(res, pt + dir) == wall;
        return { hard_edge, pt.x };
    }

    void handle_horz_water_flow(reservoir& res, std::stack<point>& stack) {
        auto start = stack.top();
        stack.pop();
        auto [hard_left_edge, left] = find_horz_surface_edge(res, start, point{ -1,0 });
        auto [hard_right_edge, right] = find_horz_surface_edge(res, start, point{ 1,0 });

        if (hard_left_edge && hard_right_edge) {
            for (int x = left; x <= right; ++x) {
                res[{x, start.y}] = standing_water;
            }
            return;
        }

        for (int x = left; x <= right; ++x) {
            res[{x, start.y}] = falling_water;
        }

        if (!hard_left_edge) {
            stack.push({ left, start.y });
        }

        if (!hard_right_edge) {
            stack.push({ right,start.y });
        }
    }

    void fill_reservoir(reservoir& res) {
        auto [orig_x, orig_y, wd, hgt] = bounds(res);
        auto reservoir_bottom = orig_y + hgt;

        std::stack<point> stack;
        stack.push({ 500,0 });

        while (!stack.empty()) {
            auto current = stack.top();
            auto top_tile = tile_at(res, current);
            auto bottom_pt = current + point{ 0,1 };
            auto bottom_tile = tile_at(res, bottom_pt);

            // water falling on a surface spreads horizontally...
            if (is_surface(bottom_tile)) {
                handle_horz_water_flow(res, stack);
                continue;
            }

            // water falling on empty space falls downward...
            if (bottom_tile == empty) {
                res[current] = falling_water;

                if (bottom_pt.y < reservoir_bottom) {
                    stack.push(bottom_pt);
                }
                else {
                    stack.pop();
                }
                continue;
            }

            // two waterfalls merging together
            if (bottom_tile == falling_water) {
                res[current] = falling_water;
            }

            // otherwise just pop the stack...
            stack.pop();
        }

        // crop vertically to the original bounds...
        res = res | rv::filter(
            [&](auto&& pair) {
                const auto& pt = pair.first;
                return pt.y >= orig_y && pt.y < orig_y + hgt;
            }
        ) | r::to<::reservoir>();
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

    fill_reservoir(reservoir);

    std::println("--- Day 17: {} ---", title);
    std::println("  part 1: {}", r::count_if(
        reservoir | rv::values,
        [](tile_type tile) {
            return tile == standing_water || tile == falling_water;
        }
    ));
    std::println("  part 2: {}", r::count_if(
        reservoir | rv::values,
        [](tile_type tile) {
            return tile == standing_water;
        }
    ));
}
