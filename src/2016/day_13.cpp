
#include "../util/util.h"
#include "../util/vec2.h"
#include "y2016.h"
#include <filesystem>
#include <functional>
#include <print>
#include <ranges>
#include <unordered_set>
#include <queue>
#include <format>

namespace r = std::ranges;
namespace rv = std::ranges::views;

/*------------------------------------------------------------------------------------------------*/

namespace {
    using point = aoc::vec2<int>;
    using point_set = aoc::vec2_set<int>;

    int hamming_weight(int val) {
        int count = 0;
        while (val) {
            count += val & 1; // Add the least significant bit to count
            val >>= 1;        // Right shift the value by 1 bit
        }
        return count;
    }

    bool is_empty_location(int magic_number, const point& loc) {
        auto [x, y] = loc;
        auto val = x * x + 3 * x + 2 * x * y + y + y * y;
        val += magic_number;
        return hamming_weight(val) % 2 == 0;
    }

    enum visit_result {
        continue_traversal,
        terminate_branch,
        terminate_traversal
    };

    using visit_fn = std::function<visit_result(const point&, int)>;

    point_set bfs(int magic_number, const visit_fn& visit) {
        point_set visited;
        point_set walls;
        point_set floor;

        std::queue<std::tuple<point, int>> queue;
        const static std::array<point, 4> deltas = { {
            {0,-1},{1,0},{0,1},{-1,0}
        } };

        queue.emplace(point{ 1,1 }, 0);
        while (!queue.empty()) {
            auto [loc, dist] = queue.front();
            queue.pop();

            if (visited.contains(loc)) {
                continue;
            }

            auto result = visit(loc, dist);
            visited.insert(loc);

            if (result == terminate_traversal) {
                return visited;
            } else if (result == terminate_branch) {
                continue;
            }

            for (auto adj : deltas | rv::transform([&](auto&& d) {return d + loc; })) {
                if (adj.x < 0 || adj.y < 0) {
                    continue;
                }
                if (!floor.contains(adj) && !walls.contains(adj)) {
                    auto is_floor = is_empty_location(magic_number, adj);
                    if (is_floor) {
                        floor.insert(adj);
                    }
                    else {
                        walls.insert(adj);
                    }
                }
                if (floor.contains(adj)) {
                    queue.emplace(adj, dist + 1);
                }
            }
        }
        return visited;
    }

    int shortest_path(int magic_number, const point& dest) {
        int shortest_path_len = -1;
        bfs(magic_number,
            [&](const point& loc, int dist)->visit_result {
                if (loc == dest) {
                    shortest_path_len = dist;
                    return terminate_traversal;
                }
                return continue_traversal;
            }
        );
        return shortest_path_len;
    }

    int reachable_locations(int magic_number, int max_dist) {
        auto visited = bfs(
            magic_number,
            [max_dist](const point& loc, int dist)->visit_result {
                if (dist == max_dist) {
                    return terminate_branch;
                }
                return continue_traversal;
            }
        );
        return static_cast<int>( visited.size() );
    }
}

void aoc::y2016::day_13(const std::string& title) {

    auto magic = std::stoi(
        aoc::file_to_string(
            aoc::input_path(2016, 13)
        )
    );

    std::println("--- Day 13: {} ---", title);
    std::println("  part 1: {}", shortest_path(magic, { 31,39 }));
    std::println("  part 2: {}", reachable_locations(magic, 50));
    
}
