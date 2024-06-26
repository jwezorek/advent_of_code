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

    int64_t run_springdroid(const computer& comp, const std::string& program) {
        auto springdroid = comp;
        aoc::input_buffer buffer(string_to_intcode(program));
        std::stringstream ss;
        std::optional<int64_t> result;
        springdroid.run(
            buffer,
            [&](int64_t v) {
                if (v > 255) {
                    result = v;
                } else {
                    ss << static_cast<char>(v);
                }
            }
        );
        if (!result) {
            std::println("clock ticks: {}", springdroid.ticks());
            std::println("{}", ss.str());
            return -1;
        }
        
        return result.value();
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
    ;

    
    std::println("--- Day 21: {} ---", title);
    std::println("  part 1: {}",
        run_springdroid(
            springdroid,
            "NOT A J\n"  // o_

            "NOT C T\n" // o#?_#
            "AND A T\n"
            "AND D T\n"

            "OR T J\n"

            "WALK\n"
        )
    );
    std::println("  part 2: {}",
        run_springdroid(
            springdroid,
            "NOT A J\n"  // o_

            "NOT C T\n" // o#?_#  || o#-?#
            "NOT T T\n"
            "AND B T\n"
            "NOT T T\n"
            "AND A T\n"
            "AND D T\n"

            "OR T J\n"

            "RUN\n"
        )
    );
    
}