#include "../util.h"
#include "y2019.h"
#include "intcode.h"
#include <filesystem>
#include <functional>
#include <print>
#include <ranges>
#include <unordered_set>
#include <optional>

namespace r = std::ranges;
namespace rv = std::ranges::views;

/*------------------------------------------------------------------------------------------------*/

void aoc::y2019::day_05(const std::string& title) {

    auto instructions = split(
            aoc::file_to_string(aoc::input_path(2019, 5)), ','
        ) | rv::transform(
            [](auto&& str)->int64_t {
                return std::stoi(str);
            }
        ) | r::to<std::vector>();

    intcode_computer icc(instructions);
    input_buffer inp_part_1{ {1} };
    icc.run(inp_part_1);

    std::println("--- Day 5: {} ---", title);
    std::println("  part 1: {}",
        icc.output()
    );

    icc.reset(instructions);
    input_buffer inp_part_2{ {5} };
    icc.run(inp_part_2);

    std::println("  part 2: {}",
        icc.output()
    );
}