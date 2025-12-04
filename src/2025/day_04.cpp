
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

    auto adjacent_cells(const point& loc) {
        const static std::array<point, 8> directions = {{
            {-1,-1},{0,-1},{1,-1},
            {-1, 0},       {1, 0},
            {-1, 1},{0, 1},{1, 1}
        }};
        return directions | rv::transform(
            [loc](auto&& dir)->point {
                return loc + dir;
            }
        );
    }

    int neighbor_count(const point& loc, const point_set& locs) {
        return r::count_if(
            adjacent_cells(loc),
            [&](const auto& adj) {
                return locs.contains(adj);
            }
        );
    }

    point_set to_point_set(const grid& g) {
        auto [wd, hgt] = bounds(g);
        point_set set;
        for (int y = 0; y < hgt; ++y) {
            for (int x = 0; x < wd; ++x) {
                if (g[y][x] == '@') {
                    set.insert({ x,y });
                }
            }
        }
        return set;
    }

    point_set do_one_generation(const point_set& locs) {
        point_set next_gen;
        for (const auto& loc : locs) {
            if (neighbor_count(loc, locs) >= 4) {
                next_gen.insert(loc);
            }
        }
        return next_gen;
    }

    int do_part_1(const point_set& locs) {
        auto new_locs = do_one_generation(locs);
        return locs.size() - new_locs.size();
    }

    int do_part_2(const point_set& locs) {
        int total_removed = 0;
        auto curr_locs = locs;
        int just_removed = -1;

        while (just_removed != 0) {
            auto new_locs = do_one_generation(curr_locs);
            just_removed = curr_locs.size() - new_locs.size();
            total_removed += just_removed;
            curr_locs = new_locs;
        }

        return total_removed;
    }
}

void aoc::y2025::day_04(const std::string& title) {

    auto inp = to_point_set(
        aoc::file_to_string_vector(
            aoc::input_path(2025, 4)
        )
    );

    std::println("--- Day 4: {} ---", title);
    std::println("  part 1: {}", do_part_1(inp) );
    std::println("  part 2: {}", do_part_2(inp) );
    
}
