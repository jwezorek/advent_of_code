#include "days.h"
#include "../util.h"
#include <print>
#include <ranges>
#include <numeric>
#include <algorithm>
#include <unordered_map>
#include <sstream>
#include <boost/geometry.hpp>
#include <boost/geometry/algorithms/envelope.hpp>
#include <boost/geometry/geometries/polygon.hpp>
#include <boost/geometry/geometries/register/point.hpp>

namespace r = std::ranges;
namespace rv = std::ranges::views;
namespace bg = boost::geometry;

namespace {
    struct point {
        double x;
        double y;
    };
}

BOOST_GEOMETRY_REGISTER_POINT_2D(::point, double, boost::geometry::cs::cartesian, x, y);

namespace {

    using polygon = boost::geometry::model::polygon<point, true, false>;
    using ring = boost::geometry::model::ring<point, true, false>;

    polygon make_polygon(const ring& outer, const std::vector<ring>& inners) {
        polygon poly;
        poly.outer() = outer;
        poly.inners() = inners;
        bg::correct(poly);
        return poly;
    }

    polygon make_polygon(std::span<const point> verts) {
        auto poly = make_polygon(
            verts | r::to<ring>(),
            {}
        );
        bg::correct(poly);
        return poly;
    }

    polygon buffer(const polygon& poly, double amt) {
        namespace bs = bg::strategy::buffer;
        using dist = bs::distance_symmetric<double>;
        bs::side_straight  side_strategy;
        bs::join_miter   join_strategy;
        bs::end_flat    end_strategy;
        bs::point_square point_strategy;

        boost::geometry::model::multi_polygon<polygon> out;
        bg::buffer(poly, out, dist(amt), side_strategy, join_strategy, end_strategy, point_strategy);
        auto polys = out | r::to<std::vector<polygon>>();

        if (polys.size() != 1) {
            throw std::runtime_error("something is wrong");
        }

        return polys.front();
    }

    struct dig_plan_item {
        char dir;
        int64_t sz;
        std::string color;
    };

    dig_plan_item string_to_dig_plan_item(const std::string& str) {
        auto pieces = aoc::split(str, ' ');
        if (pieces.size() != 3) {
            throw std::runtime_error("something is wrong");
        }
        return {
            pieces[0].front(),
            std::stoi(pieces[1]),
            pieces[2].substr(2, 6)
        };
    }

    std::vector<dig_plan_item> parse_input(const std::vector<std::string>& inp) {
        return inp |
            rv::transform(
                string_to_dig_plan_item
            ) | r::to<std::vector<dig_plan_item>>();
    }

    point operator+(const point& lhs, const point& rhs) {
        return {
            lhs.x + rhs.x,
            lhs.y + rhs.y
        };
    }

    point operator-(const point& lhs, const point& rhs) {
        return {
            lhs.x - rhs.x,
            lhs.y - rhs.y
        };
    }

    point operator*(int64_t k, const point& rhs) {
        return { k * rhs.x, k * rhs.y };
    }

    point dir_to_offset(char dir) {
        static const std::unordered_map<char, point> offsets = {
            {'U', {0,-1}},
            {'L', {-1,0}},
            {'D', {0,1}},
            {'R', {1,0}}
        };
        return offsets.at(dir);
    }

    point line_segment_from_dig_plan(const point& from, char dir, int64_t sz) {
        return from + sz * dir_to_offset(dir);
    }

    std::vector<point> dig_plan_to_verts(const std::vector<dig_plan_item>& dig_plan) {
        std::vector<point> poly = { {0,0} };
        for (const auto& itm : dig_plan) {
            poly.push_back(
                line_segment_from_dig_plan(poly.back(), itm.dir, itm.sz)
            );
        }
        return poly;
    }

    unsigned int hex_to_num(const std::string& str) {
        std::string s = "0x" + str;
        return std::stoul(s, nullptr, 16);
    }

    dig_plan_item make_part2_dig_plan_item(const dig_plan_item& itm) {
        static const std::unordered_map<char, char> dir_tbl = {
            {'0' , 'R'},
            {'1' , 'D'},
            {'2' , 'L'},
            {'3' , 'U'}
        };
        return {
            dir_tbl.at(itm.color.back()),
            static_cast<int64_t>(hex_to_num(itm.color.substr(0,5))),
            ""
        };
    }

    std::vector<point> vertices_for_part2(const std::vector<dig_plan_item>& dig_plan) {
        auto new_dig_plan = dig_plan | rv::transform(
                make_part2_dig_plan_item
            ) | r::to< std::vector<dig_plan_item>>();
        return dig_plan_to_verts(new_dig_plan);
    }
}

/*------------------------------------------------------------------------------------------------*/

void aoc::y2023::day_18(const std::string& title) {

    auto dig_plan = parse_input(aoc::file_to_string_vector(aoc::input_path(2023, 18)));

    std::println("--- Day 18: {} ---\n", title);
    std::println("  part 1: {}",
        bg::area(
            buffer(
                make_polygon(dig_plan_to_verts(dig_plan)),
                0.5
            )
        )
    );

    std::println("  part 2: {}",
        bg::area(
            buffer(
                make_polygon(vertices_for_part2(dig_plan)),
                0.5
            )
        )
    );
}