#include "../util.h"
#include "y2019.h"
#include "intcode.h"
#include <filesystem>
#include <functional>
#include <print>
#include <ranges>
#include <unordered_set>
#include <algorithm>

namespace r = std::ranges;
namespace rv = std::ranges::views;

/*------------------------------------------------------------------------------------------------*/

namespace {

    using phase_setting_seq = std::array<int, 5>;

    int generate_output_signal(const std::vector<int>& program, const phase_setting_seq& seq) {
        int phase_inp = 0;
        for (int phase_setting : seq) {
            aoc::input_buffer prog_inp({ phase_setting, phase_inp });
            aoc::intcode_computer icc(program);
            icc.run(prog_inp);
            phase_inp = icc.output();
        }
        return phase_inp;
    }

    int max_output_signal(const std::vector<int>& program) {
        phase_setting_seq permutation = { 0,1,2,3,4 };
        int max_signal = 0;
        
        do {
            int output = generate_output_signal(program, permutation);
            max_signal = std::max(max_signal, output);
        } while (r::next_permutation(permutation).found);

        return max_signal;
    }

}

void aoc::y2019::day_07(const std::string& title) {

    auto instructions = split(
            aoc::file_to_string(aoc::input_path(2019, 7)), ','
        ) | rv::transform(
            [](auto&& str)->int {
                return std::stoi(str);
            }
        ) | r::to<std::vector>();

    std::println("--- Day 7: {} ---", title);

    std::println("  part 1: {}",
        max_output_signal(instructions)
    );

    std::println("  part 2: {}",
        0
    );

}