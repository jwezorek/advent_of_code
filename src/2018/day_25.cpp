
#include "../util/util.h"
#include "../util/vec4.h"
#include "y2018.h"
#include <filesystem>
#include <functional>
#include <print>
#include <ranges>
#include <unordered_set>
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

    using vec4 = aoc::vec4<int>;
    using vec4_set = aoc::vec4_set<int>;
    using point = bg::model::point<int, 4, bg::cs::cartesian>;
    using box = bg::model::box<point>;
    using rtree = bgi::rtree<point, bgi::quadratic<16>>;

    point vec4_to_point(const vec4& v) {
        point pt;
        pt.set<0>(v.w);
        pt.set<1>(v.x);
        pt.set<2>(v.y);
        pt.set<3>(v.z);
        return pt;
    }

    vec4 point_to_vec4(const point& pt) {
        return {
            pt.get<0>(),
            pt.get<1>(),
            pt.get<2>(),
            pt.get<3>()
        };
    }

    rtree make_rtree(const std::vector<vec4>& vecs) {
        rtree tree;
        for (const auto vec : vecs) {
            tree.insert(vec4_to_point(vec));
        }
        return tree;
    }

    vec4 parse_vec4(const std::string& inp) {
        auto vals = aoc::extract_numbers(inp, true);
        return { vals[0] , vals[1], vals[2] , vals[3] };
    }

    box neighborhood(const vec4& pt) {
        auto min_pt = pt - vec4{3, 3, 3, 3};
        auto max_pt = pt + vec4{3, 3, 3, 3};
        return { vec4_to_point(min_pt), vec4_to_point(max_pt) };
    }

    int manhattan_distance(const vec4& pt1, const vec4& pt2) {
        return std::abs(pt1.w - pt2.w) +
            std::abs(pt1.x - pt2.x) +
            std::abs(pt1.y - pt2.y) +
            std::abs(pt1.z - pt2.z);
    }

    void find_constellation(const vec4& start, const rtree& tree, vec4_set& already_used) {
        std::stack<vec4> stack;
        stack.push(start);
        while (!stack.empty()) {
            auto current = stack.top();
            stack.pop();

            if (already_used.contains(current)) {
                continue;
            }
            already_used.insert(current);

            std::vector<point> near_by;
            tree.query(bgi::covered_by(neighborhood(current)), std::back_inserter(near_by));

            for (const auto& near_by_pt : near_by) {
                auto v = point_to_vec4(near_by_pt);
                if (manhattan_distance(current, v) <= 3) {
                    stack.push(v);
                }
            }
        }
    }

    int number_of_constellations(const std::vector<vec4>& vecs) {
        auto tree = make_rtree(vecs);
        vec4_set already_used;
        int constellations = 0;
        for (const auto& vec : vecs) {
            if (!already_used.contains(vec)) {
                find_constellation(vec, tree, already_used);
                constellations++;
            }
        }
        return constellations;
    }
}

void aoc::y2018::day_25(const std::string& title) {

    auto points = aoc::file_to_string_vector(
            aoc::input_path(2018, 25)
        ) | rv::transform(
            parse_vec4
        ) | r::to<std::vector>();

    std::println("--- Day 25: {} ---", title);
    std::println("  part 1: {}", number_of_constellations(points) );
    std::println("  part 2: {}", "<x-mas freebie>");
    
}
