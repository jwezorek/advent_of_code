#include "../util.h"
#include "y2019.h"
#include <filesystem>
#include <functional>
#include <print>
#include <ranges>
#include <unordered_set>
#include <numbers>
#include <boost/geometry.hpp>
#include <boost/geometry/index/rtree.hpp>
#include <boost/geometry/geometries/box.hpp>
#include <boost/geometry/geometries/register/point.hpp>
#include <boost/functional/hash.hpp>

namespace bg = boost::geometry;
namespace bgi = boost::geometry::index;
namespace r = std::ranges;
namespace rv = std::ranges::views;

namespace {

    struct point {
        int x;
        int y;

        bool operator==(const auto& pt) const {
            return x == pt.x && y == pt.y;
        }
    };

}

BOOST_GEOMETRY_REGISTER_POINT_2D(::point, int, bg::cs::cartesian, x, y)

namespace {

    using point_set = bgi::rtree<::point, bgi::quadratic<16>>;
    using segment = bg::model::segment<::point>;

    point operator-(const point& lhs, const point& rhs) {
        return { lhs.x - rhs.x, lhs.y - rhs.y };
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

    point_set grid_to_point_set(const std::vector<std::string>& grid) {
        point_set pts;
        for (const auto& [row_index, row] : rv::enumerate(grid)) {
            for (const auto& [col_index, ch] : rv::enumerate(row)) {
                if (ch == '#') {
                    pts.insert(
                        point{ 
                            static_cast<int>(col_index), 
                            static_cast<int>(row_index)
                        }
                    );
                }
            }
        }
        return pts;
    }

    std::vector<point> query_line_segment(const point_set& points, const point& u, const point& v) {
        std::vector<point> results;
        segment seg{ {3,3},{3,6} };
        points.query(bgi::intersects(segment{u,v}), std::back_inserter(results));
        return results;
    }

    std::vector<point> visible_points_from_point(const point_set& points, const point& pt) {
        std::vector<point> visible;
        for (const auto& candidate_pt : points | rv::filter([pt](auto&& p) {return p != pt; })) {
            std::vector<point> pts_on_line = query_line_segment(points, pt, candidate_pt);
            if (pts_on_line.size() == 2) {
                visible.push_back(candidate_pt);
            }
        }
        return visible;
    }

    std::tuple<point, int> point_with_max_visible_points(const point_set& set) {

        auto points = set | r::to<std::vector>();
        auto visiblity_map = points | rv::transform(
                [&](auto&& pt)->point_map::value_type {
                    auto visible_pts = visible_points_from_point(set, pt);
                    return { pt, static_cast<int>(visible_pts.size()) };
                }
            ) | r::to<point_map>();

        auto iter = r::max_element(
                points,
                [&](auto&& lhs, auto&& rhs)->bool {
                    return visiblity_map.at(lhs) < visiblity_map.at(rhs);
                }
            );
        return { *iter, visiblity_map.at(*iter) };
    }

    double to_relative_angle(const point& pt, const point& origin) {
        const auto pi = std::numbers::pi;
        auto diff = pt - origin;
        auto theta = pi/2.0 - std::atan2(-diff.y, diff.x);
        theta = (theta < 0) ? theta + 2.0 * pi : theta;
        return theta;
    };

    double to_degrees(double radians) {
        return (radians / std::numbers::pi) * 180.0;
    }

    void sort_points_clockwise_relative_to_point(std::vector<point>& pts, const point& pt) {
        r::sort(
            pts,
            [&](const point& u, const point& v)->bool {
                return to_relative_angle(u,pt) < to_relative_angle(v,pt);
            }
        );
    }

    point nth_destroyed_point(const point_set& initial_set, const point& origin, int n) {
        auto set = initial_set;
        std::optional<point> nth_destroyed;
        int num_destroyed = 0;

        while (!nth_destroyed) {
            auto destroyees = visible_points_from_point(set, origin);
            sort_points_clockwise_relative_to_point(destroyees, origin);
            for (auto&& pt : destroyees) {
                set.remove(pt);
                num_destroyed++;
                if (num_destroyed == n) {
                    nth_destroyed = pt;
                    break;
                }
            }
        }

        return *nth_destroyed;
    }
}

/*------------------------------------------------------------------------------------------------*/


void aoc::y2019::day_10(const std::string& title) {

    auto inp = grid_to_point_set(
            aoc::file_to_string_vector(aoc::input_path(2019, 10))
        );

    std::println("--- Day 10: {} ---\n", title);

    auto [location, count] = point_with_max_visible_points(inp);

    std::println("  part 1: {}", count);

    auto two_hundreth_destroyed = nth_destroyed_point(inp, location, 200);
    std::println("  part 2: {}",
        100 * two_hundreth_destroyed.x + two_hundreth_destroyed.y
    );

}