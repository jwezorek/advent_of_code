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
    struct node {
        std::string label;
        std::vector<int> neighbors;
    };

    using graph = std::unordered_map<int, node>;

    int get_node_or_add_node_to_graph(const point& loc, graph& g, point_map& point_to_node_id, int& curr_id) {

        if (!point_to_node_id.contains(loc)) {
            point_to_node_id[loc] = ++curr_id;
        }
        int id = point_to_node_id.at(loc);

        if (!g.contains(id)) {
            g[id] = node{
                std::format("( {} , {} )", loc.x, loc.y),
                std::vector<int>{}
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

    void link_to_warp(int loc_id, graph& g, const std::vector<std::string>& grid, 
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
        g[loc_id].neighbors.push_back(warp_destination);
        g[warp_destination].neighbors.push_back(loc_id);
    }

    std::tuple<graph, int, int> donut_maze_to_graph(const std::vector<std::string>& grid) {
        graph maze;
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
                        maze[id].neighbors.push_back(adj_id);
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

    int find_shortest_path(const graph& g, int start, int finish) {
        struct state {
            int node;
            int dist;
        };

        std::unordered_set<int> visited;
        std::queue<state> queue;
        queue.emplace(start, 0);
        while (!queue.empty()) {
            auto current = queue.front();
            queue.pop();

            if (visited.contains(current.node)) {
                continue;
            }
            visited.insert(current.node);

            if (current.node == finish) {
                return current.dist;
            }

            for (auto neighbor : g.at(current.node).neighbors) {
                queue.emplace(
                    neighbor,
                    current.dist + 1
                );
            }
        }

        return -1;
    }
}

void aoc::y2019::day_20(const std::string& title) {

    auto grid = aoc::file_to_string_vector(aoc::input_path(2019, 20));
    auto [graph, start, finish] = donut_maze_to_graph(grid);

    std::println("--- Day 20: {} ---", title);
    std::println("  part 1: {}",
        find_shortest_path(graph, start, finish)
    );
    std::println("  part 2: {}",
        0
    );
}