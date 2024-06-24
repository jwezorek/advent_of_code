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

    using computer = aoc::intcode_computer;

    std::vector<int64_t> string_to_intcode(const std::string& str) {
        return str | rv::transform(
                [](char ch)->int64_t {
                    return static_cast<int64_t>(ch);
                }
            ) | r::to<std::vector>();
    }

    void run_springdroid(const computer& comp, const std::string& program) {
        auto springdroid = comp;
        aoc::input_buffer buffer(string_to_intcode(program));
        std::stringstream ss;
        springdroid.run(
            buffer,
            [&](int64_t v) {
                ss << static_cast<char>(v);
            }
        );
        std::println("{}", ss.str());
    }

}

void aoc::y2019::day_21(const std::string& title) {

    auto program = split(
            aoc::file_to_string(aoc::input_path(2019, 21)), ','
        ) | rv::transform(
            [](auto&& str)->int64_t {
                return aoc::string_to_int64(str);
            }
        ) | r::to<std::vector>();

    computer springdroid(program);
    run_springdroid(
        springdroid,
        "NOT C T\n"
        "NOT D J\n"
        "AND T J\n"
        "AND B J\n"
        "NOT A T\n"
        "OR T J\n"
        "WALK\n"
    );

    /*
    std::println("--- Day 21: {} ---", title);
    std::println("  part 1: {}",
        0
    );
    std::println("  part 2: {}",
        0
    );
    */
}