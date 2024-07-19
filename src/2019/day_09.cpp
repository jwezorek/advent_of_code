#include "../util/util.h"
#include "y2019.h"
#include "intcode.h"
#include <filesystem>
#include <functional>
#include <print>
#include <ranges>
#include <unordered_set>

namespace r = std::ranges;
namespace rv = std::ranges::views;

/*------------------------------------------------------------------------------------------------*/

namespace {
    int64_t run_boost_program(const std::vector<int64_t>& program, int64_t mode) {
        aoc::intcode_computer icc(program);
        aoc::input_buffer inp({ mode });
        icc.run(inp);
        return icc.output();
    }
}

void aoc::y2019::day_09(const std::string& title) {

    auto instructions = split(
            aoc::file_to_string(aoc::input_path(2019, 9)), ','
        ) | rv::transform(
            [](auto&& str)->int64_t {
                return std::stoi(str);
            }
        ) | r::to<std::vector>();

    std::println("--- Day 9: {} ---\n", title);
    std::println("  part 1: {}",
        run_boost_program(instructions, 1)
    );
    std::println("  part 2: {}",
        run_boost_program(instructions, 2)
    );
}