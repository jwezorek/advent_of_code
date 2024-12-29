
#include "../util/util.h"
#include "../util/vec2.h"
#include "y2016.h"
#include <filesystem>
#include <functional>
#include <print>
#include <ranges>
#include <unordered_set>
#include <queue>
#include <boost/functional/hash.hpp>

namespace r = std::ranges;
namespace rv = std::ranges::views;

/*------------------------------------------------------------------------------------------------*/

namespace {

    using point = aoc::vec2<int>;
    using point_set = aoc::vec2_set<int>;

    struct node {
        point loc;
        int size;
        int used;
        int avail;
        int pcnt;
    };

    node parse_node(const std::string& str) {
        auto v = aoc::extract_numbers(str);
        return {
            {v[0],v[1]},
            v[2],
            v[3],
            v[4],
            v[5]
        };
    }

    int count_viable_pairs(const std::vector<node>& nodes) {
        return r::count_if(
            aoc::two_combinations(nodes),
            [](auto&& pair) {
                const auto [a, b] = pair;
                return (a.used > 0 && a.used <= b.avail) || 
                    (b.used > 0 && b.used <= a.avail);
            }
        );
    }

    struct dimensions {
        int wd;
        int hgt;
    };

    struct grid_info {
        dimensions bounds;
        point_set walls;
        point goal;
        point hole;
    };

    grid_info build_grid(const std::vector<node>& nodes) {
        auto holes = nodes | rv::filter(
                [](auto&& node) {
                    return node.used == 0;
                }
            ) | r::to<std::vector>();
        if (holes.size() != 1) {
            throw std::runtime_error("we assume input with exactly one hole...");
        }

        const auto& hole = holes.front();
        auto hole_capacity = hole.size;
        point_set walls;
        int max_x = 0;
        int max_y = 0;

        for (const auto& node : nodes) {
            max_x = std::max(node.loc.x, max_x);
            max_y = std::max(node.loc.y, max_y);
            if (node.used > hole_capacity) {
                walls.insert(node.loc);
            }
        }

        return {
            {max_x+1, max_y+1},
            std::move(walls),
            {max_x, 0},
            hole.loc
        };
    }

    void display(const grid_info& g) {
        for (auto y = 0; y < g.bounds.hgt; ++y) {
            for (auto x = 0; x < g.bounds.wd; ++x) {
                point pt(x, y);
                char tile = '.';
                if (g.walls.contains(pt)) {
                    tile = '#';
                } else if (pt == g.goal) {
                    tile = 'G';
                } else if (pt == g.hole) {
                    tile = '-';
                }
                std::print("{}", tile);
            }
            std::println("");
        }
        std::println("");
    }

    struct state {
        point hole;
        point goal;

        bool operator==(const state& s) const {
            return hole == s.hole && goal == s.goal;
        }
    };

    struct hash_state {
        size_t operator()(const state& s) const {
            size_t seed = aoc::hash_vec2<int>{}(s.goal);
            boost::hash_combine(seed, aoc::hash_vec2<int>{}(s.hole));
            return seed;
        }
    };

    using state_set = std::unordered_set<state, hash_state>;

    bool in_bounds(const point& loc, const dimensions& bounds) {
        return loc.x >= 0 && loc.y >= 0 && loc.x < bounds.wd && loc.y < bounds.hgt;
    }

    std::vector<state> next_states(const state& s, const point_set& walls, const dimensions& bounds) {
        std::array<point, 4> deltas = {{ {0,-1},{1,0},{0,1},{-1,0} }};
        return deltas | rv::transform(
                [&](const auto& delta)->point {
                    return s.hole + delta;
                }
            ) | rv::filter(
                [&](const auto& new_hole_loc) {
                    if (walls.contains(new_hole_loc)) {
                        return false;
                    }
                    if (!in_bounds(new_hole_loc, bounds)) {
                        return false;
                    }
                    return true;
                }
            ) | rv::transform(
                [&](const auto& new_hole_loc)->state {
                    auto new_goal_loc = (new_hole_loc == s.goal) ? s.hole : s.goal;
                    return {
                        new_hole_loc,
                        new_goal_loc
                    };
                }
            ) | r::to<std::vector>();
    }

    int fewest_steps(const grid_info& grid) {
        const auto& walls = grid.walls;
        const auto& bounds = grid.bounds;
        state_set visited;
        std::queue<std::tuple<state, int>> queue;

        queue.emplace( state{ grid.hole, grid.goal }, 0);
        while (!queue.empty()) {
            auto [curr_state, dist] = queue.front();
            queue.pop();

            if (visited.contains(curr_state)) {
                continue;
            }
            visited.insert(curr_state);

            if (curr_state.goal == point{0, 0}) {
                return dist;
            }

            for (const auto& next_state : next_states(curr_state, walls, bounds)) {
                queue.emplace(next_state, dist + 1);
            }
        }

        return -1;
    }
}

void aoc::y2016::day_22(const std::string& title) {

    auto inp = aoc::file_to_string_vector(
            aoc::input_path(2016, 22)
        ) | rv::drop(2) | rv::transform(
            parse_node
        ) | r::to<std::vector>();

    std::println("--- Day 22: {} ---", title);
    std::println("  part 1: {}", count_viable_pairs(inp) );
    std::println("  part 2: {}", fewest_steps( build_grid(inp)));
    
}
