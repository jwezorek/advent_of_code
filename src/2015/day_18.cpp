#include "../util.h"
#include "y2015.h"
#include <filesystem>
#include <functional>
#include <print>
#include <ranges>
#include <unordered_set>

namespace r = std::ranges;
namespace rv = std::ranges::views;

/*------------------------------------------------------------------------------------------------*/

namespace {

    struct vec2 {
        int x;
        int y;

        bool operator==(const vec2& p) const {
            return x == p.x && y == p.y;
        }
    };

    vec2 operator+(const vec2& lhs, const vec2& rhs) {
        return  { lhs.x + rhs.x, lhs.y + rhs.y };
    }

    auto neighborhood_deltas() {
        static const std::array<vec2, 8> deltas = {{
            {-1,-1}, {0,-1}, {1,-1},
            {1, 0},          {1, 1},
            {0, 1},  {-1,1}, {-1,0}
        }};
        return rv::all(deltas);
    }

    using grid = std::vector<std::string>;

    int live_neighbors(const grid& g, const vec2& loc) {
        int wd = g.front().size();
        int hgt = g.size();
        return r::count_if(
            neighborhood_deltas() |
            rv::transform(
                [&](auto&& delta) {
                    return loc + delta;
                }
            ) | rv::filter(
                [&](auto&& pt) {
                    return pt.x >= 0 && pt.x < wd && pt.y >= 0 && pt.y < hgt;
                }
            ),
            [&](auto&& pt) {
                return g[pt.y][pt.x] == '#';
            }
        );
    }

    grid next_generation(const grid& g) {
        int wd = g.front().size();
        int hgt = g.size();
        grid next(hgt, std::string(wd, '.'));
        for (int y = 0; y < hgt; ++y) {
            for (int x = 0; x < wd; ++x) {
                int n = live_neighbors(g, { x,y });
                if (g[y][x] == '#') {
                    if (n == 2 || n == 3) {
                        next[y][x] = '#';
                    }
                } else {
                    if (n == 3) {
                        next[y][x] = '#';
                    }
                }
            }
        }
        return next;
    }

    void munge_grid_for_part_2(grid& g) {
        g[0][0] = '#';
        g[0][99] = '#';
        g[99][0] = '#';
        g[99][99] = '#';
    }

    int alive_after_n_generations(const grid& initial, int n, bool part_2) {
        auto g = initial;
        for (int i = 0; i < n; ++i) {
            if (part_2) {
                munge_grid_for_part_2(g);
            }
            g = next_generation(g);
        }
        if (part_2) {
            munge_grid_for_part_2(g);
        }
        return r::count_if(
            g | rv::join,
            [](char ch) {
                return ch == '#';
            }
        );
    }
}

void aoc::y2015::day_18(const std::string& title) {

    auto grid = aoc::file_to_string_vector(aoc::input_path(2015, 18));

    std::println("--- Day 18: {} ---", title);
    std::println("  part 1: {}",
        alive_after_n_generations(grid, 100, false)
    );
    std::println("  part 2: {}",
        alive_after_n_generations(grid, 100, true)
    );
}