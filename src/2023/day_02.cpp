#include "days.h"
#include "../util.h"
#include <print>
#include <ranges>
#include <numeric>
#include <algorithm>
#include <regex>

namespace r = std::ranges;
namespace rv = std::ranges::views;

namespace {
    
    using color_count = std::vector<int>;
    struct game_info {
        int id;
        color_count max_color_count;
    };

    color_count color_count_from_tuple(int count, int color) {
        std::vector<int> counts(3, 0);
        counts[color] = count;
        return counts;
    }

    int max_color_count(int color, const std::vector<color_count>& col_counts) {
        return r::max(
            col_counts | 
                rv::transform(
                    [color](const auto& ary) {
                        return ary[color];
                    }
                )
        );
    }

    color_count max_color_counts(const std::vector<color_count>& col_counts) {
        return rv::iota(0,3) |
            rv::transform(
                [&col_counts](auto color) {
                    return max_color_count(color, col_counts);
                }
        ) | r::to<color_count>();
    }

    game_info parse_nums(const std::vector<int>& nums) {
        auto color_counts = nums | rv::drop(1) |
            rv::chunk(2) | rv::transform(
                [](auto rng) {
                    return color_count_from_tuple(rng[0], rng[1]); 
                }
            ) | r::to<std::vector<color_count>>();
        return {
            nums.front(),
            max_color_counts(color_counts)
        };
    }

    game_info parse_line(const std::string& line) {
        auto numeric_str = std::regex_replace(line, std::regex("red"), "0");
        numeric_str = std::regex_replace(numeric_str, std::regex("green"), "1");
        numeric_str = std::regex_replace(numeric_str, std::regex("blue"), "2");
        auto nums = aoc::extract_numbers(numeric_str);
        return parse_nums(nums);
    }

    bool is_possible(const game_info& gi) {
        return gi.max_color_count[0] <= 12 && 
            gi.max_color_count[1] <= 13 && 
            gi.max_color_count[2] <= 14;
    }
}

/*------------------------------------------------------------------------------------------------*/

void aoc::y2023::day_02(const std::string& title) {

    auto input = aoc::file_to_string_vector(aoc::input_path(2023, 2)) |
        rv::transform(parse_line) |
        r::to<std::vector<game_info>>();

    std::println("--- Day 2: {0} ---\n", title);
    
    std::println("  part 1: {0}", 
        r::fold_left(
            input | rv::filter(is_possible) | rv::transform([](auto&& gi) {return gi.id; }), 
            0, std::plus<>()
        )
    );
    std::println("  part 2: {0}",
        r::fold_left(
            input | rv::transform(
                [](auto&& gi) {
                    return r::fold_left(
                        gi.max_color_count,
                        1, std::multiplies<>()
                    );
                }
            ),
            0, std::plus<>()
        )
    );
}