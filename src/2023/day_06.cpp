#include "days.h"
#include "../util.h"
#include <print>
#include <ranges>
#include <numeric>
#include <algorithm>
#include <sstream>

namespace r = std::ranges;
namespace rv = std::ranges::views;

namespace {
    struct race {
        int64_t time;
        int64_t distance;
    };

    std::vector<race> parse_input(const std::vector<std::string> lines) {
        auto time_line = aoc::extract_numbers(lines.front());
        auto dist_line = aoc::extract_numbers(lines.back());
        return rv::zip(time_line, dist_line) |
            rv::transform(
                [](const auto& tup)->race {
                    auto [time, dist] = tup;
                    return { time,dist };
                }
            ) | r::to<std::vector<race>>();
    }

    int64_t calc_ways_to_win(const race& race) {
        auto D = static_cast<double>(race.distance);
        auto T = static_cast<double>(race.time);
        auto determinant = std::sqrt(-4.0 * D + T * T);
        auto u = T / 2.0 - 0.5 * determinant;
        auto v = T / 2.0 + 0.5 * determinant;

        auto dist_fn = [T](double t) {return(T - t) * t; };
        int64_t bottom = dist_fn(std::floor(u)) > D ? 
            static_cast<int64_t>(std::floor(u)) : 
            static_cast<int64_t>(std::floor(u)) + 1;

        int64_t top = dist_fn(std::ceil(v) - 1) > D ?
            static_cast<int64_t>(std::ceil(v) - 1) :
            static_cast<int64_t>(std::ceil(v));

        return top - bottom + 1;
    }

    int do_part_1(const std::vector<race>& races) {
        return r::fold_left(
            races | rv::transform(calc_ways_to_win),
            1, std::multiplies<>()
        );
    }

    race get_part_2_input(const std::vector<race>& races) {
        std::stringstream time;
        std::stringstream dist;
        for (const auto& race : races) {
            time << race.time;
            dist << race.distance;
        }
        return {
            aoc::string_to_int64(time.str()),
            aoc::string_to_int64(dist.str())
        };
    }

    int64_t do_part_2(const std::vector<race>& races) {
        auto race = get_part_2_input(races);
        return calc_ways_to_win(race);
    }
}

/*------------------------------------------------------------------------------------------------*/

void aoc::y2023::day_06(const std::string& title) {

    auto input = parse_input(
        aoc::file_to_string_vector( aoc::input_path(2023, 6) )
    );

    std::println("--- Day 6: {0} ---\n", title);
    std::println("  part 1: {}", do_part_1(input));
    std::println("  part 2: {}", do_part_2(input));
}
