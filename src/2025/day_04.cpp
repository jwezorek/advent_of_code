
#include "../util/util.h"
#include "y2025.h"
#include <filesystem>
#include <functional>
#include <print>
#include <ranges>
#include <unordered_set>
#include "../util/vec2.h"

namespace r = std::ranges;
namespace rv = std::ranges::views;

/*------------------------------------------------------------------------------------------------*/

namespace {
    using point = aoc::vec2<int>;
    using grid = std::vector<std::string>;
    using point_set = aoc::vec2_set<int>;

    std::tuple<int, int> bounds(const grid& g) {
        return {
            static_cast<int>(g.front().size()),
            static_cast<int>(g.size())
        };
    }

    auto adjacent_cells(const point& loc, int wd, int hgt) {
        const static std::array<point, 8> directions = {{
            {-1,-1},{0,-1},{1,-1},
            {-1, 0},       {1, 0},
            {-1, 1},{0, 1},{1, 1}
        }};
        return directions | rv::transform(
                [loc](auto&& dir)->point {
                    return loc + dir;
                }
            ) | rv::filter(
                [wd, hgt](auto&& pt) {
                    return pt.x >= 0 && pt.y >= 0 && pt.x < wd && pt.y < hgt;
                }
            );
    }

    int neighbor_count(const point& loc, const grid& g) {
        auto [wd, hgt] = bounds(g);
        int count = 0;
        for (auto adj : adjacent_cells(loc, wd, hgt)) {
            count += ((g[adj.y][adj.x] == '@') ? 1 : 0);
        }
        return count;
    }

    int do_part_1(const grid& g) {
        auto [wd, hgt] = bounds(g);
        int count = 0;
        for (int y = 0; y < hgt; ++y) {
            for (int x = 0; x < wd; ++x) {
                if (g[y][x] != '@') {
                    continue;
                }
                if (neighbor_count({ x,y }, g) < 4) {
                    ++count;
                } 
            }
        }
        return count;
    }
}

void aoc::y2025::day_04(const std::string& title) {

    auto inp = aoc::file_to_string_vector(
            aoc::input_path(2025, 4)
        ); 


    std::println("--- Day 4: {} ---", title);
    std::println("  part 1: {}", do_part_1(inp) );
    std::println("  part 2: {}", 0);
    
}
