#include "../util.h"
#include "y2019.h"
#include <filesystem>
#include <functional>
#include <print>
#include <ranges>
#include <unordered_set>

namespace r = std::ranges;
namespace rv = std::ranges::views;

/*------------------------------------------------------------------------------------------------*/

namespace {
    bool contains_adjacent_duplicate(int num) {
        auto str = std::to_string(num);
        for (auto rng : str | rv::slide(2)) {
            if (rng[0] == rng[1]) {
                return true;
            }
        }
        return false;
    }

    bool contains_unique_adj_duplicate(int num) {
        auto str = std::to_string(num);
        for (auto rng : str | rv::chunk_by(r::equal_to{})) {
            if (r::distance(rng) == 2) {
                return true;
            }
        }
        return false;
    }

    bool is_increasing(int num) {
        auto str = std::to_string(num);
        for (auto rng : str | rv::slide(2)) {
            if (rng[0] > rng[1]) {
                return false;
            }
        }
        return true;
    }
    
    int count_valid(int start_of_range, int end_of_range, 
            const std::function<bool(int)>& is_valid ) {
        return r::fold_left(
            rv::iota(start_of_range, end_of_range + 1) |
                rv::transform(
                    [&](int num) {
                        return is_valid(num) ? 1 : 0;
                    }
                ),
            0,
            std::plus<>()
        );
    }
}

void aoc::y2019::day_04(const std::string& title) {

    auto inp = aoc::split(
            aoc::file_to_string(aoc::input_path(2019, 4)), '-'
        ) | rv::transform(
            [](auto&& str) {return std::stoi(str); }
        ) | r::to<std::vector<int>>();

    std::println("--- Day 4: {} ---", title);
    std::println("  part 1: {}",
        count_valid( inp.front(), inp.back(),
            [](int num)->bool {
                return is_increasing(num) && contains_adjacent_duplicate(num);
            }
        )
    );
    std::println("  part 2: {}",
        count_valid(inp.front(), inp.back(),
            [](int num)->bool {
                return is_increasing(num) && contains_unique_adj_duplicate(num);
            }
        )
    );
}