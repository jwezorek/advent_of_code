#include "../util/util.h"
#include "y2015.h"
#include <filesystem>
#include <functional>
#include <print>
#include <ranges>
#include <unordered_set>
#include <stack>

namespace r = std::ranges;
namespace rv = std::ranges::views;

/*------------------------------------------------------------------------------------------------*/

namespace {
    std::string increment_string(const std::string& inp) {
        std::vector<char> out;

        auto reversed_inp = inp | rv::reverse;
        int digit_index = 0;
        int carry = 1;

        do {
            int curr_digit = (digit_index < reversed_inp.size()) ? 
                reversed_inp[digit_index] - 'a' : 
                0;
            curr_digit += carry;
            if (curr_digit == 26) {
                curr_digit = 0;
                carry = 1;
            } else {
                carry = 0;
            }
            out.push_back( 'a' + curr_digit);
            ++digit_index;
        } while (digit_index < reversed_inp.size() || carry > 0);

        return out | rv::reverse | r::to<std::string>();
    }

    bool has_increasing_straight(const std::string& str) {
        for (auto triple : str | rv::slide(3)) {
            auto diff_1 = triple[1] - triple[0];
            auto diff_2 = triple[2] - triple[1];
            if (diff_1 == 1 && diff_2 == 1) {
                return true;
            }
        }
        return false;
    }

    bool does_not_contain_iol(const std::string& str) {
        if (str.contains('i') || str.contains('o') || str.contains('l')) {
            return false;
        }
        return true;
    }

    bool has_two_different_nonoverlapping_pairs(const std::string& str) {
        std::unordered_set<std::string> pairs;
        for (auto [index, rng] : rv::enumerate(str | rv::slide(2))) {
            auto pair = rng | r::to<std::string>();
            if (pair[0] != pair[1]) {
                continue;
            }
            pairs.insert(pair);
            if (pairs.size() >= 2) {
                return true;
            }
        }
        return false;
    }

    bool is_valid_password(const std::string& str) {
        return has_increasing_straight(str) &&
            does_not_contain_iol(str) &&
            has_two_different_nonoverlapping_pairs(str);
    }

    std::string next_valid_password(const std::string& initial_pwd) {
        auto pwd = initial_pwd;
        do {
            pwd = increment_string(pwd);
        } while (!is_valid_password(pwd));
        return pwd;
    }
}

void aoc::y2015::day_11(const std::string& title) {

    auto password = aoc::file_to_string(aoc::input_path(2015, 11));

    std::println("--- Day 11: {} ---", title);
    std::println("  part 1: {}",
        next_valid_password(password)
    );
    std::println("  part 2: {}",
        next_valid_password(next_valid_password(password))
    );
}