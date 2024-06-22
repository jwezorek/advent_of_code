#include "../util.h"
#include "intcode.h"
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

    using computer = aoc::intcode_computer;

    int count_tractor_beam(const std::vector<int64_t>& program) {
        computer c(program);
        int sum = 0;
        for (int y = 0; y < 50; ++y) {
            for (int x = 0; x < 50; ++x) {
                auto computer = c;
                aoc::input_buffer buffer({ x,y });
                computer.run(buffer);
                sum += computer.output();
            }
        }
        return sum;
    }
}

void aoc::y2019::day_19(const std::string& title) {

    auto program = split(
            aoc::file_to_string(aoc::input_path(2019, 19)), ','
        ) | rv::transform(
            [](auto&& str)->int64_t {
                return aoc::string_to_int64(str);
            }
        ) | r::to<std::vector>();

    std::println("--- Day 19: {} ---", title);
    std::println("  part 1: {}",
        count_tractor_beam(program)
    );
    std::println("  part 2: {}",
        0
    );
}