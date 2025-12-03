
#include "../util/util.h"
#include "y2025.h"
#include <filesystem>
#include <functional>
#include <print>
#include <ranges>
#include <sstream>

namespace r = std::ranges;
namespace rv = std::ranges::views;

/*------------------------------------------------------------------------------------------------*/

namespace {

    int max_index_leaving_n(const std::string& s, int n) {
        if (s.size() <= n)
            return -1; 

        auto it = std::max_element(s.begin(), s.end() - n);
        return static_cast<int>(it - s.begin());
    }

    std::string highest_n_digit_number_as_str(const std::string& digits, int n) {

        auto start = max_index_leaving_n(digits, n - 1);
        if (n == 1) {
            return std::string(1, digits[start]);
        }
        
        std::string remaining_digits = digits.substr(
            start + 1, digits.size() - start - 1
        );

        std::stringstream ss;
        ss << digits[start];
        ss << highest_n_digit_number_as_str(remaining_digits, n - 1);

        return ss.str();
    }

    int64_t highest_n_digit_number(const std::string& digits, int n) {
        return aoc::string_to_int64(
            highest_n_digit_number_as_str(digits, n)
        );
    }

    int64_t sum_of_highest_n_digit_number(const std::vector<std::string>& digit_banks, int n) {
        return r::fold_left(
            digit_banks | rv::transform(
                [n](auto&& str)->int64_t {
                    return highest_n_digit_number(str, n);
                }
            ),
            0,
            std::plus<int64_t>()
        );
    }

}

void aoc::y2025::day_03(const std::string& title) {

    auto inp = aoc::file_to_string_vector(
            aoc::input_path(2025, 3)
        ); 

    std::println("--- Day 3: {} ---", title);
    std::println("  part 1: {}", sum_of_highest_n_digit_number(inp, 2) );
    std::println("  part 2: {}", sum_of_highest_n_digit_number(inp, 12));
    
}
