#include "../util.h"
#include "y2019.h"
#include <filesystem>
#include <functional>
#include <print>
#include <format>
#include <ranges>
#include <unordered_set>
#include <unordered_map>
#include <queue>

#include <boost/functional/hash.hpp>

namespace r = std::ranges;
namespace rv = std::ranges::views;

/*------------------------------------------------------------------------------------------------*/

namespace {

    struct point {
        int x;
        int y;

        bool operator==(const point& p) const {
            return x == p.x && y == p.y;
        }
    };

    point operator+(const point& lhs, const point& rhs) {
        return  { lhs.x + rhs.x, lhs.y + rhs.y };
    }
    struct point_hash {
        size_t operator()(const point& pt) const {
            size_t seed = 0;
            boost::hash_combine(seed, pt.x);
            boost::hash_combine(seed, pt.y);
            return seed;
        }
    };

    using point_map = std::unordered_map<point, int, point_hash>;

    struct passage {
        int dest;
        int level_delta;
    };

    struct node {
        std::string label;
        std::vector<passage> neighbors;
    };

    using donut_maze = std::unordered_map<int, node>;

    int get_node_or_add_node_to_graph(
            const point& loc, donut_maze& g, point_map& point_to_node_id, int& curr_id) {

        if (!point_to_node_id.contains(loc)) {
            point_to_node_id[loc] = ++curr_id;
        }
        int id = point_to_node_id.at(loc);

        if (!g.contains(id)) {
            g[id] = node{
                std::format("( {} , {} )", loc.x, loc.y),
                std::vector<passage>{}
            };
        }

        return id;
    }

    std::string get_warp_label(const point& loc, const point& dir, const std::vector<std::string>& grid) {
        std::string label("  ");
        auto char_1 = loc + dir;
        auto char_2 = char_1 + dir;
        label[0] = grid[char_1.y][char_1.x];
        label[1] = grid[char_2.y][char_2.x];
        if (dir == point{ 0,-1 } || dir == point{ -1,0 }) {
            r::reverse(label);
        }
        return label;
    }

    bool is_on_outer_edge(const std::vector<std::string>& grid, const point& loc) {
        if (loc.x == 2 || loc.y == 2) {
            return true;
        }
        int wd = static_cast<int>(grid.at(0).size());
        int hgt = static_cast<int>(grid.size());
        return (loc.x == wd - 3 || loc.y == hgt - 3);
    }

    void link_to_warp(int loc_id, donut_maze& maze, const std::vector<std::string>& grid,
            const point& loc, const point& dir,
            std::unordered_map<std::string, int>& warp_map,
            int& start_id, int& finish_id) {
        auto warp_label = get_warp_label(loc, dir, grid);
        if (warp_label == "AA") {
            start_id = loc_id;
            return;
        }
        if (warp_label == "ZZ") {
            finish_id = loc_id;
            return;
        }

        if (!warp_map.contains(warp_label)) {
            warp_map[warp_label] = loc_id;
            return;
        }

        int warp_destination = warp_map[warp_label];
        bool goes_up_a_level = is_on_outer_edge(grid, loc);

        maze[loc_id].neighbors.emplace_back(warp_destination, goes_up_a_level ? -1 : 0); 
        maze[warp_destination].neighbors.emplace_back(loc_id, goes_up_a_level ? 1 : -1);
    }

    std::tuple<donut_maze, int, int> grid_to_donut_maze(const std::vector<std::string>& grid) {
        donut_maze maze;
        point_map point_to_node_id;
        std::unordered_map<std::string, int> warp_to_node_id;
        int hgt = static_cast<int>(grid.size());
        int wd = static_cast<int>(grid[0].size());
        static const std::array<point, 4> directions = {{ {0,-1}, {1,0}, {0,1}, {-1,0} }};

        int start = -1;
        int finish = -1;

        int node_id = 0;
        for (int y = 0; y < hgt; ++y) {
            for (int x = 0; x < wd; ++x) {
                if (grid[y][x] != '.') {
                    continue;
                }
                point loc = { x,y };
                int id = get_node_or_add_node_to_graph(loc, maze, point_to_node_id, node_id);
                for (auto dir : directions) {
                    auto adj = loc + dir;
                    if (grid[adj.y][adj.x] == '#') {
                        continue;
                    }
                    if (grid[adj.y][adj.x] == '.') {
                        int adj_id = get_node_or_add_node_to_graph(adj, maze, point_to_node_id, node_id);
                        maze[id].neighbors.emplace_back(adj_id, 0);
                    } else {
                        link_to_warp(id, maze, grid, loc, dir, warp_to_node_id, start, finish);
                    }
                }
                
            }
        }
        return { std::move(maze), start, finish };
    }

    bool is_warp(const node& n) {
        return n.label.size() == 2;
    }

    struct maze_loc {
        int level;
        int node;

        bool operator==(const maze_loc& loc) const {
            return level == loc.level && node == loc.node;
        }
    };

    struct maze_loc_hash {
        size_t operator()(const maze_loc& ml) const {
            size_t seed = 0;
            boost::hash_combine(seed, ml.node);
            boost::hash_combine(seed, ml.level);
            return seed;
        }
    };

    using maze_loc_set = std::unordered_set<maze_loc, maze_loc_hash>;

    struct state {
        maze_loc loc;
        int dist;
    };

    std::vector<state> neighboring_states(
            const state& current, const donut_maze& maze, bool recursive_maze) {

        const auto& adj_list = maze.at(current.loc.node).neighbors;

        if (!recursive_maze) {
            return adj_list |
                rv::transform(
                    [&](auto&& passage)->state {
                        return {
                            current.loc.level,
                            passage.dest,
                            current.dist + 1
                        };
                    }
                ) | r::to<std::vector>();
        }
        return adj_list | 
            rv::filter(
                [&](auto&& passage)->bool {
                    if (current.loc.level > 0) {
                        return true;
                    }
                    return passage.level_delta >= 0;
                }
            ) | rv::transform(
                [&](auto&& passage)->state {
                    return {
                        current.loc.level + passage.level_delta,
                        passage.dest,
                        current.dist + 1
                    };
                }
            ) | r::to<std::vector>();
    }

    int solve_donut_maze(const donut_maze& maze, int start, int finish, bool recursive_maze) {
        
        maze_loc_set visited;
        std::queue<state> queue;
        queue.emplace(maze_loc{ 0, start }, 0);
        while (!queue.empty()) {
            auto current = queue.front();
            queue.pop();

            if (visited.contains(current.loc)) {
                continue;
            }
            visited.insert(current.loc);

            if (current.loc == maze_loc{0, finish}) {
                return current.dist;
            }

            for (const auto& neighbor : neighboring_states(current, maze, recursive_maze)) {
                queue.push(neighbor);
            }
        }

        return -1;
    }
}

void aoc::y2019::day_20(const std::string& title) {

    auto grid = aoc::file_to_string_vector(aoc::input_path(2019, 20, "test2"));
    auto [maze, start, finish] = grid_to_donut_maze(grid);

    std::println("--- Day 20: {} ---", title);
    //std::println("  part 1: {}",
    //    solve_donut_maze(maze, start, finish, false)
    //);
    std::println("  part 2: {}", 
        solve_donut_maze(maze, start, finish, true) 
    );
}