
#include "../util/util.h"
#include "y2016.h"
#include <filesystem>
#include <functional>
#include <print>
#include <ranges>
#include <unordered_map>

namespace r = std::ranges;
namespace rv = std::ranges::views;

/*------------------------------------------------------------------------------------------------*/

namespace {
    std::string column(const std::vector<std::string>& rows, int n) {
        return rows | rv::transform(
            [n](auto&& row) {
                return row[n];
            }
        ) | r::to<std::string>();
    }

    char select_char(const std::string& str, bool most_common) {
        std::unordered_map<char, int> count;
        for (auto ch : str) {
            ++count[ch];
        }
        return r::max_element(
            count,
            [most_common](auto&& lhs, auto&& rhs) {
                if (most_common) {
                    return lhs.second < rhs.second;
                } else {
                    return lhs.second > rhs.second;
                }
            }
        )->first;
    }

    std::string decode(const std::vector<std::string>& rows, bool most_common) {
        return rv::iota(0, static_cast<int>(rows.front().size())) | rv::transform(
            [&](auto col)->char {
                return select_char(column(rows, col), most_common);
            }
        ) | r::to<std::string>();
    }
}

void aoc::y2016::day_06(const std::string& title) {

    auto inp = aoc::file_to_string_vector(
            aoc::input_path(2016, 6)
        ); 

    std::println("--- Day 6: {} ---", title);
    std::println("  part 1: {}", decode(inp, true) );
    std::println("  part 2: {}", decode(inp, false) );
    
}
