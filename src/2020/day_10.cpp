#include "../util.h"
#include "y2020.h"
#include <filesystem>
#include <functional>
#include <print>
#include <ranges>
#include <unordered_map>

namespace r = std::ranges;
namespace rv = std::ranges::views;

/*------------------------------------------------------------------------------------------------*/

namespace {
    int count_diffs(const std::vector<int>& nums, int diff) {
        return  r::fold_left(
            nums | rv::pairwise_transform(
                [diff](int lhs, int rhs) {
                    return rhs - lhs == diff ? 1 : 0;
                }
            ),
            0,
            std::plus<>()
        );
    }

    int do_part_1(const std::vector<int>& nums) {
        auto sorted = nums;
        sorted.push_back(r::max(nums) + 3);
        sorted.push_back(0);
        r::sort(sorted);
        return count_diffs(sorted, 1) * count_diffs(sorted, 3);
    }

    int64_t count_adaptors(std::unordered_map<int, int64_t>& memoize, const std::vector<int>& nums, int i) {
        if (memoize.contains(i)) {
            return memoize.at(i);
        }

        int n = static_cast<int>(nums.size());
        int start = nums.at(i);
        auto next = rv::iota(i + 1, n) | rv::take_while(
                [&](int v) { return nums[v] - start <= 3; }
            );

        if (next.empty()) {
            return 1;
        }

        return memoize[i] = r::fold_left(
            next | rv::transform([&](int j) { return count_adaptors(memoize, nums, j); }),
            0,
            std::plus<int64_t>()
        );
    }

    int64_t do_part_2(const std::vector<int>& nums) {
        std::unordered_map<int, int64_t> memoize;
        auto sorted = nums;
        sorted.push_back(r::max(nums) + 3);
        sorted.push_back(0);
        r::sort(sorted);
        return count_adaptors(memoize, sorted, 0);
    }
}

void aoc::y2020::day_10(const std::string& title) {
    auto input = aoc::file_to_string_vector(aoc::input_path(2020, 10)) |
        rv::transform([](auto&& str) {return std::stoi(str); }) | r::to<std::vector<int>>();

    std::println("--- Day 10: {} ---", title);
    std::println("  part 1: {}", do_part_1(input) );
    std::println("  part 2: {}", do_part_2(input) );
}