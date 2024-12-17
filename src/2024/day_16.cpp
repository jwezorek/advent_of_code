
#include "../util/util.h"
#include "../util/vec2.h"
#include "y2024.h"
#include <filesystem>
#include <functional>
#include <print>
#include <ranges>
#include <unordered_set>
#include <unordered_map>
#include <boost/functional/hash.hpp>
#include <queue>

namespace r = std::ranges;
namespace rv = std::ranges::views;

/*------------------------------------------------------------------------------------------------*/

namespace {

    using point = aoc::vec2<int>;
    using point_set = aoc::vec2_set<int>;

    template<typename T>
    using point_map = aoc::vec2_map<int, T>;
    
    enum direction {
        north = 0,
        east,
        south,
        west
    };

    int rotations(direction a, direction b) {
        int diff = b - a;
        diff = (diff + 4) % 4;
        if (diff > 2) {
            diff -= 4;
        }
        return std::abs(diff);
    }

    struct maze {
        point start;
        point end;
        point_set walls;
    };

    struct state {
        point loc;
        direction dir;

        bool operator==(const state& s) const {
            return loc == s.loc && dir == s.dir;
        }
    };

    struct state_hash {
        size_t operator()(const state& s) const {
            size_t seed = aoc::hash_vec2<int>{}(s.loc);
            boost::hash_combine(seed, s.dir);
            return seed;
        }
    };
    using state_set = std::unordered_set<state, state_hash>;
    using dist_map = std::unordered_map<state, int, state_hash>;
    using predecessor_map = std::unordered_map<state, std::vector<state>, state_hash>;

    auto directions() {
        return rv::iota(0, 4) | rv::transform(
            [](auto i) { return static_cast<direction>(i); }
        );
    }

    point dir_to_delta(direction dir) {
        const static std::unordered_map<direction, point> tbl = {
            {north, {0,-1}},
            {east, {1,0}},
            {south, {0,1}},
            {west, {-1,0}}
        };
        return tbl.at(dir);
    }

    direction turn(direction dir, bool left) {
        int index = static_cast<int>(dir) + (left ? -1 : 1);
        return static_cast<direction>((index + 4) % 4);
    }

    direction reverse(direction dir) {
        int index = static_cast<int>(dir) + 2;
        return static_cast<direction>(index % 4);
    }

    maze parse_input(const std::vector<std::string>& inp) {
        maze maze;
        int wd = static_cast<int>(inp[0].size());
        int hgt = static_cast<int>(inp.size());

        for (auto [x, y] : rv::cartesian_product(rv::iota(0, wd), rv::iota(0, hgt))) {
            auto tile = inp[y][x];
            if (tile == '#') {
                maze.walls.insert(point{ x,y });
            } else if (tile == 'S') {
                maze.start = { x,y };
            } else if (tile == 'E') {
                maze.end = { x,y };
            }
        }

        return maze;
    }

    std::vector<std::tuple<state, int>> neighboring_states(const state& u, const maze& maze) {
        auto left = turn(u.dir, true);
        auto right = turn(u.dir, false);
        std::array<std::tuple<state, int>, 3> adj = {{
            {{u.loc + dir_to_delta(left), left}, 1001},
            {{u.loc + dir_to_delta(u.dir), u.dir}, 1},
            {{u.loc + dir_to_delta(right), right}, 1001}
        }};
        return adj | rv::filter(
            [&](auto&& tup) {
                const auto& [s, _] = tup;
                return !maze.walls.contains(s.loc);
            }
        ) | r::to<std::vector>();
    }

    std::tuple<dist_map, predecessor_map> dijkstra_shortest_path(const maze& maze) {
        dist_map dist;
        predecessor_map pred;
        aoc::priority_queue<state, state_hash> queue;
        state start_state = { maze.start, east };
        queue.insert(start_state, 0);
        dist[start_state] = 0;

        while (!queue.empty()) {
            auto u = queue.extract_min();
            for (auto [v, dist_to_v] : neighboring_states(u, maze)) {
                auto dist_to_u = dist.at(u);
                auto dist_through_u_to_v = dist_to_u + dist_to_v;
                auto curr_dist_to_v = dist.contains(v) ? dist.at(v) : std::numeric_limits<int>::max();

                if (dist_through_u_to_v <= curr_dist_to_v) {
                    bool new_pred = dist.contains(v) && dist[v] != dist_through_u_to_v;
                    dist[v] = dist_through_u_to_v;
                    if (new_pred) {
                        pred[v].clear();
                    }
                    pred[v].push_back(u);

                    if (dist_through_u_to_v == curr_dist_to_v) {
                        continue;
                    }

                    if (queue.contains(v)) {
                        queue.change_priority(v, dist_through_u_to_v);
                    } else {
                        queue.insert(v, dist_through_u_to_v);
                    }
                }
            }
        }

        return { std::move(dist), std::move(pred) };
    }

    int shortest_path_from_distance_map(const dist_map& dist, const point& end) {
        return r::min(
            dist | rv::filter(
                [&](auto&& pair)->int {
                    const state& s = pair.first;
                    return (s.loc ==end);
                }
            ) | rv::transform(
                [](auto&& pair) {
                    return pair.second;
                }
            )
        );
    }

    int shortest_path_len(const maze& maze) {
        auto [dist, _] = dijkstra_shortest_path(maze);
        return shortest_path_from_distance_map(dist, maze.end);
    }

    int locations_on_shortest_paths(const maze& maze) {
        std::queue<state> queue;

        auto [dist_map, pred_map] = dijkstra_shortest_path(maze);
        int short_path_len = shortest_path_from_distance_map(dist_map, maze.end);
        auto intial_states = dist_map | rv::filter(
            [&](auto&& pair) {
                return pair.first.loc == maze.end && pair.second == short_path_len;
            }
        ) | rv::keys;

        for (auto init : intial_states) {
            queue.push(init);
        }

        state_set visited;
        while (!queue.empty()) {
            auto curr_state = queue.front();
            queue.pop();

            if (visited.contains(curr_state)) {
                continue;
            }
            visited.insert(curr_state);

            const auto& preds = pred_map[curr_state];
            for (auto pred : preds) {
                queue.push(pred);
            }
        }

        auto shortest_path_locs = visited |
            rv::transform(
                [](auto&& s) {
                    return s.loc;
                }
            ) | r::to<point_set>();

        return static_cast<int>(shortest_path_locs.size());
    }

}

void aoc::y2024::day_16(const std::string& title) {

    auto inp = parse_input(
        aoc::file_to_string_vector(
            aoc::input_path(2024, 16)
        )
    );

    std::println("--- Day 16: {} ---", title);
    std::println("  part 1: {}", shortest_path_len(inp));
    std::println("  part 2: {}", locations_on_shortest_paths(inp));
    
}
