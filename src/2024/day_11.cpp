
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
        int half = static_cast<int>(str.size()) / 2;
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

        count_map num_to_count;
        for (auto v : inp) {
            ++num_to_count[v];
        }

        for (int i = 0; i < n; ++i) {
            count_map next_num_to_count;
            for (auto [num, count] : num_to_count) {
                auto next = do_one_blink(num);
                for (auto new_num : next | rv::filter([](auto v) {return v >= 0; })) {
                    next_num_to_count[new_num] += count;
                }
            }
            num_to_count = std::move(next_num_to_count);
        }

        return r::fold_left(
            num_to_count | rv::values,
            0ll,
            std::plus<int64_t>()
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
