
#include "../util/util.h"
#include "y2018.h"
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

    bool opposite_polarity(char a, char b) {
        return a != b && (std::toupper(a) == b || a == std::toupper(b));
    }

    int do_reduction(const std::string& polymer) {
        std::stack<char> stack;
        for (auto ch : polymer) {
            if (! stack.empty() && opposite_polarity(stack.top(), ch)) {
                stack.pop();
            } else {
                stack.push(ch);
            }
        }
        return stack.size();
    }

    int do_part_2(const std::string& polymer) {
        return r::min(
            rv::iota(0, 26) | rv::transform(
                [&](auto i) {
                    auto str = polymer;
                    char removee = 'a' + i;
                    std::erase(str, removee);
                    std::erase(str, std::toupper(removee));
                    return do_reduction(str);
                }
            )
        );
    }
}

void aoc::y2018::day_05(const std::string& title) {

    auto inp = aoc::file_to_string(
            aoc::input_path(2018, 5)
        ) | rv::filter(
            [](char ch) { return std::isalpha(ch); }
        ) | r::to<std::string>();
    
    std::println("--- Day 5: {} ---", title);
    std::println("  part 1: {}", do_reduction(inp));
    std::println("  part 2: {}", do_part_2(inp));
    
}
