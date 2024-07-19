#include "../util/util.h"
#include "y2021.h"
#include <filesystem>
#include <functional>
#include <print>
#include <ranges>
#include <unordered_map>
#include <sstream>

namespace r = std::ranges;
namespace rv = std::ranges::views;

/*------------------------------------------------------------------------------------------------*/

namespace {
    int binary_str_to_int(const std::string& str) {
        return std::stoi(str, nullptr, 2);
    }

    int most_common_digit(const std::vector<std::string>& inp, int digit, int tie_val) {
        int n = static_cast<int>(inp.size());
        int ones_count = r::fold_left(
            inp | rv::transform(
                [digit](auto&& str) {
                    return (str[digit] == '1') ? 1 : 0;
                }
            ),
            0,
            std::plus<>()
        );
        int zeros_count = n - ones_count;
        if (ones_count > zeros_count) {
            return 1;
        }
        if (zeros_count > ones_count) {
            return 0;
        }
        return tie_val;
    }

    std::tuple<int,int> common_digits_numbers(const std::vector<std::string>& inp) {
        int n = static_cast<int>(inp.size());
        int num_digits = static_cast<int>(inp.front().size());

        std::stringstream most_common;
        std::stringstream least_common;
        for (int i = 0; i < num_digits; ++i) {
            auto digit = most_common_digit(inp, i, 0);
            most_common << digit;
            least_common << 1 - digit;
        }
        return {
            binary_str_to_int(most_common.str()),
            binary_str_to_int(least_common.str())
        };
    }

    int find_rating(const std::vector<std::string>& inp, bool high) {
        auto set = inp;
        int digit = 0;
        while (set.size() > 1) {
            int mcd = most_common_digit(set, digit, 1);
            int target_val = high ? mcd : 1 - mcd;
            set = set | rv::filter(
                [digit, target_val](const std::string& str) {
                    return std::stoi(str.substr(digit, 1)) == target_val;
                }
            ) | r::to<std::vector<std::string>>();
            ++digit;
        }
        return binary_str_to_int(set.front());
    }
}

void aoc::y2021::day_03(const std::string& title) {
    auto input = aoc::file_to_string_vector(aoc::input_path(2021, 3));
    
    std::println("--- Day 3: {} ---", title);

    auto [most, least] = common_digits_numbers(input);
    std::println("  part 1: {}", most * least);

    std::println("  part 2: {}", 
        find_rating(input, true) * find_rating(input, false)
    );
}

