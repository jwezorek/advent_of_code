
#include "../util/util.h"
#include "y2016.h"
#include <filesystem>
#include <functional>
#include <print>
#include <ranges>
#include <unordered_set>

namespace r = std::ranges;
namespace rv = std::ranges::views;

/*------------------------------------------------------------------------------------------------*/

namespace {
    std::string dragon_invert(const std::string& str) {
        auto reversed = str;
        r::reverse(reversed);
        return reversed | rv::transform(
            [](char digit)->char {
                return digit == '0' ? '1' : '0';
            }
        ) | r::to<std::string>();
    }

    std::string dragon_join(const std::string& lhs, const std::string& rhs) {
        std::stringstream ss;
        ss << lhs << '0' << rhs;
        return ss.str();
    }

    std::string dragon_iteration(const std::string& str) {
        return dragon_join(str, dragon_invert(str));
    }

    std::string dragon_reduce(const std::string& str) {
        return str | rv::chunk(2) | rv::transform(
            [](auto&& pair)->char {
                return (pair[0] == pair[1]) ? '1' : '0';
            }
        ) | r::to<std::string>();
    }

    std::string brute_force_checksum(const std::string& inp, int sz) {
        auto str = inp;
        while (str.size() < sz) {
            str = dragon_iteration(str);
        }
        str = str.substr(0, sz);
        while (str.size() % 2 == 0) {
            str = dragon_reduce(str);
        }
        return str;
    }
}

void aoc::y2016::day_16(const std::string& title) {

    auto inp = aoc::file_to_string(
            aoc::input_path(2016, 16)
        ); 

    std::println("--- Day 16: {} ---", title);
    std::println("  part 1: {}", brute_force_checksum(inp, 272));
    std::println("  part 2: {}", brute_force_checksum(inp, 35651584));
    
}
