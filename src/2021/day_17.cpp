#include "../util/util.h"
#include "y2021.h"
#include <filesystem>
#include <functional>
#include <print>
#include <ranges>
#include <unordered_map>

namespace r = std::ranges;
namespace rv = std::ranges::views;

/*------------------------------------------------------------------------------------------------*/

namespace {

    struct rectangle {
        int x1, x2, y1, y2;

        bool contains(int x, int y) const {
            return x >= x1 && x <= x2 && y >= y1 && y <= y2;
        }

        rectangle(const std::vector<int>& v) :
            x1(v[0]), x2(v[1]), y1(v[2]), y2(v[3]) {

        }
    };

    std::tuple<bool, int> take_a_shot(int x_vel, int y_vel, const rectangle& target) {
        int x = 0;
        int y = 0;
        int max_y = std::numeric_limits<int>::min();

        int count = 0;
        while (!target.contains(x, y) && x <= target.x2 && y >= target.y1) {
            x += x_vel;
            y += y_vel;

            if (y > max_y) {
                max_y = y;
            }

            x_vel = (x_vel > 0) ? x_vel - 1 : 0;
            y_vel -= 1;
        }

        return { target.contains(x, y), max_y };
    }

    int find_highest_hit(const rectangle& target) {
        int max_x = target.x2;
        int max_y = std::abs(target.y1);

        int highest_shot = std::numeric_limits<int>::min();
        for (int y_vel = 0; y_vel < max_y; ++y_vel) {
            for (int x_vel = 0; x_vel < max_x; ++x_vel) {
                auto [hit_the_target, height] = take_a_shot(x_vel, y_vel, target);
                if (hit_the_target) {
                    highest_shot = (height > highest_shot) ? height : highest_shot;
                }
            }
        }
        return highest_shot;
    }

    int find_all_hits(const rectangle& target) {

        // make the search space crazily big cuz why not...
        int max_x = 2 * target.x2;
        int max_y = 2 * std::abs(target.y1);
        int min_y = -2 * std::abs(target.y1);

        int count = 0;
        for (int y_vel = min_y; y_vel < max_y; ++y_vel) {
            for (int x_vel = 0; x_vel < max_x; ++x_vel) {
                auto [hit_the_target, _] = take_a_shot(x_vel, y_vel, target);
                if (hit_the_target) {
                    ++count;
                }
            }
        }
        return count;
    }
}

void aoc::y2021::day_17(const std::string& title) {
    auto target = rectangle{
        aoc::extract_numbers(
            aoc::file_to_string(aoc::input_path(2021, 17)),
            true
        )
    };

    std::println("--- Day 17: {} ---", title);
    std::println("  part 1: {}", find_highest_hit(target));
    std::println("  part 2: {}", find_all_hits(target));
}