#include "../util.h"
#include "y2015.h"
#include <filesystem>
#include <functional>
#include <print>
#include <ranges>
#include <unordered_set>

namespace r = std::ranges;
namespace rv = std::ranges::views;

/*------------------------------------------------------------------------------------------------*/

namespace {
    struct reindeer {
        int speed;
        int flying_duration;
        int rest_duration;
    };

    reindeer str_to_reindeer_stats(const std::string& str) {
        auto nums = aoc::extract_numbers(str);
        return { nums[0], nums[1], nums[2]};
    }

    int distance_traveled(const reindeer& deer, int duration) {
        int complete_periods = duration / (deer.flying_duration + deer.rest_duration);
        int traveled = complete_periods * (deer.speed * deer.flying_duration);
        int leftover_time = duration % (deer.flying_duration + deer.rest_duration);
        int leftover_flying_time = std::min(leftover_time, deer.flying_duration);
        traveled += leftover_flying_time * deer.speed;
        return traveled;
    }

    int winning_reindeer_distance(const std::vector<reindeer>& reindeers, int time) {
        auto distances = reindeers | rv::transform(
                [time](auto&& reindeer) {
                    return distance_traveled(reindeer, time);
                }
            ) | r::to<std::vector>();
        return r::max(distances);
    }

    int distance_increment_at_time(const reindeer& deer, int time) {
        auto time_in_period = time % (deer.flying_duration + deer.rest_duration);
        return time_in_period < deer.flying_duration ? deer.speed : 0;
    }
    
    int winning_reindeer_points(const std::vector<reindeer>& reindeers, int time) {
        std::vector<int> points(reindeers.size(), 0);
        std::vector<int> distance(reindeers.size(), 0);
        for (int i = 0; i < time; ++i) {
            for (int j = 0; j < reindeers.size(); ++j) {
                distance[j] += distance_increment_at_time(reindeers.at(j), i);
            }
            auto current_leading_dist = r::max(distance);
            for (int j = 0; j < reindeers.size(); ++j) {
                if (distance[j] == current_leading_dist) {
                    ++points[j];
                }
            }
        }
        return r::max(points);
    }
}

void aoc::y2015::day_14(const std::string& title) {

    auto inp = aoc::file_to_string_vector(
            aoc::input_path(2015, 14)
        ) | rv::transform(
            str_to_reindeer_stats
        ) | r::to<std::vector>();

    std::println("--- Day 14: {} ---", title);
    std::println("  part 1: {}",
        winning_reindeer_distance(inp, 2503)
    );
    std::println("  part 2: {}",
        winning_reindeer_points(inp, 2503)
    );
}