#include "../util.h"
#include "y2020.h"
#include <filesystem>
#include <functional>
#include <print>
#include <ranges>
#include <unordered_map>
#include <unordered_set>

namespace r = std::ranges;
namespace rv = std::ranges::views;

/*------------------------------------------------------------------------------------------------*/

namespace {
    enum op_code {
        nop,
        acc,
        jmp
    };

    struct instruction {
        op_code op;
        int arg;
    };

    struct program_state {
        int instr_counter;
        int accumulator;
    };

    instruction str_to_instr(const std::string& str) {
        auto parts = aoc::split(str, ' ');
        static const std::unordered_map<std::string, op_code> str_to_op = {
            {"nop" , nop},
            {"acc" , acc},
            {"jmp" , jmp}
        };
        return {
            str_to_op.at(parts.front()),
            std::stoi(parts.back())
        };
    }

    program_state perform_instr(const instruction& instr, const program_state& state) {
        auto new_state = state;
        switch (instr.op) {
            case nop:
                new_state.instr_counter++;
                break;
            case acc:
                new_state.accumulator += instr.arg;
                new_state.instr_counter++;
                break;
            case jmp:
                new_state.instr_counter += instr.arg;
                break;
        }
        return new_state;
    }
    
    std::tuple<int,bool> run_until_loop( const std::vector<instruction>& code) {
        int n = static_cast<int>(code.size());
        std::unordered_set<int> line_set;
        program_state state = { 0,0 };
        while (state.instr_counter < n && !line_set.contains(state.instr_counter)) {
            line_set.insert(state.instr_counter);
            const auto& instr = code.at(state.instr_counter);
            state = perform_instr(instr, state);
        }
        return {
            state.accumulator,
            state.instr_counter == n
        };
    }

    int run_without_loop(const std::vector<instruction>& code) {
        int n = static_cast<int>(code.size());
        for (int i = 0; i < n; ++i) {
            if (code[i].op == nop || code[i].op == jmp) {
                auto altered_code = code;
                altered_code[i].op = (altered_code[i].op == nop) ? jmp : nop;
                auto [a, success] = run_until_loop(altered_code);
                if (success) {
                    return a;
                }
            }
        }
        return -1;
    }
}

void aoc::y2020::day_08(const std::string& title) {
    auto input = aoc::file_to_string_vector(aoc::input_path(2020, 8));
    auto code = input | rv::transform(str_to_instr) | r::to<std::vector<instruction>>();

    std::println("--- Day 8: {} ---", title);
    std::println("  part 1: {}", std::get<0>(run_until_loop(code)));
    std::println("  part 2: {}", run_without_loop(code));
}