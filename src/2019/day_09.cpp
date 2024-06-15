#include "../util.h"
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

    void test() {
        auto instructions = aoc::split(
                "109,1,204,-1,1001,100,1,100,1008,100,16,101,1006,101,0,99", ','
            ) | rv::transform(
                [](auto&& str)->int64_t {
                    return std::stoi(str);
                }
            ) | r::to<std::vector>();
        aoc::intcode_computer icc(instructions);
        icc.run(
            []()->int64_t {return 0; },
            [](int64_t v) {
                std::print("{},", v);
            }
        );
        std::println("");

        auto instructions2 = aoc::split(
            "1102,34915192,34915192,7,4,7,99,0", ','
        ) | rv::transform(
            [](auto&& str)->int64_t {
                return std::stoi(str);
            }
        ) | r::to<std::vector>();
        aoc::intcode_computer icc2(instructions2);
        icc2.run(
            []()->int64_t {return 0; },
            [](int64_t v) {
                std::print("{},", v);
            }
        );
        std::println("");
    }

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