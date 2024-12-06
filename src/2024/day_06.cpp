
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

    std::tuple<point, bounds, point_set, point_set> parse_input(const grid& grid) {
        bounds bounds{ static_cast<int>(grid.front().size()), static_cast<int>(grid.size()) };
        point start;
        point_set walls;
        point_set floors;
        for (auto [x, y] : rv::cartesian_product(rv::iota(0, bounds.wd), rv::iota(0, bounds.hgt))) {
            auto pt = point{ x,y };
            auto tile = grid[y][x];
            if (tile == '^') {
                start = pt;
            } else if (tile == '#') {
                walls.insert(pt);
            } else {
                floors.insert(pt);
            }
        }
        return { start, bounds, std::move(walls), std::move(floors) };
    }

    bool in_bounds(const point& loc, const bounds& bounds) {
        return loc.x >= 0 && loc.y >= 0 && loc.x < bounds.wd && loc.y < bounds.hgt;
    }

    point nth_direction(int n) {
        static const std::array<point, 4> dirs = { {
            {0,-1}, {1,0}, {0,1}, {-1,0}
        } };
        return dirs[n];
    }

    bool is_facing_wall(const point& loc, int dir, const point_set& walls) {
        return walls.contains(loc + nth_direction(dir));
    }

    bool simulate_guard(const point& start, const bounds& bounds, 
        const point_set& walls, std::function<void(const state&)> on_visit = {}) {
        state s = { start, 0 };
        state_set visited;
        while (in_bounds(s.loc, bounds) && !visited.contains(s)) {
            visited.insert(s);
            if (on_visit) {
                on_visit(s);
            }
            if (is_facing_wall(s.loc, s.dir, walls)) {
                s.dir = (s.dir + 1) % 4;
                continue;
            }
            s.loc = s.loc + nth_direction(s.dir);
        }
        return visited.contains(s);
    }

    int count_visited_locs(
            const point& start, const bounds& bounds, const point_set& walls) {
        point_set visited;
        simulate_guard(start, bounds, walls,
            [&visited](const state& s) {
                visited.insert(s.loc);
            }
        );
        return static_cast<int>(visited.size());
    }

    int count_loops(const point& start, const bounds& bounds, 
            const point_set& walls, const point_set& floors) {
        int count = 0;
        for (auto loc : floors) {
            auto new_walls = walls;
            new_walls.insert(loc);
            if (simulate_guard(start, bounds, new_walls)) {
                ++count;
            }
        }
        return count;
    }
}

void aoc::y2024::day_06(const std::string& title) {

    auto [start_loc, bounds, walls, floors] = parse_input(
        aoc::file_to_string_vector(
            aoc::input_path(2024, 6)
        )
    );

    std::println("--- Day 6: {} ---", title);
    std::println("  part 1: {}", count_visited_locs(start_loc, bounds, walls));
    std::println("  part 2: {}", count_loops(start_loc, bounds, walls, floors));
    
}
