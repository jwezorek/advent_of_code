
#include "../util/util.h"
#include "y2017.h"
#include <filesystem>
#include <functional>
#include <print>
#include <ranges>
#include <unordered_map>

namespace r = std::ranges;
namespace rv = std::ranges::views;

/*------------------------------------------------------------------------------------------------*/

namespace {
    enum compare_op {
        GT,
        LT,
        GTE,
        LTE,
        EQ,
        NEQ
    };

    struct instruction {
        std::string reg;
        int offset;
        compare_op op;
        std::string arg1;
        int arg2;
    };

    instruction parse_instruction(const std::string& str) {
        static const std::unordered_map<std::string, compare_op> str_to_op = {
            {">", GT}, {"<", LT}, {">=", GTE}, {"<=", LTE},  {"==", EQ},  {"!=", NEQ}
        };
        auto parts = aoc::split(str, ' ');
        int increment_multiplier = (parts[1] == "inc") ? 1 : -1;
        return {
            .reg = parts[0],
            .offset = increment_multiplier * std::stoi(parts[2]),
            .op = str_to_op.at(parts[5]),
            .arg1 = parts[4],
            .arg2 = std::stoi(parts[6])
        };
    }

    bool do_compare_op(compare_op op, int arg1, int arg2) {
        static const std::vector<std::function<bool(int, int)>> funcs = {
            /* GT */    [](int lhs, int rhs)->bool {
                            return lhs > rhs;
                        } ,
            /* LT */    [](int lhs, int rhs)->bool {
                            return lhs < rhs;
                        } ,
            /* GTE */   [](int lhs, int rhs)->bool {
                            return lhs >= rhs;
                        },
            /* LTE */   [](int lhs, int rhs)->bool {
                            return lhs <= rhs;
                        },
            /* EQ */    [](int lhs, int rhs)->bool {
                            return lhs == rhs;
                        },
            /* NEQ */   [](int lhs, int rhs)->bool {
                            return lhs != rhs;
                        }
        };
        return funcs.at(static_cast<int>(op))(arg1, arg2);
    }

    using state = std::unordered_map<std::string, int>;

    int& get_register(state& state, const std::string& reg) {
        if (!state.contains(reg)) {
            state[reg] = 0;
        }
        return state[reg];
    }

    void perform_instruction(state& state, const instruction& instr) {
        if (do_compare_op(instr.op, get_register(state, instr.arg1), instr.arg2)) {
            get_register(state, instr.reg) += instr.offset;
        }
    }

    int do_part_1(const std::vector<instruction>& code) {
        state state;
        for (const auto& instr : code) {
            perform_instruction(state, instr);
        }
        return r::max(state | rv::values);
    }

    int do_part_2(const std::vector<instruction>& code) {
        state state;
        int max = std::numeric_limits<int>::min();
        for (const auto& instr : code) {
            perform_instruction(state, instr);
            auto current_max_reg = r::max(state | rv::values);
            max = std::max(max, current_max_reg);
        }
        return max;
    }
}

void aoc::y2017::day_08(const std::string& title) {

    auto inp = aoc::file_to_string_vector(
            aoc::input_path(2017, 8)
        ) | rv::transform(
            parse_instruction
        ) | r::to<std::vector>();

    std::println("--- Day 8: {} ---", title);
    std::println("  part 1: {}", do_part_1(inp) );
    std::println("  part 2: {}", do_part_2(inp) );
    
}
