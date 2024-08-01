
#include "../util/util.h"
#include "y2018.h"
#include <filesystem>
#include <functional>
#include <print>
#include <ranges>
#include <unordered_set>

namespace r = std::ranges;
namespace rv = std::ranges::views;

/*------------------------------------------------------------------------------------------------*/

namespace {
    using op_fn = std::function<int(int, int)>;

    enum op_code {
        addr = 0,
        addi,
        mulr,
        muli,
        banr,
        bani,
        borr,
        bori,
        setr,
        seti,
        gtir,
        gtri,
        gtrr,
        eqir,
        eqri,
        eqrr
    };

    auto op_codes() {
        return rv::iota(0, 16) | rv::transform(
            [](auto i) { return static_cast<op_code>(i); }
        );
    }

    struct op_info {
        std::string op_str;
        op_code code;
        op_fn func;
        bool a_is_reg;
        bool b_is_reg;
    };

    using code_set_tbl = std::unordered_map<int, std::unordered_set<op_code>>;
    using op_code_tbl = std::unordered_map<int, op_code>;
    using register_state = std::array<int, 6>;

    int add(int a, int b) {
        return a + b;
    }

    int mul(int a, int b) {
        return a * b;
    }

    int ban(int a, int b) {
        return a & b;
    }

    int bor(int a, int b) {
        return a | b;
    }

    int set(int a, int b) {
        return a;
    }

    int gt(int a, int b) {
        return (a > b) ? 1 : 0;
    }

    int eq(int a, int b) {
        return (a == b) ? 1 : 0;
    }

    const std::array<op_info, 16> g_ops = { {
        {"addr", addr, add, true, true},
        {"addi", addi, add, true, false},
        {"mulr", mulr, mul, true, true},
        {"muli", muli, mul, true, false},
        {"banr", banr, ban, true, true},
        {"bani", bani, ban, true, false},
        {"borr", borr, bor, true, true},
        {"bori", bori, bor, true, false},
        {"setr", setr, set, true, false},
        {"seti", seti, set, false, false},
        {"gtir", gtir,  gt, false, true},
        {"gtri", gtri,  gt, true, false},
        {"gtrr", gtrr,  gt, true, true},
        {"eqir", eqir,  eq, false, true},
        {"eqri", eqri,  eq, true, false},
        {"eqrr", eqrr,  eq, true, true}
    } };

    struct instruction {
        op_code op; 
        int a; 
        int b; 
        int c;
    };

    register_state perform_op(const register_state& inp_state, const instruction& instr) {

        static std::unordered_map<op_code, op_info> tbl;
        if (tbl.empty()) {
            tbl = g_ops | rv::transform(
                [](const auto& oi)->std::unordered_map<op_code, op_info>::value_type {
                    return { oi.code, oi };
                }
            ) | r::to<std::unordered_map<op_code, op_info>>();
        }

        auto state = inp_state;
        const auto& info = tbl.at(instr.op);
        int arg_a = info.a_is_reg ? state.at(instr.a) : instr.a;
        int arg_b = info.b_is_reg ? state.at(instr.b) : instr.b;
        state.at(instr.c) = info.func(arg_a, arg_b);

        return state;
    }

    struct program {
        int instr_ptr_reg;
        std::vector<instruction> code;
    };

    program parse(const std::vector<std::string>& inp) {
        auto ip_declaration = aoc::extract_numbers(inp.front()).front();
        static std::unordered_map<std::string, op_code> str_to_op;
        if (str_to_op.empty()) {
            str_to_op = g_ops | rv::transform(
                [](auto&& info)->std::unordered_map<std::string, op_code>::value_type {
                    return { info.op_str, info.code };
                }
            ) | r::to<std::unordered_map<std::string, op_code>>();
        }
        return {
            ip_declaration,
            inp | rv::drop(1) | rv::transform(
                    [](auto&& str)->instruction {
                        auto words = aoc::split(str,' ');
                        return {
                            str_to_op.at(words[0]),
                            std::stoi(words[1]),
                            std::stoi(words[2]),
                            std::stoi(words[3])
                        };
                    }
                ) | r::to<std::vector>()
        };
    }

    int run_program(const program& prog) {
        int ip = 0;
        register_state state = {{0,0,0,0,0,0}};
        while (ip < prog.code.size()) {
            state[prog.instr_ptr_reg] = ip;
            state = perform_op(state, prog.code.at(ip));
            ip = state[prog.instr_ptr_reg];
            ip++;
        }
        return state[0];
    }

    int sum_of_divisors(int n) {
        int sum = 0;
        for (int i = 1; i <= n; ++i) {
            if (n % i == 0) {
                sum += i;
            }
        }
        return sum;
    }

    int do_part_2(const program& prog) {
        int ip = 0;
        register_state state = { {1,0,0,0,0,0} };
        int last_ip = 0;
        auto bignum = 0;
        while (ip < prog.code.size()) {
            if (last_ip > ip) {
                bignum = r::max(state);
                break;
            }
            state[prog.instr_ptr_reg] = ip;
            state = perform_op(state, prog.code.at(ip));
            last_ip = ip;
            ip = state[prog.instr_ptr_reg];
            ip++;
        }
        return sum_of_divisors(bignum);
    }
}

void aoc::y2018::day_19(const std::string& title) {

    auto inp_program = parse(
        aoc::file_to_string_vector(
            aoc::input_path(2018, 19)
        )
    );

    std::println("--- Day 19: {} ---", title);
    std::println("  part 1: {}", run_program(inp_program));
    std::println("  part 2: {}", do_part_2(inp_program) );
    
}
