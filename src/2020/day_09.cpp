#include "../util.h"
#include "y2020.h"
#include <filesystem>
#include <functional>
#include <print>
#include <ranges>
#include <unordered_set>

namespace r = std::ranges;
namespace rv = std::ranges::views;

/*------------------------------------------------------------------------------------------------*/

namespace {

    bool is_sum_of_window_number_pair(int target, const std::unordered_multiset<int64_t>& window) {
        for (auto num : window) {
            auto val = target - num;
            if (val == num) {
                continue;
            }
            if (window.contains(val)) {
                return true;
            }
        }
        return false;
    }

    int64_t find_first_invalid_number(const std::vector<int64_t>& nums, int window_sz) {
        auto window = nums | rv::take(window_sz) | r::to<std::unordered_multiset<int64_t>>();
        for (int i = window_sz; i < nums.size(); ++i) {
            auto current_num = nums[i];
            if (!is_sum_of_window_number_pair(current_num, window)) {
                return current_num;
            }
            window.erase(window.find(nums[i-window_sz]));
            window.insert(current_num);
        }
        return -1;
    }

    std::optional<int64_t> find_encryption_weakness(const std::vector<int64_t>& nums, 
            int64_t target, int window_sz) {

        auto windows = nums | rv::slide(window_sz);
        auto iter = r::find_if(windows,
            [target](auto&& window) {
                auto sum = r::fold_left(window, static_cast<int64_t>(0), std::plus<int64_t>());
                return sum == target;
            }
        );
        if (iter != windows.end()) {
            auto solution = *iter | r::to<std::vector<int64_t>>();
            r::sort(solution);
            return solution.front() + solution.back();
        }
        return {};

    }

    int64_t find_encryption_weakness(const std::vector<int64_t>& nums, int64_t target) {
        for (int window_sz = 2; window_sz < nums.size(); ++window_sz) {
            auto encryption_weakness = find_encryption_weakness(nums, target, window_sz);
            if (encryption_weakness) {
                return encryption_weakness.value();
            }
        }
        return -1;
    }
}

void aoc::y2020::day_09(const std::string& title) {
    auto input = aoc::file_to_string_vector(aoc::input_path(2020, 9)) |
        rv::transform([](auto&& str) {return aoc::string_to_int64(str); }) |
        r::to<std::vector<int64_t>>();

    std::println("--- Day 9: {} ---", title);

    int64_t invalid;
    std::println("  part 1: {}", invalid = find_first_invalid_number(input, 25));
    std::println("  part 2: {}", find_encryption_weakness(input, invalid));
}