#include "../util.h"
#include "y2020.h"
#include <filesystem>
#include <functional>
#include <print>
#include <ranges>
#include <unordered_map>
#include <format>

namespace r = std::ranges;
namespace rv = std::ranges::views;

/*------------------------------------------------------------------------------------------------*/

namespace {
    int do_part_1(const std::vector<std::string>& inp) {
        int timestamp = std::stoi(inp.front());
        auto buses = aoc::extract_numbers(inp.back());
        auto wait_times =
            buses | rv::transform(
                [timestamp](int bus) {
                    return bus - timestamp % bus;
                }
            ) | r::to<std::vector<int>>();
        auto index = std::distance(wait_times.begin(), r::min_element(wait_times));
        return wait_times[index] * buses[index];
    }

    std::string generate_mathematica(const std::string& str) {
        auto nums = aoc::split(str, ',');
        std::vector<std::string> constraints;
        for (int i = 0; i < nums.size(); ++i) {
            if (nums[i] == "x") {
                continue;
            }
            if (i == 0) {
                constraints.push_back(
                    std::format(
                        "Mod[n,{}] == {}",
                        std::stoi(nums[i]),
                        i 
                    )
                );
            } else {
                auto x = std::stoi(nums[i]);
                constraints.push_back(
                    std::format(
                        "{} - Mod[n,{}] == {}",
                        x,
                        x,
                        i % x
                    )
                );
            }
        }

        auto equations = constraints | rv::join_with(std::string(" && ")) | r::to<std::string>();
        return std::format(
            "Solve[{}, {}, Integers]",
            equations,
            "{n}"
        );
    }
}

void aoc::y2020::day_13(const std::string& title) {
    auto input = aoc::file_to_string_vector(aoc::input_path(2020, 13));

    std::println("--- Day 13: {} ---", title);
    std::println("  part 1: {}", do_part_1(input));

    //TODO: do this with the chinese remainder theorem, or whatever.
    // i just solved this one with mathematica...
    std::println("  part 2:\n\n{}",
        generate_mathematica(input.back())
    );
}