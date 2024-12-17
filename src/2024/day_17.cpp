
#include "../util/util.h"
#include "y2024.h"
#include <filesystem>
#include <functional>
#include <print>
#include <ranges>
#include <unordered_set>
#include <unordered_map>
#include <format>

namespace r = std::ranges;
namespace rv = std::ranges::views;

/*------------------------------------------------------------------------------------------------*/

namespace {

    struct computer_state {
        std::array<int64_t, 3> registers;
        std::vector<int> program;
        std::vector<int> output;
        int instr_ptr;
    };

    computer_state parse_input(const std::vector<std::string>& inp) {
        auto vals = inp | rv::transform(
            [](auto&& str) {
                return aoc::extract_numbers_int64(str);
            }
        ) | r::to<std::vector>();

        computer_state state;
        state.registers[0] = vals[0].front();
        state.registers[1] = vals[1].front();
        state.registers[2] = vals[2].front();
        state.program = vals[4] | rv::transform(
                [](auto v) {return static_cast<int>(v); }
            ) | r::to<std::vector>();
        state.instr_ptr = 0;

        return state;
    }

    enum op {
        adv = 0,
        bxl,
        bst,
        jnz,
        bxc,
        out,
        bdv,
        cdv
    };

    using op_fn = std::function<void(computer_state&, int64_t)>;

    constexpr int a_reg = 0;
    constexpr int b_reg = 1;
    constexpr int c_reg = 2;

    struct op_info {
        op_fn func;
        bool literal_operand;
    };

    void do_adv(computer_state& state, int64_t operand) {
        auto numer = state.registers[a_reg];
        auto denom = static_cast<int64_t>(1) << operand;
        state.registers[a_reg] = numer / denom;
    }

    void do_bxl(computer_state& state, int64_t operand) {
        state.registers[b_reg] = state.registers[b_reg] ^ operand;
    }

    void do_bst(computer_state& state, int64_t operand) {
        state.registers[b_reg] = operand % 8;
    }

    void do_jnz(computer_state& state, int64_t operand) {
        if (state.registers[a_reg] == 0) {
            return;
        }
        state.instr_ptr = operand;
    }

    void do_bxc(computer_state& state, int64_t operand) {
        state.registers[b_reg] = state.registers[b_reg] ^ state.registers[c_reg];
    }

    void do_out(computer_state& state, int64_t operand) {
        state.output.push_back(operand % 8);
    }

    void do_bdv(computer_state& state, int64_t operand) {
        auto numer = state.registers[a_reg];
        auto denom = static_cast<int64_t>(1) << operand;
        state.registers[b_reg] = numer / denom;
    }

    void do_cdv(computer_state& state, int64_t operand) {
        auto numer = state.registers[a_reg];
        auto denom = static_cast<int64_t>(1) << operand;
        state.registers[c_reg] = numer / denom;
    }

    int64_t eval_operand(const computer_state& state, int operand, bool literal_operand) {
        if (literal_operand) {
            return operand;
        }
        if (operand >= 0 && operand <= 3) {
            return operand;
        }
        if (operand == 7) {
            throw std::runtime_error("this should not happen");
        }
        return state.registers[operand - 4];
    }

    std::string run_computer(const computer_state& inp) {

        const static std::unordered_map<op, op_info> op_tbl = {
            {adv, {do_adv, false}},
            {bxl, {do_bxl, true}},
            {bst, {do_bst, false}},
            {jnz, {do_jnz, true}},
            {bxc, {do_bxc, true}},
            {out, {do_out, false}},
            {bdv, {do_bdv, false}},
            {cdv, {do_cdv, false}}
        };

        auto state = inp;
        while (state.instr_ptr < state.program.size()) {
            const auto& op_info = op_tbl.at(static_cast<op>(state.program[state.instr_ptr]));
            auto operand = eval_operand(
                state, state.program[state.instr_ptr + 1], op_info.literal_operand
            );
            auto old_instr_ptr = state.instr_ptr;
            op_info.func(state, operand);
            if (old_instr_ptr == state.instr_ptr) {
                state.instr_ptr += 2;
            }
        }

        return state.output | rv::transform(
                [](auto&& v) { return std::to_string(v); }
            ) | rv::join_with(',') | r::to<std::string>();
    }

    int64_t oct_to_dec(const std::string& str) {
        return std::stoll(str, nullptr, 8);
    }

    std::string find_magic(const std::string& magic_octal, const std::vector<int>& target) {

        if (target.empty()) {
            return magic_octal;
        }

        int target_digit = target.front();
        for (int test_digit = 0; test_digit < 8; ++test_digit) {
            auto test = magic_octal + std::to_string(test_digit);
            auto transformed_digit = 
                ((oct_to_dec(test) / (1 << (7 - test_digit))) ^ test_digit) % 8;
            if (transformed_digit == target_digit) {
                auto result = find_magic( test, target | rv::drop(1) | r::to<std::vector>() );
                if (result.empty()) {
                    continue;
                }
                return result;
            }
        }

        return "";
    }
    
    int64_t find_magic_number(const std::vector<int>& inp) {
        auto target_digits = inp;
        r::reverse(target_digits);
        return oct_to_dec(find_magic("", target_digits));
    }

}

void aoc::y2024::day_17(const std::string& title) {

    auto inp = parse_input(
        aoc::file_to_string_vector(
            aoc::input_path(2024, 17)
        )
    );

    std::println("--- Day 17: {} ---", title);
    std::println("  part 1: {}", run_computer(inp) );
    std::println("  part 2: {}", find_magic_number(inp.program) );
    
}
