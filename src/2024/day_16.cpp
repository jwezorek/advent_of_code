
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

    struct edge {
        direction from_dir;
        direction to_dir;
        point dest;
        int dist;
    };

    using maze_graph = point_map<std::vector<edge>>;

    struct maze {
        point start;
        point end;
        maze_graph graph;
    };

    auto corner_walls(const std::vector<std::string>& maze, const point& pt, int wd, int hgt) {
        
        static const std::array<point, 4> deltas = {{ 
            {-1,-1},{1,-1},{1,1},{-1,1} 
        }};

        return deltas | rv::transform(
            [pt](auto&& delta) {
                return pt + delta;
            }
        ) | rv::filter(
            [&maze, wd, hgt](auto&& loc) {
                if (loc.x < 0 || loc.y < 0 || loc.x >= wd || loc.y >= hgt) {
                    return false;
                }
                return maze[loc.y][loc.x] == '#';
            }
        );
    }

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

    auto adjacent(const std::vector<std::string>& maze, const point& pt, int wd, int hgt) {
        return directions() | rv::transform(
            dir_to_delta
        ) | rv::transform(
            [pt](auto&& delta) {
                return pt + delta;
            }
        ) | rv::filter(
            [&maze, wd, hgt](auto&& loc) {
                if (loc.x < 0 || loc.y < 0 || loc.x >= wd || loc.y >= hgt) {
                    return false;
                }
                return maze[loc.y][loc.x] != '#';
            }
        );
    }

    bool is_node(const std::vector<std::string>& maze, const point& pt, int wd, int hgt) {
        auto num_corner_walls = r::distance(corner_walls(maze, pt, wd, hgt));
        if (num_corner_walls != 4) {
            return false;
        }

        auto tile = maze[pt.y][pt.x];
        if (tile == 'S' || tile == 'E') {
            return true;
        }

        auto adj_count = r::distance(adjacent(maze, pt, wd, hgt));
        return adj_count == 1 || adj_count > 2;
    }

    std::tuple<std::vector<point>,point,point> find_nodes(const std::vector<std::string>& maze) {
        int wd = static_cast<int>(maze[0].size());
        int hgt = static_cast<int>(maze.size());

        point start;
        point end;

        auto nodes = rv::cartesian_product(rv::iota(0, wd), rv::iota(0, hgt)) |
            rv::transform(
                [&](auto&& tup)->point {
                    auto [x, y] = tup;
                    if (maze[y][x] == 'S') {
                        start = { x,y };
                    } else if (maze[y][x] == 'E') {
                        end = { x,y };
                    }
                    return { x,y };
                }
            ) | rv::filter(
                [&](auto&& pt) {
                    return is_node(maze, pt, wd, hgt);
                }
            ) | r::to<std::vector>();

        return { std::move(nodes), start, end };
    }

    std::tuple<point, int, direction> path_to_node(
            const point& loc, const point& prev, direction prev_dir, 
            const std::vector<std::string>& grid, const point_set& nodes, int dist) {
        if (nodes.contains(loc)) {
            return { loc, dist, prev_dir };
        }
        for (auto next_dir : directions()) {
            auto next_loc = loc + dir_to_delta(next_dir);
            if (grid[next_loc.y][next_loc.x] == '#' || next_loc == prev) {
                continue;
            }
            auto rotation_cost = 1000 * rotations(prev_dir, next_dir);
            return path_to_node( next_loc, loc, next_dir, grid, nodes, dist + 1 + rotation_cost);
        }
        throw std::runtime_error("this should not happen");
    }

    std::optional<edge> edge_in_direction(
            const point& u, direction from_dir, const std::vector<std::string>& grid,
            const point_set& nodes) {
        auto start = u + dir_to_delta(from_dir);
        if (grid[start.y][start.x] == '#') {
            return {};
        }
        auto [dest, dist, to_dir] = path_to_node(start, u, from_dir, grid, nodes, 1);
        return edge{
            from_dir,
            to_dir,
            dest,
            dist
        };
    }

    maze_graph construct_maze_graph(const std::vector<std::string>& grid, 
            const std::vector<point>& nodes) {
        auto node_set = nodes | r::to<point_set>();
        maze_graph graph;
        for (const auto& node : nodes) {
            for (auto dir : directions()) {
                auto edge = edge_in_direction(node, dir, grid, node_set);
                if (edge) {
                    graph[node].push_back(*edge);
                }
            }
        }
        return graph;
    }

    maze parse_input(const std::vector<std::string>& inp) {
        std::vector<point> nodes;
        maze maze;
        std::tie(nodes, maze.start, maze.end) = find_nodes(inp);
        maze.graph = construct_maze_graph(inp, nodes);
        return maze;
    }

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

    std::vector<std::tuple<state, int>> neighboring_states(const state& u, const maze& g) {
        const auto& edges = g.graph.at(u.loc);
        return edges | rv::transform(
                [&](auto&& edge)->std::tuple<state, int> {
                    return {
                        state{
                            edge.dest,
                            edge.to_dir
                        },
                        1000 * rotations(u.dir, edge.from_dir) + edge.dist
                    };
                }
            ) | r::to<std::vector>();
    }

    int dijkstra_shortest_path(const maze& g) {

        std::unordered_map<state, int, state_hash> dist;
        aoc::priority_queue<state, state_hash> queue;
        state start_state = { g.start, east };
        queue.insert(start_state, 0);
        dist[start_state] = 0;

        while (!queue.empty()) {
            auto u = queue.extract_min();
            for (auto [v, dist_to_v] : neighboring_states(u, g)) {
                auto dist_to_u = dist.at(u);
                auto dist_through_u_to_v = dist_to_u + dist_to_v;
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
                [&](auto&& pair)->int {
                    const state& s = pair.first;
                    return (s.loc == g.end);
                }
            ) | rv::transform(
                [](auto&& pair) {
                    return pair.second;
                }
            )
        );
    }
}

void aoc::y2024::day_16(const std::string& title) {

    auto inp = parse_input(
        aoc::file_to_string_vector(
            aoc::input_path(2024, 16)
        )
    );

    std::println("--- Day 16: {} ---", title);
    std::println("  part 1: {}", dijkstra_shortest_path(inp));
    std::println("  part 2: {}", 0);
    
}
