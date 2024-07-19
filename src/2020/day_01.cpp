#include "../util/util.h"
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
    int do_part_1(const std::vector<int>& nums) {
        std::unordered_set<int> num_set = nums | r::to<std::unordered_set<int>>();
        auto num = r::find_if(nums,
            [&num_set](int num) {
                return num_set.contains(2020 - num);
            }
        );
        return *num * (2020 - *num);
    }

    auto combos(const std::vector<int>& nums) {
        int n = static_cast<int>(nums.size());
        return rv::iota(0, n - 1) |
            rv::transform(
                [n](int i) {
                    return rv::iota(i + 1, n) |
                        rv::transform([i](int j) {return std::tuple<int, int>(i, j); });
                }
            ) | rv::join | rv::transform(
            [&nums](auto&& tup)->std::tuple<int, int> {
                auto [i, j] = tup;
                return { nums.at(i), nums.at(j) };
            }
        );
    }

    int do_part_2(const std::vector<int>& nums) {
        std::unordered_set<int> num_set = nums | r::to<std::unordered_set<int>>();
        for (auto [a, b] : combos(nums)) {
            auto c = 2020 - (a + b);
            if (num_set.contains(c)) {
                return a * b * c;
            }
        }
        return -1;
    }
}

void aoc::y2020::day_01(const std::string& title) {

    auto nums = aoc::file_to_string_vector(aoc::input_path(2020, 1)) | rv::transform(
        [](auto&& str) {return std::stoi(str); }
    ) | r::to<std::vector<int>>();

    std::println("--- Day 1: {} ---", title);
    std::println("  part 1: {}", do_part_1(nums) );
    std::println("  part 2: {}", do_part_2(nums) );
}