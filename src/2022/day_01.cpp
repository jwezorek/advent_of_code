#include "../util.h"
#include "y2022.h"
#include <filesystem>
#include <functional>
#include <print>
#include <ranges>

namespace r = std::ranges;
namespace rv = std::ranges::views;

/*------------------------------------------------------------------------------------------------*/

void aoc::y2022::day_01(const std::string& title) {
    auto input = aoc::file_to_string_vector(aoc::input_path(2022, 1));
    auto calories = input | 
            rv::chunk_by([](auto lhs, auto rhs) {return is_number(lhs) && is_number(rhs); }) |
            rv::filter([](auto group) {return !group.front().empty(); }) |
            rv::transform(
                [](auto group) {
                    return r::fold_left(
                        group | rv::transform([](const auto& str) {return std::stoi(str); }), 
                        0,
                        std::plus<>()
                    );
                }
        ) | r::to<std::vector<int>>();

    r::sort(calories, std::greater<int>());

    auto max_calories_1_elf = calories.front();
    auto max_calories_3_elves = r::fold_left(calories | rv::take(3), 0, std::plus<>());

    std::println("--- Day 1: {} ---", title);
    std::println("  part 1: {}", max_calories_1_elf);
    std::println("  part 2: {}", max_calories_3_elves);
}