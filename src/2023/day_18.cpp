#include "days.h"
#include "../util.h"
#include <print>
#include <ranges>
#include <numeric>
#include <algorithm>
#include <unordered_map>


namespace r = std::ranges;
namespace rv = std::ranges::views;

namespace {

    struct dig_plan_item {
        char dir;
        int sz;
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

    struct point {
        int x;
        int y;
    };

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

    point operator*(int k, const point& rhs) {
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

    point line_segment_from_dig_plan(const point& from, char dir, int sz) {
        return from + sz * dir_to_offset(dir);
    }

    std::vector<point> dig_plan_to_polygon(const std::vector<dig_plan_item>& dig_plan) {
        std::vector<point> poly = { {0,0} };
        for (const auto& itm : dig_plan) {
            poly.push_back(
                line_segment_from_dig_plan(poly.back(), itm.dir, itm.sz)
            );
        }
        return poly;
    }

    std::tuple<point, point> poly_bounds(const std::vector<point>& poly) {
        auto [min_x, max_x] = r::minmax(poly | rv::transform([](auto&& v) {return v.x; }));
        auto [min_y, max_y] = r::minmax(poly | rv::transform([](auto&& v) {return v.y; }));
        return { {min_x,min_y}, {max_x, max_y} };
    }

    int sgn(int val) {
        return (0 < val) - (val < 0);
    }

    point sgn(const point& v) {
        return { sgn(v.x), sgn(v.y) };
    }

    void paint_line(std::vector<std::string>& painting, const point& from, const point& to) {
        auto diff = to - from;
        auto offset = sgn(diff);
        auto sz = std::max(std::abs(diff.x), std::abs(diff.y));
        for (int i = 0; i < sz; ++i) {
            auto pt = from + i * offset;
            painting[pt.y][pt.x] = '#';
        }
    }

    int poly_area(const std::vector<point>& poly) {
        int area = 0;
        int n = static_cast<int>(poly.size());
        for (int i = 0; i < n - 1; ++i) {
            area += poly[i].x * poly[i + 1].y - poly[i + 1].x * poly[i].y;
        }
        area += poly[n-1].x * poly[0].y - poly[0].x * poly[n - 1].y;
        area = abs(area) / 2;
        return area;
    }

    std::vector<std::string> paint(const std::vector<point>& poly) {
        auto [ul, lr] = poly_bounds(poly);
        auto cols = lr.x - ul.x + 1;
        auto rows = lr.y - ul.y + 1;
        std::vector<std::string> painting(rows, std::string(cols, '.'));
        for (const auto& [u, v] : rv::pairwise(poly)) {
            auto from = u - ul;
            auto to = v - ul;
            paint_line(painting, from, to);
        }
        return painting;
    }
}

/*------------------------------------------------------------------------------------------------*/

void aoc::y2023::day_18(const std::string& title) {

    auto dig_plan = parse_input(aoc::file_to_string_vector(aoc::input_path(2023, 18, "test")));

    auto polygon = dig_plan_to_polygon(dig_plan);
    auto painting = paint(polygon);

    std::println("{}", poly_area(polygon));

    for (const auto& row : painting) {
        std::println("{}", row);
    }
    std::println("");

    std::println("--- Day 18: {0} ---\n", title);

}