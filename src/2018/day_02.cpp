
#include "../util/util.h"
#include "y2018.h"
#include <filesystem>
#include <functional>
#include <print>
#include <ranges>
#include <unordered_set>

namespace r = std::ranges;
namespace rv = std::ranges::views;

/*------------------------------------------------------------------------------------------------*/

namespace {

    auto letters() {
        return rv::iota(0, 26) |
            rv::transform(
                [](auto i)->char {
                    return 'a' + i;
                }
            );
    }

    bool has_n_repeated_letters(const std::string& str, int n) {
        for (auto letter : letters()) {
            auto count = r::count_if(str, [&](auto ch) {return letter == ch; });
            if (count == n) {
                return true;
            }
        }
        return false;
    }

    int calculate_checksum(const std::vector<std::string>& inp) {
        int twos = 0;
        int threes = 0;
        for (auto str : inp) {
            twos += has_n_repeated_letters(str, 2) ? 1 : 0;
            threes += has_n_repeated_letters(str, 3) ? 1 : 0;
        }
        return twos * threes;
    }

    int differences_count(const std::string& str1, const std::string& str2) {
        return r::count_if(
            rv::zip(str1, str2),
            [](auto&& pair) {
                auto [u, v] = pair;
                return u != v;
            }
        );
    }

    std::string common_letters(const std::string& str1, const std::string& str2) {
        std::stringstream ss;
        for (auto [u, v] : rv::zip(str1, str2)) {
            if (u == v) {
                ss << u;
            }
        }
        return ss.str();
    }

    std::string find_boxes(const std::vector<std::string>& inp) {
        for (auto [u, v] : aoc::two_combinations(inp)) {
            if (differences_count(u, v) == 1) {
                return common_letters(u, v);
            }
        }
        return "";
    }
}

void aoc::y2018::day_02(const std::string& title) {

    auto inp = aoc::file_to_string_vector(
        aoc::input_path(2018, 2)
    );

    std::println( "--- Day 2: {} ---", title);
    std::println( "  part 1: {}", calculate_checksum(inp) );
    std::println("  part 2: {}", find_boxes(inp));
    
}
