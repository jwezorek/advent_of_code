#include "../util/util.h"
#include "y2019.h"
#include "intcode.h"
#include <filesystem>
#include <functional>
#include <print>
#include <ranges>
#include <unordered_map>
#include <tuple>

namespace r = std::ranges;
namespace rv = std::ranges::views;

/*------------------------------------------------------------------------------------------------*/

namespace {

    int run_program(const std::vector<int64_t>& memory, int noun, int verb) {
        aoc::intcode_computer intcode(memory);

        intcode.value(1) = noun;
        intcode.value(2) = verb;
        intcode.run();

        return intcode.value(0);
    }

    std::tuple<int64_t, int64_t> find_noun_and_verb(const std::vector<int64_t>& memory, int target) {
        for (auto [noun, verb] : rv::cartesian_product(rv::iota(0, 100), rv::iota(0, 100))) {
            if (run_program(memory, noun, verb) == target) {
                return { noun, verb };
            }
        }
        throw std::runtime_error("target not found");
    }

    int score_noun_and_verb(const std::vector<int64_t>& memory, int target) {
        auto [noun, verb] = find_noun_and_verb(memory, target);
        return 100 * noun + verb;
    }
}

void aoc::y2019::day_02(const std::string& title) {

    auto inp = split(
            aoc::file_to_string(aoc::input_path(2019, 2)), ','
        ) | rv::transform(
            [](auto&& str)->int64_t {
                return std::stoi(str);
            }
        ) | r::to<std::vector>();

    std::println("--- Day 2: {} ---", title);

    std::println("  part 1: {}",
        run_program( inp, 12, 2 )
    );
    std::println("  part 2: {}",
        score_noun_and_verb(inp, 19690720)
    );
}