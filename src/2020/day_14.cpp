#include "../util/util.h"
#include "y2020.h"
#include <filesystem>
#include <functional>
#include <print>
#include <ranges>
#include <unordered_map>

namespace r = std::ranges;
namespace rv = std::ranges::views;

/*------------------------------------------------------------------------------------------------*/

namespace {

    struct command {
        std::string mask;
        uint64_t address;
        uint64_t value;
    };

    command parse_command(const std::string& str) {
        if (str.contains("mask")) {
            auto parts = aoc::split(str, ' ');
            return { parts[2], 0, 0};
        }
        auto numbers = aoc::extract_numbers_int64(str);
        return {
            std::string{},
            static_cast<uint64_t>(numbers[0]),
            static_cast<uint64_t>(numbers[1])
        };
    }

    using memory = std::unordered_map<uint64_t, uint64_t>;

    struct state {
        memory mem;
        std::string mask;
    };

    uint64_t make_ones_mask(const std::string& mask) {
        auto ones = mask | rv::transform(
            [](char digit) {
                return (digit == '1') ? '1' : '0';
            }
        ) | r::to<std::string>();
        return std::stoull(ones, 0, 2);
    }

    uint64_t make_zeros_mask(const std::string& mask) {
        auto zeros = mask | rv::transform(
            [](char digit) {
                return (digit == '0') ? '0' : '1';
            }
        ) | r::to<std::string>();
        zeros = std::string(64 - 36, '1') + zeros;
        return std::stoull(zeros, 0, 2);
    }

    using assignment_statement_fn = std::function<void(state&, const command& cmd)>;

    void masked_value_assignment(state& state, const command& cmd) {
        uint64_t ones_mask = make_ones_mask(state.mask);
        uint64_t zeros_mask = make_zeros_mask(state.mask);
        auto new_val = cmd.value | ones_mask;
        new_val = new_val & zeros_mask;

        state.mem[cmd.address] = new_val;
    }

    void exec_command(state& state, const command& cmd, 
            assignment_statement_fn assignment) {
        if (!cmd.mask.empty()) {
            state.mask = cmd.mask;
            return;
        }
        assignment(state, cmd);
    }

    uint64_t run_program_and_sum_memory(const std::vector<command>& cmds, 
            assignment_statement_fn assignment) {
        state state;
        for (const auto& cmd : cmds) {
            exec_command(state, cmd, assignment);
        }
        return r::fold_left(
            state.mem | rv::values,
            0,
            std::plus<uint64_t>()
        );
    }
}

auto binary_digits(uint64_t value, int n) {
    return rv::iota(0, n) | rv::transform(
        [value, n](uint64_t j) {
            uint64_t i = n - j - 1;
            uint64_t mask = static_cast<uint64_t>(1) << i;
            return (value & mask) >> i;
        }
    );
}

void masked_address_assignment(state& state, const command& cmd) {
    auto variables = rv::enumerate(state.mask) | rv::filter(
            [](auto&& key_val) {
                return std::get<1>(key_val) == 'X';
            }
        ) | rv::keys | r::to<std::vector<int>>();
    uint64_t num_digits = variables.size();
    auto n = (1ull << num_digits);

    auto num_str = rv::zip_transform(
            [](char ch, uint64_t digit)->char {
                if (ch == '0') {
                    return '0' + digit;
                }
                return ch;
            },
            state.mask,
            binary_digits(cmd.address, 36)
        ) | r::to<std::string>();

    for (uint64_t i = 0; i < n; ++i) {
        auto filled_in_vars = num_str;
        for (auto [var_index, digit] : rv::zip(variables, binary_digits(i, num_digits))) {
            filled_in_vars[var_index] = '0' + digit;
        }
        uint64_t new_address = std::stoull(filled_in_vars, 0, 2);
        state.mem[new_address] = cmd.value;
    }
}

void aoc::y2020::day_14(const std::string& title) {
    auto input = aoc::file_to_string_vector(aoc::input_path(2020, 14)) |
        rv::transform(parse_command) | r::to<std::vector<command>>();

    std::println("--- Day 14: {} ---", title);
    std::println("  part 1: {}", run_program_and_sum_memory(input, masked_value_assignment));
    std::println("  part 2: {}", run_program_and_sum_memory(input, masked_address_assignment));
}