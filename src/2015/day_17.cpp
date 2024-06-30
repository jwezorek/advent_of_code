#include "../util.h"
#include "y2015.h"
#include <filesystem>
#include <functional>
#include <print>
#include <ranges>
#include <unordered_set>

namespace r = std::ranges;
namespace rv = std::ranges::views;

/*------------------------------------------------------------------------------------------------*/

namespace {
    int do_part_1(const std::vector<int>& sizes) {
        int count = 0;
        uint32_t n = 1 << static_cast<int>(sizes.size());
        for (uint32_t i = 1; i < n; ++i) {
            int sum = 0;
            for (int j = 0; j < sizes.size(); ++j) {
                if (i & (1 << j)) {
                    sum += sizes[j];
                }
            }
            if (sum == 150) {
                ++count;
            }
        }
        return count;
    }

    int hamming_weight(uint32_t x) {
        int count = 0;
        while (x) {
            count += x & 1;
            x >>= 1;
        }
        return count;
    }

    int minimum_necessary(const std::vector<int>& sizes) {
        uint32_t n = 1 << static_cast<int>(sizes.size());
        int min = std::numeric_limits<int>::max();
        for (uint32_t i = 1; i < n; ++i) {
            int sum = 0;
            for (int j = 0; j < sizes.size(); ++j) {
                if (i & (1 << j)) {
                    sum += sizes[j];
                }
            }
            if (sum == 150) {
                int weight = hamming_weight(i);
                min = std::min(weight, min);
            }
        }
        return min;
    }

    int do_part_2(const std::vector<int>& sizes) {
        int count = 0;
        int min_weight = minimum_necessary(sizes);
        uint32_t n = 1 << static_cast<int>(sizes.size());
        for (uint32_t i = 1; i < n; ++i) {
            int sum = 0;
            for (int j = 0; j < sizes.size(); ++j) {
                if (i & (1 << j)) {
                    sum += sizes[j];
                }
            }
            if (sum == 150 && hamming_weight(i) == min_weight) {
                ++count;
            }
        }
        return count;
    }
}

void aoc::y2015::day_17(const std::string& title) {

    auto nums = aoc::extract_numbers(
            aoc::file_to_string(
                aoc::input_path(2015, 17)
            )
        );

    std::println("--- Day 17: {} ---", title);
    std::println("  part 1: {}",
        do_part_1(nums)
    );
    std::println("  part 2: {}",
        do_part_2(nums)
    );
}