
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
    using grid = std::vector<std::string>;
    using point = aoc::vec2<int>;
    using point_set = aoc::vec2_set<int>;

    std::tuple<int, int> bounds(const grid& g) {
        return {
            static_cast<int>(g.front().size()),
            static_cast<int>(g.size())
        };
    }

    bool has_neighbor(const grid& g, const point& loc, int wd, int hgt, char plant) {
        if (loc.x < 0 || loc.y < 0 || loc.x >= wd || loc.y >= hgt) {
            return false;
        }
        if (g[loc.y][loc.x] != plant) {
            return false;
        }
        return true;
    }

    std::vector<point> neighbors(const grid& g, const point& pt, char plant) {
        auto [wd, hgt] = bounds(g);
        const static std::array<point, 4> deltas = {{
            {0,-1},{1,0},{0,1},{-1,0}
        }};
        return deltas | rv::transform(
            [&](auto&& delta) {
                return pt + delta;
            }
        ) | rv::filter(
            [&](auto&& loc) {
                return has_neighbor(g, loc, wd, hgt, plant);
            }
        ) | r::to<std::vector>();
    }

    using perimeter_fn = std::function<int(const grid& g, const point& loc)>;

    int perimeter_of_plot(const grid& g, const point& loc) {
        char plant = g[loc.y][loc.x];
        return  4 - neighbors(g,loc,plant).size();
    }

    int num_sides_per_plot(const grid& g, const point& loc) {
        char plant = g[loc.y][loc.x];
        auto [wd, hgt] = bounds(g);
        auto neighbor = [&](const point& loc)->bool {
            return has_neighbor(g, loc, wd, hgt, plant);
        };

        auto nw = neighbor(loc + point{ -1,-1 });
        auto north = neighbor(loc + point{ 0,-1 });
        auto ne = neighbor(loc + point{ 1,-1 });
        auto east = neighbor(loc + point{ 1,0 });
        auto se = neighbor(loc + point{1, 1});
        auto south = neighbor(loc + point{ 0,1 });
        auto sw = neighbor(loc + point{ -1, 1 });
        auto west = neighbor(loc + point{ -1,0 });

        int corners = 0;

        if (!west && !north) {
            ++corners;
        }

        if (!north && !east) {
            ++corners;
        }

        if (!east && !south) {
            ++corners;
        }

        if (!south && !west) {
            ++corners;
        }

        if (north && east && !ne) {
            ++corners;
        }

        if (east && south && !se) {
            ++corners;
        }

        if (south && west && !sw) {
            ++corners;
        }

        if (west && north && !nw) {
            ++corners;
        }

        return corners;
    }

    int cost_of_fence_for_region(
            const grid& g, const point& start, point_set& visited,
            const perimeter_fn& calc_perim) {
        std::stack<point> stack;
        stack.push(start);
        
        int area = 0;
        int perimeter = 0;
        char plant = g[start.y][start.x];

        while (!stack.empty()) {
            auto curr = stack.top();
            stack.pop();

            if (visited.contains(curr)) {
                continue;
            }
            visited.insert(curr);
            area++;

            auto adj = neighbors(g, curr, plant);
            perimeter += calc_perim(g, curr);

            for (auto neighbor : adj) {
                stack.push(neighbor);
            }
        }

        return area * perimeter;
    }

    int cost_of_fence(const grid& g, const perimeter_fn& calc_perim) {
        auto [wd, hgt] = bounds(g);
        point_set visited;
        int fence_cost = 0;
        for (auto [x, y] : rv::cartesian_product(rv::iota(0, wd), rv::iota(0, hgt))) {
            point loc = { x,y };
            if (visited.contains(loc)) {
                continue;
            }
            fence_cost += cost_of_fence_for_region(g, loc, visited, calc_perim);
        }
        return fence_cost;
    }
}

void aoc::y2024::day_12(const std::string& title) {

    auto garden = aoc::file_to_string_vector(
            aoc::input_path(2024, 12)
        ); 

    std::println("--- Day 12: {} ---", title);
    std::println("  part 1: {}", cost_of_fence( garden, perimeter_of_plot) );
    std::println("  part 2: {}", cost_of_fence( garden, num_sides_per_plot) );
    
}
