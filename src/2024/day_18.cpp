
#include "../util/util.h"
#include "../util/vec2.h"
#include "y2024.h"
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

    point parse_point(const std::string& str) {
        auto v = aoc::extract_numbers(str);
        return { v[0],v[1] };
    }

    int shortest_path_len(const std::vector<point>& inp, int wd, int hgt) {
        point_set visited;
        auto walls = inp | r::to<point_set>();
        auto target = point{ wd - 1,hgt - 1 };
        std::queue<std::tuple<point, int>> queue;
        const static std::array<point, 4> deltas = { {
            {0,-1},{1,0},{0,1},{-1,0}
        } };

        queue.emplace(point{ 0,0 }, 0);
        while (!queue.empty()) {
            auto [loc, dist] = queue.front();
            queue.pop();

            if (loc == target) {
                return dist;
            }

            if (visited.contains(loc)) {
                continue;
            }
            visited.insert(loc);

            for (auto adj : deltas | rv::transform([&](auto&& d) {return d + loc; })) {
                if (walls.contains(adj) || adj.x < 0 || adj.y < 0 || adj.x >= wd || adj.y >= hgt) {
                    continue;
                }
                queue.emplace(adj, dist + 1);
            }
        }
        return -1;
    }

    std::string first_blocking_byte(const std::vector<point>& inp, int wd, int hgt) {
        auto start_amt = 1024;
        auto bytes = inp | rv::take(start_amt) | r::to<std::vector>();
        for (int n = start_amt; n < inp.size(); ++n) {
            auto new_pt = inp[n];
            bytes.push_back(new_pt);
            if (shortest_path_len(bytes, wd, hgt) == -1) {
                return std::format("{},{}", new_pt.x, new_pt.y);
            }
        }
        return "";
    }
}

void aoc::y2024::day_18(const std::string& title) {

    auto inp = aoc::file_to_string_vector(
            aoc::input_path(2024, 18)
        ) | rv::transform(
            parse_point
        ) | r::to<std::vector>();

    std::println("--- Day 18: {} ---", title);
    std::println("  part 1: {}",
        shortest_path_len(
            inp | rv::take(1024) | r::to<std::vector>(),
            71,
            71
        )
    );
    std::println("  part 2: {}", first_blocking_byte(inp, 71, 71));
    
}
