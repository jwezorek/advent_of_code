
#include "../util/util.h"
#include "y2016.h"
#include <filesystem>
#include <functional>
#include <print>
#include <ranges>
#include <unordered_set>
#include <variant>
#include <unordered_map>

namespace r = std::ranges;
namespace rv = std::ranges::views;

/*------------------------------------------------------------------------------------------------*/

namespace {
    enum op {
        cpy,
        inc,
        dec,
        jnz
    };

    using operand = std::variant<std::monostate, int64_t, char>;

    struct statement {
        op op;
        std::vector<operand> args;
    };

    struct state {
        int instr_ptr;
        std::unordered_map<char, int64_t> registers;

        state() : instr_ptr(0) {
            registers['a'] = 0;
            registers['b'] = 0;
            registers['c'] = 0;
            registers['d'] = 0;
        }
    };

    operand str_to_operand(const std::string& str) {
        if (std::isalpha(str.front())) {
            return { str.front() };
        }
        return aoc::string_to_int64(str);
    }

    statement parse_statement(const std::string& str) {
        static const std::unordered_map<std::string, op> op_tbl = {
            {"cpy", cpy},
            {"inc", inc},
            {"dec", dec},
            {"jnz", jnz}
        };

        auto parts = str | rv::split(' ') | rv::transform(
            [](auto rng) {
                return rng | r::to<std::string>();
            }
        ) | r::to<std::vector>();

        return {
            op_tbl.at(parts.front()),
            parts | rv::drop(1) | rv::transform(
                str_to_operand
            ) | r::to<std::vector>()
        };
    }

    int64_t eval_operand(const state& state, const operand& operand) {
        return std::visit(
            aoc::overload{
                [&](char reg)->int64_t {
                    return state.registers.at(reg);
                },
                [](int64_t val)->int64_t {
                    return val;
                },
                [](std::monostate)->int64_t {
                    throw std::runtime_error("bad operand");
                }
            },
            operand
        );
    }

    int64_t& eval_operand_as_ref(state& state, const operand& operand) {
        return state.registers[std::get<char>(operand)];
    }

    using op_fn = std::function<void(state&, const operand&, const operand&)>;

    void do_inc(state& state, const operand& x, const operand& y) {
        ++eval_operand_as_ref(state, x);
    }

    void do_dec(state& state, const operand& x, const operand& y) {
        --eval_operand_as_ref(state, x);
    }

    void do_jnz(state& state, const operand& x, const operand& y) {
        if (eval_operand(state, x) != 0) {
            state.instr_ptr += eval_operand(state, y);
        }
    }

    void do_cpy(state& state, const operand& x, const operand& y) {
        eval_operand_as_ref(state, y) = eval_operand(state, x);
    }

    void exec_statement(state& state, const statement& stmt) {
        const static std::unordered_map<op, op_fn> op_tbl = {
            {cpy, do_cpy},
            {inc, do_inc},
            {dec, do_dec},
            {jnz, do_jnz}
        };

        auto arg1 = stmt.args[0];
        auto arg2 = (stmt.args.size() > 1) ? stmt.args[1] : operand{};
        op_tbl.at(stmt.op)( state, arg1, arg2);
    }

    state run_program(const state& inp, const std::vector<statement>& program) {
        state state = inp;
        while (state.instr_ptr < program.size()) {
            auto old_instr_ptr = state.instr_ptr;
            exec_statement(state, program.at(state.instr_ptr));
            if (state.instr_ptr == old_instr_ptr) {
                ++state.instr_ptr;
            }
        }
        return state;
    }

    int do_part_1(const std::vector<statement>& program) {
        auto state = run_program({},program);
        return state.registers['a'];
    }

    int do_part_2(const std::vector<statement>& program) {
        state inp;
        inp.registers['c'] = 1;
        auto state = run_program(inp, program);
        return state.registers['a'];
    }
}

void aoc::y2016::day_12(const std::string& title) {

    auto inp = aoc::file_to_string_vector(
            aoc::input_path(2016, 12)
        ) | rv::transform(
            parse_statement
        ) | r::to<std::vector>();

    std::println("--- Day 12: {} ---", title);
    std::println("  part 1: {}", do_part_1(inp) );
    std::println("  part 2: {}", do_part_2(inp) );
    
}
