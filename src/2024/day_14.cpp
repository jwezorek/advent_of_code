
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
    using vec = aoc::vec2<int>;
    using vec_set = aoc::vec2_set<int>;

    struct bounds {
        int wd; 
        int hgt;
    };

    struct robot {
        vec pos;
        vec vel;
    };

    robot parse_robot(const std::string& str) {
        auto vals = aoc::extract_numbers(str, true);
        return {
            {vals[0],vals[1]},
            {vals[2],vals[3]}
        };
    }

    int wrap(int v, int dim) {
        if (v >= 0) {
            return v % dim;
        }
        return ((v % dim) + dim) % dim;
    }

    vec wrap(const vec& pt, const bounds& b) {
        return {
            wrap(pt.x, b.wd),
            wrap(pt.y, b.hgt)
        };
    }

    void simulate(const bounds& bounds, std::vector<robot>& robots, int n) {
        for (auto& robot : robots) {
            robot.pos = robot.pos + n * robot.vel;
            robot.pos = wrap(robot.pos, bounds);
        }
    }

    int do_part_1(const bounds& bounds, const std::vector<robot>& inp) {
        auto robots = inp;
        simulate(bounds, robots, 100);
        std::array<std::array<int, 2>, 2> quadrants = { {
            {{0,0}},
            {{0,0}}
        } };
        int half_wd = bounds.wd / 2;
        int half_hgt = bounds.hgt / 2;
        for (auto& robot : robots) {
            auto pt = robot.pos;
            if (pt.x == half_wd || pt.y == half_hgt) {
                continue;
            }
            ++quadrants[pt.y < half_hgt ? 0 : 1][pt.x < half_wd ? 0 : 1];
        }
        return quadrants[0][0] * quadrants[0][1] * quadrants[1][0] * quadrants[1][1];
    }

    int size_of_connected_component(
            vec_set& visited, const vec_set& robots, const vec& start) {
        const static std::array<vec, 8> deltas = { {
            {0,-1},{1,0},{0,1},{-1,0}
        }};
        std::stack<vec> stack;
        int size = 0;

        stack.push(start);
        while (!stack.empty()) {
            auto loc = stack.top();
            stack.pop();

            if (visited.contains(loc)) {
                continue;
            }
            visited.insert(loc);
            ++size;

            for (const auto& delta : deltas) {
                auto next = delta + loc;
                if (!robots.contains(next)) {
                    continue;
                }
                stack.push(next);
            }
        }

        return size;
    }

    int biggest_connected_component(const bounds& bounds, const std::vector<robot>& inp) {
        auto robots = inp | rv::transform(
                [](auto&& robot) {
                    return robot.pos;
                }
            ) | r::to<vec_set>();
        vec_set visited;
        int max_sz = 0;
        for (auto [x, y] : rv::cartesian_product(rv::iota(0, bounds.wd), rv::iota(0, bounds.hgt))) {
            vec pt = { x,y };
            if (robots.contains(pt) && !visited.contains(pt)) {
                max_sz = std::max(max_sz, size_of_connected_component(visited, robots, pt));
            }
        }
        return max_sz;
    }

    void display(const bounds& bounds, const std::vector<robot>& robots) {
        std::vector<std::string> grid(bounds.hgt, std::string(bounds.wd, '.'));
        for (const auto& robot : robots) {
            grid[robot.pos.y][robot.pos.x] = '#';
        }
        for (const auto& row : grid) {
            std::println("{}", row);
        }
    }

    void do_part_2(const bounds& bounds, const std::vector<robot>& inp) {
        auto robots = inp;
        int biggest_cc = 0;
        int iteration = 0;
        do {
            simulate(bounds, robots, 1);
            ++iteration;
            biggest_cc = biggest_connected_component(bounds, robots);
        } while (biggest_cc < 200);

        std::println("after {} seconds...", iteration);
        display(bounds, robots);
    }
}

void aoc::y2024::day_14(const std::string& title) {

    auto inp = aoc::file_to_string_vector(
            aoc::input_path(2024, 14)
        ) | rv::transform(
            parse_robot
        ) | r::to<std::vector>();

    bounds dimensions{ 101,103 };

    std::println("--- Day 14: {} ---", title);
    std::println("  part 1: {}", do_part_1(dimensions, inp) );
    std::println("  part 2:\n");
    do_part_2(dimensions, inp);
    
}
