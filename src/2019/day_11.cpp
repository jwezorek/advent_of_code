#include "../util.h"
#include "y2019.h"
#include "intcode.h"
#include <filesystem>
#include <functional>
#include <print>
#include <ranges>
#include <unordered_set>
#include <unordered_map>
#include <boost/functional/hash.hpp>

namespace r = std::ranges;
namespace rv = std::ranges::views;

/*------------------------------------------------------------------------------------------------*/

namespace {
    struct point {
        int x;
        int y;

        bool operator==(const point& p) const {
            return x == p.x && y == p.y;
        }
    };

    point operator+(const point& lhs, const point& rhs) {
        return  { lhs.x + rhs.x, lhs.y + rhs.y };
    }

    point operator-(const point& lhs, const point& rhs) {
        return  { lhs.x - rhs.x, lhs.y - rhs.y };
    }

    struct point_hash {
        size_t operator()(const point& pt) const {
            size_t seed = 0;
            boost::hash_combine(seed, pt.x);
            boost::hash_combine(seed, pt.y);
            return seed;
        }
    };

    using point_set = std::unordered_set<point, point_hash>;

    enum direction {
        north = 0,
        east,
        south,
        west
    };

    point direction_to_offset(direction dir) {
        static const std::unordered_map<direction, point> tbl = {
            {north,{0, -1}},
            {east, {1, 0}},
            {south,{0, 1}},
            {west ,{-1,0}}
        };
        return tbl.at(dir);
    }

    direction turn_left(direction dir) {
        int left = static_cast<int>(dir) - 1;
        return (left >= 0) ? static_cast<direction>(left) : west;
    }

    direction turn_right(direction dir) {
        int right = static_cast<int>(dir) + 1;
        return static_cast<direction>(right % 4);
    }

    std::optional<std::tuple<bool, bool>> run_robot_once(aoc::intcode_computer& robot, 
            bool on_white_tile) {
        
        auto result = robot.run_until_event(on_white_tile ? 1 : 0);
        if (result == aoc::terminated) {
            return {};
        }

        result = robot.run_until_event();
        int64_t color_output = robot.output();

        result = robot.run_until_event();
        int64_t dir_output = robot.output();

        return { { color_output == 1, dir_output == 1 } };
    }

    void run_robot(const std::vector<int64_t>& instructions,
            point_set& white_panels,
            std::function<void(const point&)> on_painting) {

        aoc::intcode_computer robot_brain(instructions);
        point loc = { 0,0 };
        direction dir = north;

        std::optional<std::tuple<bool, bool>> result;
        do {
            bool on_white_panel = white_panels.contains(loc);
            result = run_robot_once(robot_brain, on_white_panel);
            if (result) {
                on_painting(loc);
                auto [should_paint_white, is_right_turn] = result.value();
                if (should_paint_white) {
                    white_panels.insert(loc);
                } else {
                    white_panels.erase(loc);
                }
                dir = (is_right_turn) ? turn_right(dir) : turn_left(dir);
                loc = loc + direction_to_offset(dir);
            }
        } while (result);
    }

    std::tuple<int, int, int, int> point_set_bounds(const point_set& set) {
        auto x1 = r::min(set | rv::transform([](auto&& pt) { return pt.x; }));
        auto y1 = r::min(set | rv::transform([](auto&& pt) { return pt.y; }));
        auto x2 = r::max(set | rv::transform([](auto&& pt) { return pt.x; })) + 1;
        auto y2 = r::max(set | rv::transform([](auto&& pt) { return pt.y; })) + 1;

        int wd = x2 - x1;
        int hgt = y2 - y1;

        return { x1, y1, wd, hgt };
    }

    int panels_painted_at_least_once(const std::vector<int64_t>& instructions) {
        point_set white_panels;
        point_set painted_at_least_once;

        run_robot(instructions, white_panels,
            [&](const point& pt) { painted_at_least_once.insert(pt); }
        );

        return static_cast<int>(painted_at_least_once.size());
    }

    void print_registration_identifier(const std::vector<int64_t>& instructions) {
        point_set white_panels;
        point_set test;
        white_panels.insert({ 0,0 });

        run_robot(instructions, white_panels,
            [&](const point& pt) {
                test.insert(pt);
            }
        );

        auto [x, y, wd, hgt] = point_set_bounds( white_panels );
        std::vector<std::string> canvas(hgt, std::string(wd, '.'));
        for (auto&& pt : white_panels) {
            canvas[pt.y - y][pt.x - x] = '#';
        }

        for (auto&& row : canvas) {
            std::print("  ");
            for (auto ch : row) {
                std::print("{}", ch);
            }
            std::println("");
        }
    }
}

void aoc::y2019::day_11(const std::string& title) {

    auto inp = split(
        aoc::file_to_string(aoc::input_path(2019, 11)), ','
    ) | rv::transform(
        [](auto&& str)->int64_t {
            return aoc::string_to_int64(str);
        }
    ) | r::to<std::vector>();

    std::println("--- Day 11: {} ---", title);
    std::println("  part 1: {}",
        panels_painted_at_least_once(inp)
    );
    std::println("  part 2:\n");
    print_registration_identifier(inp);
}