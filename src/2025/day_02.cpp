
#include "../util/util.h"
#include "y2025.h"
#include <filesystem>
#include <functional>
#include <print>
#include <ranges>
#include <algorithm>
#include <functional>

namespace r = std::ranges;
namespace rv = std::ranges::views;

/*------------------------------------------------------------------------------------------------*/

namespace {

    struct id_range {
        int64_t start;
        int64_t end;
    };

    bool all_equal( auto&& r) {
        return r::adjacent_find(r, std::not_equal_to{}) == r.end();
    }

    bool is_invalid_id_part_2(int64_t n) {
        auto str = std::to_string(n);
        for (int i = 1; i <= str.size() / 2; ++i) {
            if (str.size() % i != 0) {
                continue;
            }
            auto pieces = str | rv::chunk(i) | rv::transform(
                [](auto group) {
                    return std::string(group.begin(), group.end());
                }
            );

            if (all_equal(pieces)) {
                return true;
            }
        }

        return false;
    }

    bool is_invalid_id_part_1(int64_t n) {
        auto str = std::to_string(n);
        if (str.size() % 2 != 0) {
            return false;
        }
        auto half_sz = str.size() / 2;
        auto front = str.substr(0, half_sz);
        auto back = str.substr(half_sz, half_sz);
        return front == back;
    }

    using is_invalid_id_fn = std::function<bool(int64_t)>;

    int64_t sum_of_invalid_ids_in_range(const id_range& range, const is_invalid_id_fn& is_invalid) {
        return r::fold_left(
            rv::iota(range.start, range.end + 1) | rv::filter(is_invalid),
            0,
            std::plus<int64_t>()
        );
    }

    int64_t sum_of_invalid_ids(const std::vector<id_range>& ranges, const is_invalid_id_fn& is_invalid) {
        return r::fold_left(
            ranges | rv::transform(
                    [is_invalid](auto&& range) {
                        return sum_of_invalid_ids_in_range(range, is_invalid);
                    }
                ),
            0,
            std::plus<int64_t>()
        );
    }
}

void aoc::y2025::day_02(const std::string& title) {

    auto inp = aoc::split(
            aoc::file_to_string(
                aoc::input_path(2025, 2)
            ),
            ','
        ) | rv::transform(
            [](auto&& line)->id_range {
                auto nums = aoc::extract_numbers_int64(line, false);
                return { nums.front(), nums.back() };
            }
        ) | r::to<std::vector>();

    std::println("--- Day 2: {} ---", title);
    std::println("  part 1: {}", sum_of_invalid_ids(inp, is_invalid_id_part_1));
    std::println("  part 2: {}", sum_of_invalid_ids(inp, is_invalid_id_part_2));
    
}
