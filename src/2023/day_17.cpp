#include "days.h"
#include "../util.h"
#include <print>
#include <ranges>
#include <numeric>
#include <algorithm>
#include <limits>
#include <stack>
#include <unordered_set>
#include <unordered_map>
#include <boost/functional/hash.hpp>

namespace r = std::ranges;
namespace rv = std::ranges::views;

namespace {

    struct loc {
        int col;
        int row;
    };

    loc operator+(const loc& lhs, const loc& rhs) {
        return {
            lhs.col + rhs.col,
            lhs.row + rhs.row
        };
    }

    bool operator==(const loc& lhs, const loc& rhs) {
        return lhs.col == rhs.col && lhs.row == rhs.row;
    }

    enum direction {
        north = 0,
        west,
        south,
        east
    };

    struct state {
        loc pos;
        direction dir;
        int steps_in_dir;
    };

    bool operator==(const state& lhs, const state& rhs) {
        return lhs.pos == rhs.pos && lhs.dir == rhs.dir && lhs.steps_in_dir == rhs.steps_in_dir;
    }

    struct state_hash {
        size_t operator()(const state& s) const {
            size_t seed = 0;
            boost::hash_combine(seed, s.pos.col);
            boost::hash_combine(seed, s.pos.row);
            boost::hash_combine(seed, s.dir);
            boost::hash_combine(seed, s.steps_in_dir);
            return seed;
        }
    };

    using grid = std::vector<std::vector<int>>;

    grid init_grid(std::tuple<int, int> dim, int val) {
        auto [cols, rows] = dim;
        return grid(
            rows,
            std::vector<int>(cols, val)
        );
    }

    std::tuple<int, int> dimensions(const grid& g) {
        return {
            static_cast<int>(g.front().size()),
            static_cast<int>(g.size())
        };
    }

    direction left_of(direction dir) {
        auto index = (static_cast<int>(dir) + 1) % 4;
        return static_cast<direction>(index);
    }

    direction right_of(direction dir) {
        auto index = static_cast<int>(dir) - 1;
        index = (index == -1) ? 3 : index;
        return static_cast<direction>(index);
    }

    std::vector<direction> respect_bounds(const std::vector<direction>& dirs, loc pos, int cols, int rows) {
        return dirs |
            rv::filter(
                [&](direction dir)->bool {
                    if (dir == north && pos.row == 0) {
                        return false;
                    }
                    if (dir == west && pos.col == 0) {
                        return false;
                    }
                    if (dir == south && pos.row == rows-1) {
                        return false;
                    }
                    if (dir == east && pos.col == cols-1) {
                        return false;
                    }
                    return true;
                }
            ) | r::to<std::vector<direction>>();
    }

    loc direction_to_offset(direction dir) {
        static const std::array<loc, 4> offsets = { {
            {0,-1}, // north
            {-1,0}, // west
            {0, 1}, // south
            {1, 0}  // east
        } };
        return offsets[dir];
    }

    std::vector<direction> get_allowed_directions(const state& s, int cols, int rows) {
        if (s.steps_in_dir == 0) {
            // this only happens with the initial state
            return respect_bounds( {north,west,south,east}, s.pos, cols, rows);
        }
        std::vector<direction> allowed = { left_of(s.dir), right_of(s.dir) };

        if (s.steps_in_dir < 3) {
            allowed.push_back(s.dir);
        }

        return respect_bounds(allowed, s.pos, cols, rows);
    }

    using neighbors_fn = std::function<std::vector<state>(const state&, int, int)>;

    std::vector<state> directions_to_states(const std::vector<direction>& directions, const state& s) {
        return directions | rv::transform(
            [&](auto dir)->state {
                auto new_steps = (dir == s.dir) ? s.steps_in_dir + 1 : 1;
                return {
                    s.pos + direction_to_offset(dir),
                    dir,
                    new_steps
                };
            }
        ) | r::to<std::vector<state>>();
    }

    std::vector<state> part1_neighbors(const state& state, int cols, int rows) {
        return directions_to_states(
            get_allowed_directions(state, cols, rows),
            state
        );
    }

    std::vector<direction> get_allowed_directions_part2(const state& s, int cols, int rows) {
        if (s.steps_in_dir == 0) {
            // this only happens with the initial state
            return respect_bounds({ north,west,south,east }, s.pos, cols, rows);
        }
        if (s.steps_in_dir < 4) {
            return respect_bounds({ s.dir }, s.pos, cols, rows);
        }

        std::vector<direction> allowed = { left_of(s.dir), right_of(s.dir) };

        if (s.steps_in_dir < 10) {
            allowed.push_back(s.dir);
        }

        return respect_bounds(allowed, s.pos, cols, rows);
    }

    std::vector<state> part2_neighbors(const state& state, int cols, int rows) {
        return directions_to_states(
            get_allowed_directions_part2(state, cols, rows),
            state
        );
    }

    int dijkstra_shortest_path(const grid& g, const loc& start, const loc& end, neighbors_fn neigh_fn) {
        auto [cols, rows] = dimensions(g);

        std::unordered_map<state, int, state_hash> dist;
        aoc::priority_queue<state, state_hash> queue;
        state start_state = { start, north, 0 };
        queue.insert(start_state, 0);
        dist[start_state] = 0;

        while (!queue.empty()) {
            auto u = queue.extract_min();
            for (auto v : neigh_fn(u, cols, rows)) {
                auto dist_to_u = dist.at(u);
                auto dist_through_u_to_v = dist_to_u + g[v.pos.row][v.pos.col];
                auto curr_dist_to_v = dist.contains(v) ? dist.at(v) : std::numeric_limits<int>::max();

                if (dist_through_u_to_v < curr_dist_to_v) {
                    dist[v] = dist_through_u_to_v;
                    if (queue.contains(v)) {
                        queue.change_priority(v, dist_through_u_to_v);
                    } else {
                        queue.insert(v, dist_through_u_to_v);
                    }
                }
            }
        }

        return r::min(
            dist | rv::filter(
                [end](auto&& key_val)->bool {
                    const auto& [s, dist] = key_val;
                    return s.pos == end;
                }
            ) | rv::transform(
                [](auto&& key_val)->int {
                    const auto& [s, dist] = key_val;
                    return dist;
                }
            )
        );
    }
}

/*------------------------------------------------------------------------------------------------*/

void aoc::y2023::day_17(const std::string& title) {

    auto grid = aoc::strings_to_2D_array_of_digits(
        aoc::file_to_string_vector(aoc::input_path(2023, 17))
    );

    auto [cols, rows] = dimensions(grid);

    std::println("--- Day 17: {} ---\n", title);
    std::println("  part 1: {}", 
        dijkstra_shortest_path(grid, { 0,0 }, { cols-1,rows-1}, part1_neighbors)
    );
    std::println("  part 2: {}",
        dijkstra_shortest_path(grid, { 0,0 }, { cols - 1,rows - 1 }, part2_neighbors)
    );

}