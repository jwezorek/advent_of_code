
#include "../util/util.h"
#include "../util/vec2.h"
#include "y2025.h"
#include <boost/geometry.hpp>
#include <boost/geometry/index/rtree.hpp>
#include <boost/geometry/geometries/box.hpp>
#include <boost/geometry/geometries/register/point.hpp>
#include <functional>
#include <print>
#include <ranges>
#include <unordered_set>

namespace bg = boost::geometry;
namespace bgi = boost::geometry::index;
namespace r = std::ranges;
namespace rv = std::ranges::views;

/*------------------------------------------------------------------------------------------------*/

namespace {
    using point = aoc::vec2<int64_t>;
}

BOOST_GEOMETRY_REGISTER_POINT_2D(::point, int64_t, bg::cs::cartesian, x, y)

namespace {

    struct edge {
        bool is_horz;          // true = horizontal, false = vertical
        int64_t row_or_col;    // y for horizontal, x for vertical
        int64_t from;          // x for horz, y for vert
        int64_t to;            // x for horz, y for vert
    };

    using box = bg::model::box<point>;
    using edge_value = std::pair<box, edge>;

    using rtree = bgi::rtree<edge_value, bgi::quadratic<16>>;
    using polygon = bg::model::polygon<point, false, true>;

    box to_box(const point& a, const point& b) {
        auto x1 = std::min(a.x, b.x);
        auto x2 = std::max(a.x, b.x);
        auto y1 = std::min(a.y, b.y);
        auto y2 = std::max(a.y, b.y);
        return { point{x1,y1}, point{x2,y2} };
    }

    edge to_edge(const point& u, const point& v) {
        if (u.y == v.y) { // horizontal
            return edge{ true, u.y, u.x, v.x  };
        } else {          // vertical
            return edge{ false, u.x,  u.y, v.y };
        }
    }

    std::array<point, 4> box_corners(const box& b) {
        auto mn = b.min_corner();
        auto mx = b.max_corner();
        return {{
            mn, {mn.x, mx.y}, mx, {mx.x, mn.y}
        }};
    }

    std::array<edge, 4> box_edges(const box& b) {
        auto mn = b.min_corner();
        auto mx = b.max_corner();
        return {{
            to_edge(mn, point{mn.x, mx.y}), to_edge(mx, point{mx.x, mn.y}),
            to_edge(mn, point{mx.x, mn.y}), to_edge(mx, point{mn.x, mx.y})
        }};
    }

    std::optional<box> interior_bounds(const edge& e) {

        auto lo = std::min(e.from, e.to) + 1;
        auto hi = std::max(e.from, e.to) - 1;

        if (hi < lo) {
            return {};
        }

        return (e.is_horz) ?
            box{ point{lo, e.row_or_col}, point{hi, e.row_or_col} }:
            box{ point{ e.row_or_col, lo}, point{ e.row_or_col, hi} };
    }

    class rectilinear_polygon {
        polygon poly_;
        rtree horz_edges_;  // horizontal interior edges of polygon
        rtree vert_edges_;  // vertical interior edges of polygon

        static std::vector<edge> edges(const std::vector<point>& pts) {
            auto verts = pts;
            verts.push_back(pts.front());
            return verts | rv::pairwise | rv::transform(
                    [](auto&& pr) {
                        auto& [u, v] = pr;
                        return to_edge(u, v);
                    }
                ) | r::to<std::vector>();
        }

        bool intersects_edge_of_poly(const edge& e) const {
            auto qb = interior_bounds(e);
            if (!qb) {
                return false; 
            }

            std::vector<edge_value> hits;

            if (e.is_horz) {
                // Horizontal edge intersects vertical edges
                vert_edges_.query(bgi::intersects(*qb), std::back_inserter(hits));
            } else {
                // Vertical edge intersects horizontal edges
                horz_edges_.query(bgi::intersects(*qb), std::back_inserter(hits));
            }

            return !hits.empty();
        }

    public:
        rectilinear_polygon(const std::vector<point>& pts) {
            for (auto& p : pts) {
                poly_.outer().push_back(p);
            }
            poly_.outer().push_back(pts.front());

            for (const auto& e : edges(pts)) {
                if (auto ib = interior_bounds(e)) {
                    if (e.is_horz) {
                        horz_edges_.insert({ *ib, e });
                    } else {
                        vert_edges_.insert({ *ib, e });
                    }
                }
            }
        }

        bool contains(const box& b) const {

            for (auto& c : box_corners(b)) {
                if (!bg::covered_by(c, poly_)) {
                    return false;
                }
            }

            for (auto& e : box_edges(b)) {
                if (intersects_edge_of_poly(e)) {
                    return false;
                }
            }

            return true;
        }
    };

    int64_t largest_area_rectangle(const std::vector<point>& inp) {
        return r::max(
            aoc::two_combinations(inp) | rv::transform(
                [](auto&& pair)->int64_t {
                    const auto& [a, b] = pair;
                    auto diff = a - b;
                    return (std::abs(diff.x) + 1) * (std::abs(diff.y) + 1);
                }
            )
        );
    }

    int64_t largest_area_rectangle_in_poly(const std::vector<point>& inp) {
        rectilinear_polygon poly(inp);
        return r::max(
            aoc::two_combinations(inp) | rv::filter(
                [&](auto&& pair) {
                    const auto& [a, b] = pair;
                    auto box = to_box(a, b);
                    return poly.contains(box);
                }
            ) | rv::transform(
                [](auto&& pair)->int64_t {
                    const auto& [a, b] = pair;
                    auto diff = a - b;
                    return (std::abs(diff.x) + 1) * (std::abs(diff.y) + 1);
                }
            )
        );
    }
}

void aoc::y2025::day_09(const std::string& title) {

    auto inp = aoc::file_to_string_vector(
            aoc::input_path(2025, 9)
        ) | rv::transform(
            [](auto&& str)->point {
                auto v = aoc::extract_numbers_int64(str);
                return { v[0],v[1] };
            }
        ) | r::to<std::vector>();

    std::println("--- Day 9: {} ---", title);
    std::println("  part 1: {}", largest_area_rectangle(inp) );
    std::println("  part 2: {}", largest_area_rectangle_in_poly(inp));
    
}
