#include "days.h"
#include "../util.h"
#include <print>
#include <ranges>
#include <numeric>
#include <algorithm>

namespace r = std::ranges;
namespace rv = std::ranges::views;

namespace {

    struct num_val {
        std::string str;
        int val;
    };

    std::vector<num_val> g_nums = {
        {"0",0},{"1",1},{"2",2},{"3",3},{"4",4},{"5",5},
        {"6",6},{"7",7},{"8",8},{"9",9},{"one",1}, {"two",2},
        {"three",3}, {"four",4}, {"five",5}, {"six",6}, 
        {"seven",7}, {"eight",8}, {"nine",9}
    };

    int left_num(const std::string& inp_str) {
        return r::min_element(
            g_nums,
            [inp_str](auto&& lhs, auto&& rhs)->bool {
                return inp_str.find(lhs.str) < inp_str.find(rhs.str);
            }
        )->val;
    }

    int right_num(const std::string& inp_str) {
        return r::max_element(
            g_nums,
            [inp_str](auto&& lhs, auto&& rhs)->bool {
                return inp_str.rfind(lhs.str) > inp_str.rfind(rhs.str);
            }
        )->val;
    }
}

/*------------------------------------------------------------------------------------------------*/

void aoc::y2023::day_01(const std::string& title) {

    auto input = aoc::file_to_string_vector(aoc::input_path(2023, 1));

    std::println("--- Day 1: {0} ---\n", title);
    std::println("part 1: {0}", 
        r::fold_left(
            input |
            rv::transform(aoc::remove_nonnumeric) |
            rv::transform(
                [](auto&& str)->int {
                    return 10 * (*str.begin() - '0') + (*str.rbegin() - '0');
                }
        ), 0, std::plus<>())
    );
    std::println("part 2: {0}", 
        r::fold_left(
            input |
            rv::transform(
                [](auto&& str)->int {
                    return 10 * left_num(str) + right_num(str);
                }
        ), 0, std::plus<>())
    );
    
}