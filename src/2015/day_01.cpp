#include "../util/util.h"
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

    std::tuple<int, int> process_parens(const std::string& parens) {
        int sum = 0;
        int first_neg = -1;
        for (int i = 0; i < parens.size(); ++i) {
            sum += (parens.at(i) == '(') ? 1 : -1;
            if (first_neg < 0 && sum == -1) {
                first_neg = i;
            }
        }
        return { sum, first_neg + 1};
    }

}

void aoc::y2015::day_01(const std::string& title) {

    auto [part_1, part_2] = process_parens(
        aoc::file_to_string(aoc::input_path(2015, 1))
    );

    std::println("--- Day 1: {} ---", title);
    std::println("  part 1: {}", part_1);
    std::println("  part 2: {}", part_2);
    
}