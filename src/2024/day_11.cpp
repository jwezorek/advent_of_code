
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

    std::optional<std::array<int64_t, 2>> split_digits(int64_t stone) {
        auto str = std::to_string(stone);
        if (str.size() % 2 == 1) {
            return {};
        }
        auto half = str.size() / 2;
        return std::array{
            aoc::string_to_int64(str.substr(0,half)),
            aoc::string_to_int64(str.substr(half,half))
        };
    }

    std::array<int64_t,2> do_one_blink(int64_t stone) {
        std::vector<int64_t> next_gen;
        if (stone == 0) {
            return { 1, -1 };
        }
        auto split = split_digits(stone);
        if (split) {
            return *split;
        }
        return { stone * 2024 , -1};
    }

    int64_t count_after_n_blinks(const std::vector<int64_t>& inp, int n) {
        using count_map = std::unordered_map<int64_t, int64_t>;

        count_map count_per_stone_number;
        for (auto v : inp) {
            ++count_per_stone_number[v];
        }

        for (int i = 0; i < n; ++i) {
            count_map next_count_map;
            for (auto [stone_num, count] : count_per_stone_number) {
                auto next_nums = do_one_blink(stone_num);
                for (auto next_num : next_nums | rv::filter([](auto v) {return v >= 0; })) {
                    next_count_map[next_num] += count;
                }
            }
            count_per_stone_number = std::move(next_count_map);
        }

        return r::fold_left( 
            count_per_stone_number | rv::values, 0ll, std::plus<int64_t>() 
        );
    }
}

void aoc::y2024::day_11(const std::string& title) {

    auto inp = aoc::extract_numbers_int64(
        aoc::file_to_string(
            aoc::input_path(2024, 11)
        )
    );

    std::println("--- Day 11: {} ---", title);
    std::println("  part 1: {}", count_after_n_blinks(inp, 25));
    std::println("  part 2: {}", count_after_n_blinks(inp, 75));
    
}
