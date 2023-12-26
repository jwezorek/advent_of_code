#include "days.h"
#include "../util.h"
#include <print>
#include <ranges>
#include <numeric>
#include <algorithm>
#include <tuple>
#include <boost/geometry.hpp>
#include <boost/geometry/index/rtree.hpp>
#include <boost/geometry/geometries/box.hpp>
#include <boost/geometry/geometries/register/point.hpp>
#include <unordered_map>
#include <unordered_set>
#include <stack>

namespace bg = boost::geometry;
namespace bgi = boost::geometry::index;
namespace r = std::ranges;
namespace rv = std::ranges::views;

namespace {

    struct point {
        int x;
        int y;
        int z;
    };

}

BOOST_GEOMETRY_REGISTER_POINT_3D(::point, int, bg::cs::cartesian, x, y, z)

namespace {

    using box = bg::model::box<point>;
    using rtree_value = std::pair<box, int>;
    using rtree = bgi::rtree<rtree_value, bgi::rstar<8>>;

    box line_to_brick(const point& u, const point& v) {
        auto extent = [](int v1, int v2) {
            return std::abs(v1 - v2) + 1;
        };
        return {
            {std::min(u.x,v.x), std::min(u.y,v.y), std::min(u.z,v.z)},
            {std::max(u.x,v.x), std::max(u.y,v.y), std::max(u.z,v.z)}
        };
    }

    std::vector<box> parse_input(const std::vector<std::string>& lines) {
        return lines |
            rv::transform(
                [](auto&& str) {
                    auto v = aoc::extract_numbers(str);
                    return line_to_brick({ v[0],v[1],v[2] }, { v[3],v[4],v[5] });
                }
            ) | r::to<std::vector<box>>();
    }

    void place_brick(rtree& rtree, const box& brick, int z, int index) {
        point u = brick.min_corner();
        point v = brick.max_corner();
        int diff = u.z - z;
        u.z = z;
        v.z -= diff;
        rtree.insert({ {u,v}, index });
    }

    std::vector<rtree_value> top_bricks(const std::vector<rtree_value>& bricks) {
        int highest_z = r::max(
            bricks | rv::transform(
                [](auto&& v) {
                    return v.first.max_corner().z;
                }
            )
        );
        return bricks | rv::filter(
            [highest_z](auto&& v) {
                return  v.first.max_corner().z == highest_z;
            }
        ) | r::to<std::vector<rtree_value>>();
    }

    const int k_ground = -1;

    std::vector<int> drop_brick(rtree& rtree, const box& brick, int index) {
        point u = brick.min_corner();
        if (u.z == 1) {
            place_brick(rtree, brick, 1, index);
            return { k_ground };
        }
        point v = brick.max_corner();
        box area_under = { {u.x,u.y,1},{v.x,v.y,u.z - 1} };
        std::vector<rtree_value> bricks_under;
        rtree.query(bgi::intersects(area_under), std::back_inserter(bricks_under));
        if (bricks_under.empty()) {
            place_brick(rtree, brick, 1, index);
            return { k_ground };
        }
        auto top = top_bricks(bricks_under);
        place_brick(rtree, brick, top.front().first.max_corner().z + 1, index);
        return top |
            rv::transform(
                [](auto&& v)->int {
                    return v.second;
                }
        ) | r::to<std::vector<int>>();
    }

    using adjacency_graph = std::unordered_map<int, std::vector<int>>;
    adjacency_graph drop_bricks(const std::vector<box>& bricks) {
        adjacency_graph graph;
        ::rtree rtree;
        for (const auto& [index, brick] : rv::enumerate(bricks)) {
            auto bricks_beneath = drop_brick(rtree, brick, index);
            graph[index] = bricks_beneath;
        }
        return graph;
    }

    int count_removable_bricks(const adjacency_graph& g, int n) {
        std::unordered_set<int> bricks = rv::iota(0, n) | r::to<std::unordered_set<int>>();
        for (const auto& [from, to] : g) {
            if (to.size() == 1) {
                bricks.erase(to.front());
            }
        }
        return bricks.size();
    }

    int do_part_1(const std::vector<box>& bricks) {
        auto graph = drop_bricks(bricks);
        return count_removable_bricks(graph, bricks.size());
    }

    adjacency_graph invert_graph(const adjacency_graph& g) {
        adjacency_graph graph;
        for (const auto [from, to] : g) {
            for (auto j : to) {
                graph[j].push_back(from);
            }
        }
        return graph;
    }

    int count_bricks_connected_to_ground(const adjacency_graph& g) {
        const auto graph = invert_graph(g);
        std::stack<int> stack;
        stack.push(k_ground);
        std::unordered_set<int> visited;
        while (!stack.empty()) {
            int brick = stack.top();
            stack.pop();
            if (visited.contains(brick)) {
                continue;
            }
            if (brick != k_ground) {
                visited.insert(brick);
            }
            if (graph.contains(brick)) {
                for (auto child : graph.at(brick)) {
                    stack.push(child);
                }
            }
        }
        return visited.size();
    }

    int count_bricks_not_connected_to_ground(const adjacency_graph& graph) {
        int n = static_cast<int>(graph.size());
        return n - count_bricks_connected_to_ground(graph);
    }

    int count_dependent_bricks(const adjacency_graph& g, int brick) {
        auto graph = g;
        graph.erase(brick);
        return count_bricks_not_connected_to_ground(graph);
    }

    int do_part_2(const std::vector<box>& bricks) {
        auto graph = drop_bricks(bricks);
        int n = static_cast<int>(bricks.size());
        return r::fold_left(
            rv::iota(0,n) | rv::transform(
                [&](int brink_index) {
                    return count_dependent_bricks(graph, brink_index);
                }
            ),
            0,
            std::plus<>()
        );
    }
}

/*------------------------------------------------------------------------------------------------*/

void aoc::y2023::day_22(const std::string& title) {

    auto bricks = parse_input(
        aoc::file_to_string_vector(aoc::input_path(2023, 22))
    );

    r::sort(bricks,
        [](auto&& lhs, auto&& rhs) {
            return lhs.min_corner().z < rhs.min_corner().z;
        }
    );

    std::println("--- Day 22: {0} ---\n", title);
    std::println("  part 1: {}", do_part_1(bricks));
    std::println("  part 1: {}", do_part_2(bricks));
    
}