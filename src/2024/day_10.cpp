
#include "../util/util.h"
#include "../util/vec2.h"
#include "y2024.h"
#include <filesystem>
#include <functional>
#include <print>
#include <ranges>
#include <unordered_set>
#include <stack>

namespace r = std::ranges;
namespace rv = std::ranges::views;

/*------------------------------------------------------------------------------------------------*/

namespace {

    using point = aoc::vec2<int>;
    using grid = std::vector<std::vector<int>>;
    using point_set = aoc::vec2_set<int>;

    auto neighbors(const point& loc, const grid& g) {

        int cols = static_cast<int>(g.front().size());
        int rows = static_cast<int>(g.size());

        static const std::array<point, 4> deltas = { {
            {0,-1}, {1, 0}, {0,1}, {-1,0}
        }};

        int altitude = g[loc.y][loc.x];
        return deltas | rv::transform(
            [loc](auto&& delta) {
                return loc + delta;
            }
        ) | rv::filter(
            [&g, altitude, cols, rows](auto&& pt) {
                if (pt.x < 0 || pt.y < 0 || pt.x >= cols || pt.y >= rows) {
                    return false;
                }
                if (g[pt.y][pt.x] != altitude + 1) {
                    return false;
                }
                return true;
            }

        );
    }

    int score_trailhead(const point& trailhead, const grid& g, bool all_paths) {
        point_set visited;
        std::stack<point> stack;

        int score = 0;

        stack.push(trailhead);
        while (!stack.empty()) {
            auto curr = stack.top();
            stack.pop();

            if (!all_paths) {
                if (visited.contains(curr)) {
                    continue;
                }
                visited.insert(curr);
            }

            if (g[curr.y][curr.x] == 9) {
                ++score;
                continue;
            }

            for (auto neighbor : neighbors(curr, g)) {
                stack.push(neighbor);
            }
        }

        return score;
    }

    int score_all_trailheads(const grid& g, bool all_paths) {
        int cols = static_cast<int>(g.front().size());
        int rows = static_cast<int>(g.size());

        int score = 0;
        for (auto [x, y] : rv::cartesian_product(rv::iota(0,cols), rv::iota(0,rows))) {
            if (g[y][x] == 0) {
                score += score_trailhead({ x,y }, g, all_paths);
            }
        }

        return score;
    }

}

void aoc::y2024::day_10(const std::string& title) {

    auto inp = aoc::strings_to_2D_array_of_digits(
        aoc::file_to_string_vector(
            aoc::input_path(2024, 10)
        )
    );

    std::println("--- Day 10: {} ---", title);
    std::println("  part 1: {}", score_all_trailheads(inp, false) );
    std::println("  part 2: {}", score_all_trailheads(inp, true) );
    
}
