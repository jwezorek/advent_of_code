
#include "../util/util.h"
#include "../util/vec2.h"
#include "y2024.h"
#include <filesystem>
#include <functional>
#include <print>
#include <ranges>
#include <unordered_set>
#include <boost/functional/hash.hpp>

namespace r = std::ranges;
namespace rv = std::ranges::views;

/*------------------------------------------------------------------------------------------------*/

namespace {
    using point = aoc::vec2<int>;
    using point_set = aoc::vec2_set<int>;
    using grid = std::vector<std::string>;

    struct state {
        point loc;
        int dir;

        bool operator==(const state& s) const {
            return loc == s.loc && dir == s.dir;
        }
    };

    struct hash_state {
        size_t operator()(const state& s) const {
            size_t seed = aoc::hash_vec2<int>{}(s.loc);
            boost::hash_combine(seed, s.dir);
            return seed;
        }
    };

    using state_set = std::unordered_set<state, hash_state>;

    struct bounds {
        int wd;
        int hgt;
    };

    std::tuple<point, bounds, point_set> parse_input(const grid& grid) {
        bounds bnds{ static_cast<int>(grid.front().size()), static_cast<int>(grid.size()) };
        point start;
        point_set walls;
        for (auto [x, y] : rv::cartesian_product(rv::iota(0, bnds.wd), rv::iota(0, bnds.hgt))) {
            auto pt = point{ x,y };
            auto tile = grid[y][x];
            if (tile == '^') {
                start = pt;
            } else if (tile == '#') {
                walls.insert(pt);
            } 
        }
        return { start, bnds, std::move(walls) };
    }

    bool in_bounds(const point& loc, const bounds& bounds) {
        return loc.x >= 0 && loc.y >= 0 && loc.x < bounds.wd && loc.y < bounds.hgt;
    }

    bool simulate_guard(const point& start, const bounds& bounds, 
            const point_set& walls, std::function<void(const state&)> on_visit = {}) {

        static const std::array<point, 4> dirs = {{
            {0,-1}, {1,0}, {0,1}, {-1,0}
        }};

        state s = { start, 0 };
        state_set visited;

        while (in_bounds(s.loc, bounds) && !visited.contains(s)) {
            visited.insert(s);
            if (on_visit) {
                on_visit(s);
            }
            // if facing a wall, turn right...
            if (walls.contains(s.loc + dirs.at(s.dir))) {
                s.dir = (s.dir + 1) % 4;
                continue;
            }
            s.loc = s.loc + dirs.at(s.dir);
        }

        return visited.contains(s);
    }

    point_set visited_locs(const point& start, const bounds& bounds, const point_set& walls) {
        point_set visited;
        simulate_guard(start, bounds, walls,
            [&visited](const state& s) {
                visited.insert(s.loc);
            }
        );
        return visited;
    }

    int count_visited_locs(
            const point& start, const bounds& bounds, const point_set& walls) {
        return static_cast<int>(
            visited_locs(start, bounds, walls).size()
        );
    }

    int count_loops(const point& start, const bounds& bounds, const point_set& walls) {
        auto floors = visited_locs(start, bounds, walls);
        return r::count_if(
            floors,
            [&](auto&& loc) {
                auto new_walls = walls;
                new_walls.insert(loc);
                return simulate_guard(start, bounds, new_walls);
            }
        );
    }
}

void aoc::y2024::day_06(const std::string& title) {

    auto [start_loc, bounds, walls] = parse_input(
        aoc::file_to_string_vector(
            aoc::input_path(2024, 6)
        )
    );

    std::println("--- Day 6: {} ---", title);
    std::println("  part 1: {}", count_visited_locs(start_loc, bounds, walls));
    std::println("  part 2: {}", count_loops(start_loc, bounds, walls));
    
}
