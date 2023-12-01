#include "days.h"
#include "../util.h"
#include <print>
#include <ranges>
#include <numeric>
#include <algorithm>


namespace r = std::ranges;
namespace rv = std::ranges::views;

namespace {
    std::vector<std::string> g_num_strings = {
        "0","1","2","3","4","5","6","7","8", "9",
        "one", "two", "three", "four", "five", "six", "seven", "eight", "nine"
    };

    std::vector<int> g_num_val = {
        0,1,2,3,4,5,6,7,8,9,
        1,2,3,4,5,6,7,8,9
    };

    int leftmost_num(const std::string& inp_str) {
        auto leftmost_pos = static_cast<int>(inp_str.size());
        int val = -1;
        for (const auto& [index, str] : rv::enumerate(g_num_strings)) {
            auto i = inp_str.find(str,0);
            if (i != std::string::npos && i < leftmost_pos) {
                leftmost_pos = i;
                val = g_num_val[index];
            }
        }
        return val;
    }

    int rightmost_num(const std::string& inp_str) {
        auto rightmost_pos = -1;
        int val = -1;
        for (const auto& [index, str] : rv::enumerate(g_num_strings)) {
            int i = inp_str.rfind(str);
            if (i != std::string::npos && i > rightmost_pos) {
                rightmost_pos = i;
                val = g_num_val[index];
            }
        }
        return val;
    }
}

/*------------------------------------------------------------------------------------------------*/

void aoc::y2023::day_01(const std::string& title) {

    auto input = aoc::file_to_string_vector(aoc::input_path(2023, 1));

    std::println("*** day 1 : {0} ***\n", title);
    std::println("{0}", 
        r::fold_left(
            input |
            rv::transform(aoc::remove_nonnumeric) |
            rv::transform(
                [](auto&& str)->int {
                    return 10 * (*str.begin() - '0') + (*str.rbegin() - '0');
                }
        ), 0, std::plus<>())
    );

    std::println("{0}", 
        r::fold_left(
            input |
            rv::transform(
                [](auto&& str)->int {
                    return 10 * leftmost_num(str) + rightmost_num(str);
                }
        ), 0, std::plus<>())
    );
    
}