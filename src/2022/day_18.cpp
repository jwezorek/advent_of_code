#include "../util/util.h"
#include "y2022.h"
#include <filesystem>
#include <functional>
#include <print>
#include <ranges>
#include <unordered_set>
#include <stack>
#include <boost/functional/hash.hpp>

namespace r = std::ranges;
namespace rv = std::ranges::views;

/*------------------------------------------------------------------------------------------------*/


namespace {

    struct point {
        int x;
        int y;
        int z;

        bool operator==(const point& p) const {
            return x == p.x && y == p.y && z == p.z;
        }
    };

    point operator+(const point& lhs, const point& rhs) {
        return  { lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z };
    }

    struct point_hash {
        size_t operator()(const point& pt) const {
            size_t seed = 0;
            boost::hash_combine(seed, pt.x);
            boost::hash_combine(seed, pt.y);
            boost::hash_combine(seed, pt.z);
            return seed;
        }
    };

    using point_set = std::unordered_set<point, point_hash>;

    auto strings_to_points(const std::vector<std::string>& strings) {
        return strings |
            rv::transform(
                [](auto&& str)->point {
                    auto triple = aoc::split(str, ',');
                    return {
                        std::stoi(triple[0]),
                        std::stoi(triple[1]),
                        std::stoi(triple[2])
                    };
                }
        ) | r::to<std::vector<point>>();
    }

    const std::array<point, 6> k_adjacencies = { {
        {0,1,0},  // north
        {1,0,0},  // east
        {0,-1,0}, // south
        {-1,0,0}, // west
        {0,0,1},  // up
        {0,0,-1}, // down
    } };

    int surface_area(const std::vector<point>& points) {
        auto set = points | r::to<point_set>();
        return r::fold_left(
            points |
            rv::transform(
                [&](auto&& pt)->int {
                    return r::fold_left(
                        k_adjacencies |
                        rv::transform(
                            [pt, &set](auto&& adjacency)->int {
                                return set.contains(pt + adjacency) ? 0 : 1;
                            }
                        ),
                        0,
                        std::plus<>()
                    );
                }
            ),
            0,
            std::plus<>()
        );
    }

    using cuboid = std::tuple<point, point>;

    cuboid bounding_volume(const std::vector<point>& points) {
        constexpr auto large = std::numeric_limits<int>::max();
        point min = { large, large, large };
        point max = { -large, -large, -large };
        for (auto&& pt : points) {
            min.x = (pt.x < min.x) ? pt.x : min.x;
            min.y = (pt.y < min.y) ? pt.y : min.y;
            min.z = (pt.z < min.z) ? pt.z : min.z;
            max.x = (pt.x > max.x) ? pt.x : max.x;
            max.y = (pt.y > max.y) ? pt.y : max.y;
            max.z = (pt.z > max.z) ? pt.z : max.z;
        }
        return { min, max };
    }

    int surface_area(const cuboid& c) {
        const auto& [min, max] = c;
        auto x = max.x - min.x + 1;
        auto y = max.y - min.y + 1;
        auto z = max.z - min.z + 1;
        return 2 * (x * y) + 2 * (x * z) + 2 * (y * z);
    }

    cuboid inflate(const cuboid& c) {
        const auto& [min, max] = c;
        return {
            min + point{-1,-1,-1},
            max + point{1,1,1}
        };
    }

    bool in_cuboid(const cuboid& c, const point& pt) {
        const auto& [min, max] = c;
        return pt.x >= min.x && pt.x <= max.x &&
            pt.y >= min.y && pt.y <= max.y &&
            pt.z >= min.z && pt.z <= max.z;
    }

    std::vector<point> exterior_region(const std::vector<point>& points, const cuboid& bounds) {
        point_set region = points | r::to<point_set>();
        point_set visited;
        std::stack<point> stack;

        stack.push({ 0,0,0 });
        while (!stack.empty()) {
            auto pt = stack.top();
            stack.pop();

            if (visited.contains(pt)) {
                continue;
            }
            visited.insert(pt);

            auto neighbors = k_adjacencies |
                rv::transform([&](auto&& adj) {return  pt + adj; }) |
                rv::filter(
                    [&](auto&& v) {
                        return in_cuboid(bounds, v) && !region.contains(v); 
                    }
                );

            for (const auto& neighbor : neighbors) {
                stack.push(neighbor);
            }
        }

        return visited | r::to<std::vector<point>>();
    }

    int exterior_surface_area(const std::vector<point>& points) {
        auto bounds = inflate(bounding_volume(points));
        auto exterior = exterior_region(points, bounds);
        return surface_area(exterior) - surface_area(bounds);
    }
}

void aoc::y2022::day_18(const std::string& title) {
    auto input = file_to_string_vector(input_path(2022, 18));
    auto points = strings_to_points(input);

    std::println("--- Day 18: {} ---", title);
    std::println("  part 1: {}",
        surface_area(points)
    );
    std::println("  part 2: {}",
        exterior_surface_area(points)
    );
}