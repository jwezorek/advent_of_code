#include <filesystem>
#include <functional>
#include <print>
#include <ranges>
#include <unordered_set>
#include <queue>
#include "../util/util.h"
#include "y2018.h"
#include "../util/vec2.h"

namespace r = std::ranges;
namespace rv = std::ranges::views;

/*------------------------------------------------------------------------------------------------*/

namespace {

    using point = aoc::vec2<int>;
    using point_set = aoc::vec2_set<int>;

    point parse_point(const std::string inp) {
        auto vals = aoc::extract_numbers(inp);
        return {
            vals[0], vals[1]
        };
    }

    int manhattan_distance(const point& u, const point& v) {
        return std::abs(u.x - v.x) + std::abs(u.y - v.y);
    }

    point closest_point(const std::vector<point>& points, const point& u) {
        return r::min(
            points,
            [u](auto&& lhs, auto&& rhs) {
                return manhattan_distance(lhs, u) < manhattan_distance(rhs, u);
            }
        );
    }

    std::tuple<int, int, int, int> bounds(const std::vector<point>& points) {
        auto [min_x, max_x] = r::minmax(
            points | rv::transform([](auto&& pt) {return pt.x; })
        );
        auto [min_y, max_y] = r::minmax(
            points | rv::transform([](auto&& pt) {return pt.y; })
        );
        return { min_x, min_y, max_x, max_y };
    }

    point_set find_infinite_points(const std::vector<point>& points) {
        point_set infinite;
        auto [x1, y1, x2, y2] = bounds(points);
        for (int x = x1; x <= x2; ++x) {
            infinite.insert(closest_point(points, { x,y1 }));
            infinite.insert(closest_point(points, { x,y2 }));
        }
        for (int y = y1; y <= y2; ++y) {
            infinite.insert(closest_point(points, { x1, y }));
            infinite.insert(closest_point(points, { x2, y }));
        }
        return infinite;
    }

    int find_area_size(const std::vector<point>& points, const point& pt) {
        point_set area;
        point_set visited;
        std::queue<point> queue;
        queue.push(pt);
        while (!queue.empty()) {
            auto current = queue.front();
            queue.pop();

            if (visited.contains(current)) {
                continue;
            }
            visited.insert(current);

            auto closest_pt = closest_point(points, current);
            if (closest_pt != pt) {
                continue;
            }

            area.insert(current);
            static const std::array<point, 4> deltas = {{
                {0,-1},{1,0},{0,1},{-1,0}
            }};
            for (auto&& delta : deltas) {
                queue.push(current + delta);
            }
        }
        return area.size();
    }

    int largest_finite_area(const std::vector<point>& points) {
        auto infinite = find_infinite_points(points);
        return r::max(
            points | rv::filter(
                [&](auto&& pt) {
                    return !infinite.contains(pt);
                }
            ) | rv::transform(
                [&](auto&& pt) {
                    return find_area_size(points, pt);
                }
            )
        );
    }

    int safe_area_size(const std::vector<point>& points, int n) {
        auto [x1, y1, x2, y2] = bounds(points);

        return r::count_if(
            rv::cartesian_product(rv::iota(x1, x2 + 1), rv::iota(y1, y2 + 1)),
            [&](auto&& pair)->bool {
                auto [x, y] = pair;
                auto dist_sum = r::fold_left(
                    points | rv::transform(
                        [&](auto&& pt) {
                            return manhattan_distance({ x,y }, pt);
                        }
                    ),
                    0,
                    std::plus<>()
                );
                return dist_sum < n;
            }
        );
    }

}

void aoc::y2018::day_06(const std::string& title) {

    auto points = aoc::file_to_string_vector(
            aoc::input_path(2018, 6)
        ) | rv::transform(
            parse_point
        ) | r::to<std::vector>();

    std::println("--- Day 6: {} ---", title);
    std::println("  part 1: {}", largest_finite_area(points) );
    std::println("  part 2: {}", safe_area_size(points, 10000));
    
}
