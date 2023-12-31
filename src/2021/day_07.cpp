#include "../util.h"
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
    float median(const std::vector<int>& v) {
        auto nums = v;
        std::sort(nums.begin(), nums.end());

        int n = v.size() / 2;
        if (v.size() % 2 == 0) {
            return (static_cast<float>(nums[n - 1]) + static_cast<float>(nums[n])) / 2.0f;
        }
        else {
            return nums[n];
        }

        return 0;
    }

    int sum_of_weighted_abs_distance(int p, const std::vector<int>& v, std::function<int(int)> weight_fn) {
        return r::fold_left(v | rv::transform(
            [weight_fn, p](int n) -> int {
                return weight_fn(std::abs(n - p));
            }
        ), 0, std::plus<>());
    }

    std::tuple<int, int> find_minima(const std::vector<int>& nums, std::function<int(int)> weight_fn) {
        int max_v = *std::max_element(nums.begin(), nums.end());
        int min = std::numeric_limits<int>::max();
        int loc_of_min;
        for (int i = 0; i <= max_v; ++i) {
            auto used = sum_of_weighted_abs_distance(i, nums, weight_fn);
            if (used < min) {
                min = used;
                loc_of_min = i;
            }
        }
        return { min, loc_of_min };
    }
}

void aoc::y2021::day_07(const std::string& title) {
    auto positions = aoc::extract_numbers(aoc::file_to_string(aoc::input_path(2021, 7)));

    // this one is actually just the median but whatever i wrote the find_minima routine for the other one
    auto [minima_1, loc_1] = find_minima(positions, [](int n) {return n; });
    auto [minima_2, loc_2] = find_minima(positions, [](int n) {return (n * (n + 1)) / 2; });

    std::println("--- Day 7: {} ---", title);
    std::println("  part 1: {}", minima_1);
    std::println("  part 2: {}", minima_2);
}