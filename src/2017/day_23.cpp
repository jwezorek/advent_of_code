
#include "../util/util.h"
#include "y2017.h"
#include <filesystem>
#include <functional>
#include <print>
#include <ranges>
#include <unordered_set>
#include <queue>
#include <variant>

namespace r = std::ranges;
namespace rv = std::ranges::views;

/*------------------------------------------------------------------------------------------------*/

namespace {

    enum op_type {
        set,
        sub,
        mul,
        jnz
    };

    using arg_var = std::variant<std::monostate, std::string, int64_t>;

    struct instruction {
        op_type op;
        arg_var arg1;
        arg_var arg2;
    };

    arg_var parse_arg_var(const std::string& str) {
        if (str.empty()) {
            return std::monostate{};
        }
        if (std::isalpha(str.front())) {
            return str;
        }
        return static_cast<int64_t>(std::stoi(str));
    }

    op_type str_to_op(const std::string& str) {
        static const std::unordered_map<std::string, op_type> tbl = {
            {"set",set}, {"sub",sub}, {"mul",mul}, {"jnz",jnz}
        };
        return tbl.at(str);
    }

    instruction parse_instr(const std::string& str) {
        auto parts = aoc::split(str, ' ');
        auto second_arg = (parts.size() == 3) ? parts[2] : std::string{};
        return {
            str_to_op(parts[0]),
            parse_arg_var(parts[1]),
            parse_arg_var(second_arg)
        };
    }

    using reg_map = std::unordered_map<std::string, int64_t>;

    struct execution_state {
        int pos;
        reg_map registers;
        int num_mults;
    };

    using op_func = std::function<void(execution_state&, const arg_var&, const arg_var&)>;

    int64_t eval_arg(execution_state& state, const arg_var& arg) {
        return std::visit(
            aoc::overload{
                [](std::monostate)->int64_t {
                    throw std::runtime_error("bad argument");
                },
                [&](const std::string& str)->int64_t {
                    return state.registers[str];
                },
                [&](int64_t v)->int64_t {
                    return v;
                }
            },
            arg
        );
    }

    void assign_to_reg(execution_state& state, const arg_var& var, int64_t val) {
        if (!std::holds_alternative<std::string>(var)) {
            throw std::runtime_error("bad assignment");
        }
        state.registers[std::get<std::string>(var)] = val;
    }

    void do_set(execution_state& state, const arg_var& arg1, const arg_var& arg2) {
        assign_to_reg(state, arg1, eval_arg(state, arg2));
        ++state.pos;
    }

    void do_sub(execution_state& state, const arg_var& arg1, const arg_var& arg2) {
        auto old_val = eval_arg(state, arg1);
        assign_to_reg(state, arg1, old_val - eval_arg(state, arg2));
        ++state.pos;
    }

    void do_mul(execution_state& state, const arg_var& arg1, const arg_var& arg2) {
        auto old_val = eval_arg(state, arg1);
        assign_to_reg(state, arg1, old_val * eval_arg(state, arg2));
        ++state.num_mults;
        ++state.pos;
    }

    void do_jnz(execution_state& state, const arg_var& arg1, const arg_var& arg2) {
        if (eval_arg(state, arg1) != 0) {
            state.pos += eval_arg(state, arg2);
        } else {
            ++state.pos;
        }
    }

    void execute_instruction(execution_state& state, const instruction& instr) {
        const static std::array<op_func, 4> tbl = { {
            do_set, do_sub, do_mul, do_jnz
        } };
        tbl.at(static_cast<int>(instr.op))(state, instr.arg1, instr.arg2);
    }

    int64_t run_program(const std::vector<instruction>& program) {
        execution_state state{ 0, {}, 0 };
        while (state.pos >= 0 && state.pos < program.size()) {
            execute_instruction(state, program.at(state.pos));
        }
        return state.num_mults;
    }
}

void aoc::y2017::day_23(const std::string& title) {

    auto inp = aoc::file_to_string_vector(
            aoc::input_path(2017, 23)
        ) | rv::transform(
            parse_instr
        ) | r::to<std::vector>();

    std::println("--- Day 23: {} ---", title);
    std::println("  part 1: {}", run_program(inp) );
    std::println("  part 2: {}", 0);
    
}
