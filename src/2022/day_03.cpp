#include "../util.h"
#include "y2022.h"
#include <filesystem>
#include <functional>
#include <print>
#include <ranges>
#include <unordered_map>
#include <unordered_set>

namespace r = std::ranges;
namespace rv = std::ranges::views;

/*------------------------------------------------------------------------------------------------*/

namespace {
    std::tuple<std::string, std::string> split_string_in_half(const std::string& str) {
        auto n = str.size() / 2;
        return { str.substr(0, n), str.substr(n, n) };
    }

    char shared_item_letter(const std::string& input_line) {
        auto [rucksack_1,rucksack_2] = split_string_in_half(input_line);
        auto items_in_rucksack_1 = rucksack_1 | r::to<std::unordered_set<char>>();
        return *r::find_if(rucksack_2,
            [&items_in_rucksack_1](auto rucksack_2_item) {
                return items_in_rucksack_1.find(rucksack_2_item) != items_in_rucksack_1.end();
            }
        );
    }

    int priority_from_letter(char item) {
        return (std::islower(item)) ? (item - 'a') + 1 : (item - 'A') + 27;
    }

    // part 2 ...
    char shared_letter(auto rng) {
        auto letter_sets = rng |
            rv::transform(
                [](const std::string& inp_line) {
                    return inp_line | r::to<std::unordered_set<char>>();
                }
            ) | r::to<std::vector<std::unordered_set<char>>>();
        for (char seed : rng[0]) {
            bool appears_in_all_sets = true;
            for (const auto& letter_set : letter_sets) {
                if (letter_set.find(seed) == letter_set.end()) {
                    appears_in_all_sets = false;
                    break;
                }
            }
            if (appears_in_all_sets) {
                return seed;
            }
        }
        throw std::runtime_error("something's wrong");
    }
}

void aoc::y2022::day_03(const std::string& title) {
    auto input = file_to_string_vector(input_path(2022, 3));
    auto part_1 = r::fold_left(
        input | rv::transform(
            [](const auto& inp_line)->int {
                return priority_from_letter(shared_item_letter(inp_line));
            }
        ),
        0,
        std::plus<>()
    );
    
    auto part_2 = r::fold_left(
        input |
            rv::chunk(3) |
            rv::transform(
                [](auto triple) {
                    return priority_from_letter(shared_letter(triple));
                }
            ),
        0,
        std::plus<>()
    );

    std::println("--- Day 3: {} ---", title);
    std::println("  part 1: {}", part_1);
    std::println("  part 2: {}", part_2);
}