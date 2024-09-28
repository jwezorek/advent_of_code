
#include "../util/util.h"
#include "y2017.h"
#include <filesystem>
#include <functional>
#include <print>
#include <ranges>
#include <unordered_map>
#include <variant>
#include <queue>

namespace r = std::ranges;
namespace rv = std::ranges::views;

/*------------------------------------------------------------------------------------------------*/

namespace {

    enum op_type {
        snd,
        set,
        add,
        mul,
        mod,
        rcv,
        jgz
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
            {"snd",snd}, {"set",set}, {"add",add}, {"mul",mul},
            {"mod",mod}, {"rcv",rcv}, {"jgz",jgz}
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
    using msg_queue = std::queue<int64_t>;

    struct execution_state {
        int pos;
        reg_map registers;
        msg_queue incoming_messages;
        msg_queue* outgoing_messages;
        int num_sends;
    };

    using op_func = std::function<bool(execution_state&, const arg_var&, const arg_var&)>;

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

    bool do_sound(execution_state& state, const arg_var& arg1, const arg_var& arg2) {
        state.incoming_messages = {};
        state.incoming_messages.push(eval_arg(state, arg1));
        ++state.pos;
        return true;
    }

    bool do_send(execution_state& state, const arg_var& arg1, const arg_var& arg2) {
        state.outgoing_messages->push(eval_arg(state, arg1));
        ++state.num_sends;
        ++state.pos;
        return true;
    }

    bool do_set(execution_state& state, const arg_var& arg1, const arg_var& arg2) {
        assign_to_reg(state, arg1, eval_arg(state, arg2));
        ++state.pos;
        return true;
    }

    bool do_add(execution_state& state, const arg_var& arg1, const arg_var& arg2) {
        auto old_val = eval_arg(state, arg1);
        assign_to_reg(state, arg1, old_val + eval_arg(state, arg2));
        ++state.pos;
        return true;
    }

    bool do_mul(execution_state& state, const arg_var& arg1, const arg_var& arg2) {
        auto old_val = eval_arg(state, arg1);
        assign_to_reg(state, arg1, old_val * eval_arg(state, arg2));
        ++state.pos;
        return true;
    }

    bool do_mod(execution_state& state, const arg_var& arg1, const arg_var& arg2) {
        auto old_val = eval_arg(state, arg1);
        assign_to_reg(state, arg1, old_val % eval_arg(state, arg2));
        ++state.pos;
        return true;
    }

    bool do_recover(execution_state& state, const arg_var& arg1, const arg_var& arg2) {
        if (eval_arg(state, arg1) != 0) {
            ++state.pos;
            return false;
        }
        ++state.pos;
        return true;
    }

    bool do_receive(execution_state& state, const arg_var& arg1, const arg_var& arg2) {
        if (state.incoming_messages.empty()) {
            return false;
        }
        auto msg = state.incoming_messages.front();
        state.incoming_messages.pop();
        assign_to_reg(state, arg1, msg);
        ++state.pos;
        return true;
    }

    bool do_jgz(execution_state& state, const arg_var& arg1, const arg_var& arg2) {
        if (eval_arg(state, arg1) > 0) {
            state.pos += eval_arg(state, arg2);
        } else {
            ++state.pos;
        }
        return true;
    }

    bool execute_instruction(execution_state& state, const instruction& instr) {
        const static std::array<op_func, 7> tbl_part_1 = {{
            do_sound, do_set, do_add, do_mul, do_mod, do_recover, do_jgz
        }};

        const static std::array<op_func, 7> tbl_part_2 = { {
            do_send, do_set, do_add, do_mul, do_mod, do_receive, do_jgz
        } };
        const auto& tbl = (state.outgoing_messages == nullptr) ? tbl_part_1 : tbl_part_2;
        return tbl.at(static_cast<int>(instr.op))(state, instr.arg1, instr.arg2);
    }

    int64_t run_sound_program(const std::vector<instruction>& program) {
        execution_state state{ 0, {}, {}, nullptr, 0 };
        bool executing = true;
        while (executing && state.pos >= 0 && state.pos < program.size()) {
            executing = execute_instruction(state, program.at(state.pos));
        }
        return state.incoming_messages.front();
    }

    bool is_running_duet_prog(const execution_state& state, const std::vector<instruction>& program) {

        if (state.pos < 0 || state.pos >= program.size()) {
            return false;
        }
        auto curr_op = program.at(state.pos).op;
        if (state.incoming_messages.empty() && curr_op == rcv) {
            return false;
        }

        return true;
    }

    int64_t run_duet_program(const std::vector<instruction>& program) {
        execution_state prog_0{ 0, {}, {}, nullptr, 0 };
        execution_state prog_1{ 0, {}, {}, nullptr, 0 };

        prog_0.registers["p"] = 0;
        prog_1.registers["p"] = 1;
        prog_0.outgoing_messages = &prog_1.incoming_messages;
        prog_1.outgoing_messages = &prog_0.incoming_messages;

        while (is_running_duet_prog(prog_0, program) || is_running_duet_prog(prog_1, program)) {
            if (is_running_duet_prog(prog_0, program)) {
                execute_instruction(prog_0, program.at(prog_0.pos));
            }
            if (is_running_duet_prog(prog_1, program)) {
                execute_instruction(prog_1, program.at(prog_1.pos));
            }
        }

        return prog_1.num_sends;
    }
}

void aoc::y2017::day_18(const std::string& title) {

    auto inp = aoc::file_to_string_vector(
            aoc::input_path(2017, 18)
        ) | rv::transform(
            parse_instr
        ) | r::to<std::vector>();

    std::println("--- Day 18: {} ---", title);
    std::println("  part 1: {}", run_sound_program(inp));
    std::println("  part 2: {}", run_duet_program(inp));
    
}
