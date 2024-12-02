
#include "../util/util.h"
#include "y2024.h"
#include <filesystem>
#include <functional>
#include <print>
#include <ranges>
#include <unordered_set>

namespace r = std::ranges;
namespace rv = std::ranges::views;

/*------------------------------------------------------------------------------------------------*/

namespace {

    template <typename T> int sgn(T val) {
        return (T(0) < val) - (val < T(0));
    }

    bool is_safe(const std::vector<int>& report) {
        auto diffs = report | rv::pairwise | rv::transform(
                [](auto&& pair) {
                    auto [i, j] = pair;
                    return j - i;
                }
            ) | r::to<std::vector>();

        if (!r::all_of(diffs, [s = sgn(diffs.front())](auto v) {return sgn(v) == s; })) {
            return false;
        }
        return r::all_of(diffs, [](auto v) {return std::abs(v) >= 1 && std::abs(v) <= 3; });
    }

    bool is_safe_with_dampener(const std::vector<int>& report) {
        if (is_safe(report)) {
            return true;
        }

        for (int i = 0; i < report.size(); ++i) {
            auto drop_one = rv::enumerate(report) | rv::filter(
                    [i](auto&& pair) {
                        auto [index, val] = pair;
                        return index != i;
                    }
                ) | rv::values | r::to<std::vector>();

            if (is_safe(drop_one)) {
                return true;
            }
        }

        return false;
    }

}

void aoc::y2024::day_02(const std::string& title) {

    auto reports = aoc::file_to_string_vector(
            aoc::input_path( 2024, 2 )
        ) | rv::transform(
            [](auto&& str) { return aoc::extract_numbers(str); }
        ) | r::to<std::vector>();

    std::println("--- Day 2: {} ---", title);
    std::println("  part 1: {}", r::count_if( reports, is_safe) );
    std::println("  part 2: {}", r::count_if( reports, is_safe_with_dampener) );
    
}
