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

    bool has_three_vowels(const std::string& str) {
        return r::count_if(
                str,
                [](char ch) {
                    return ch == 'a' || ch == 'e' || ch == 'i' || ch == 'o' || ch == 'u';
                }
            ) >= 3;
    }

    bool has_double_letter(const std::string& str) {
        for (auto pair : str | rv::slide(2)) {
            if (pair[0] == pair[1]) {
                return true;
            }
        }
        return false;
    }

    bool doesnt_have_bad_pair(const std::string& str) {
        std::unordered_set<std::string> bad_set = {
            "ab", "cd", "pq", "xy"
        };
        for (auto pair : str | rv::slide(2)) {
            if (bad_set.contains(pair | r::to<std::string>())) {
                return false;
            }
        }
        return true;
    }
    
    bool is_nice_part1(const std::string& str) {
        return has_three_vowels(str) && has_double_letter(str) && doesnt_have_bad_pair(str);
    }

    bool has_pair_of_nonoverlapping_pairs(const std::string& str) {
        std::unordered_map<std::string, int> first_occurence;
        for (auto [index,rng] : rv::enumerate(str | rv::slide(2))) {
            auto pair = rng | r::to<std::string>();
            if (first_occurence.contains(pair) && index - first_occurence[pair] >= 2) {
                return true;
            }
            if (!first_occurence.contains(pair)) {
                first_occurence[pair] = index;
            }
        }
        return false;
    }

    bool has_same_letter_separated_by_a_letter(const std::string& str) {
        for (auto triple : str | rv::slide(3)) {
            if (triple[0] == triple[2]) {
                return true;
            }
        }
        return false;
    }

    bool is_nice_part2(const std::string& str) {
        return has_pair_of_nonoverlapping_pairs(str) && 
            has_same_letter_separated_by_a_letter(str);
    }

}

void aoc::y2015::day_05(const std::string& title) {

    auto inp = aoc::file_to_string_vector(aoc::input_path(2015, 5));

    std::println("--- Day 5: {} ---", title);
    std::println("  part 1: {}",
        r::count_if(inp, is_nice_part1)
    );
    std::println("  part 2: {}",
        r::count_if(inp, is_nice_part2)
    );
}