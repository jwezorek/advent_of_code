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

    int generate_output_signal(const std::vector<int64_t>& program, const phase_setting_seq& seq) {
        int phase_inp = 0;
        for (int phase_setting : seq) {
            aoc::input_buffer prog_inp({ phase_setting, phase_inp });
            aoc::intcode_computer icc(program);
            icc.run(prog_inp);
            phase_inp = icc.output();
        }
        return phase_inp;
    }

    int max_output_signal(const std::vector<int64_t>& program) {
        phase_setting_seq permutation = { 0,1,2,3,4 };
        int max_signal = 0;
        
        do {
            int output = generate_output_signal(program, permutation);
            max_signal = std::max(max_signal, output);
        } while (r::next_permutation(permutation).found);

        return max_signal;
    }

    int generate_output_signal_with_feedback(
            const std::vector<int64_t>& program, const phase_setting_seq& seq) {

        std::array<aoc::intcode_computer, 5> amp = {{
            {program},{program},{program},{program},{program},
        }};

        for (auto [index, phase_setting] : rv::enumerate(seq)) {
            auto event = amp[index].run_until_event(phase_setting);
            if (event != aoc::received_input) {
                throw std::runtime_error("error priming amps");
            }
        }

        bool initialized = false;
        int curr_amp_index = 0;
        bool done = false;
        while (!done) {
            int prev_amp_index = (curr_amp_index > 0) ? curr_amp_index - 1 : 4;
            int input = (!initialized) ? 0 : amp[prev_amp_index].output();
            initialized = true;

            aoc::icc_event next_event = aoc::terminated;
            do {
                next_event = amp[curr_amp_index].run_until_event(input);
            } while (next_event == aoc::received_input);

            done = next_event == aoc::terminated;
            curr_amp_index = (curr_amp_index + 1) % 5;
        }

        return amp[4].output();
    }

    int max_output_signal_with_feedback(const std::vector<int64_t>& program) {
        phase_setting_seq permutation = { 5,6,7,8,9 };
        int max_signal = 0;

        do {
            int output = generate_output_signal_with_feedback( program, permutation );
            max_signal = std::max(max_signal, output);
        } while (r::next_permutation(permutation).found);

        return max_signal;
    }
}

void aoc::y2019::day_07(const std::string& title) {

    auto instructions = split(
            aoc::file_to_string(aoc::input_path(2019, 7)), ','
        ) | rv::transform(
            [](auto&& str)->int64_t {
                return std::stoi(str);
            }
        ) | r::to<std::vector>();

    std::println("--- Day 7: {} ---", title);

    std::println("  part 1: {}",
        max_output_signal( instructions )
    );

    std::println("  part 2: {}",
        max_output_signal_with_feedback( instructions )
    );

}