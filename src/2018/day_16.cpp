
#include "../util/util.h"
#include "y2018.h"
#include <filesystem>
#include <functional>
#include <print>
#include <ranges>
#include <unordered_set>
#include <unordered_map>

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
        op_code code;
        op_fn func;
        bool a_is_reg;
        bool b_is_reg;
    };

    using quadruple = std::array<int, 4>;

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
        {addr, add, true, true},
        {addi, add, true, false},
        {mulr, mul, true, true},
        {muli, mul, true, false},
        {banr, ban, true, true},
        {bani, ban, true, false},
        {borr, bor, true, true},
        {bori, bor, true, false},
        {setr, set, true, false},
        {seti, set, false, false},
        {gtir,  gt, false, true},
        {gtri,  gt, true, false},
        {gtrr,  gt, true, true},
        {eqir,  eq, false, true},
        {eqri,  eq, true, false},
        {eqrr,  eq, true, true}
    } };

    quadruple perform_op(const quadruple& inp_state, op_code op, int a, int b, int c) {

        static std::unordered_map<op_code, op_info> tbl;
        if (tbl.empty()) {
            tbl = g_ops | rv::transform(
                [](const auto& oi)->std::unordered_map<op_code, op_info>::value_type {
                    return { oi.code, oi };
                }
            ) | r::to<std::unordered_map<op_code, op_info>>();
        }

        auto state = inp_state;
        const auto& info = tbl.at(op);
        int arg_a = info.a_is_reg ? state.at(a) : a;
        int arg_b = info.b_is_reg ? state.at(b) : b;
        state.at(c) = info.func(arg_a, arg_b);

        return state;
    }

    struct test {
        quadruple before;
        quadruple instruction;
        quadruple after;
    };

    quadruple to_quadruple(const std::string& str) {
        auto vals = aoc::extract_numbers(str);
        return { {vals[0],vals[1],vals[2],vals[3]} };
    }

    std::vector<test> parse_tests(const std::vector<std::string>& inp) {
        auto test_strings = inp | rv::filter(
            [](auto&& str) {
                return !str.empty();
            }
        ) | r::to<std::vector>();

        return test_strings | rv::chunk(3) | rv::transform(
            [](auto rng)->test {
                return {
                    to_quadruple(rng[0]),
                    to_quadruple(rng[1]),
                    to_quadruple(rng[2])
                };
            }
        ) | r::to<std::vector>();
    }

    std::vector<quadruple> parse_code(const std::vector<std::string>& inp) {
        return inp | rv::filter(
            [](auto&& str) {
                return !str.empty();
            }
        ) | rv::transform(
            to_quadruple
        ) | r::to<std::vector>();
    }

    std::tuple<std::vector<test>, std::vector<quadruple>> parse(
        const std::vector<std::string>& inp) {
        auto n = static_cast<int>(inp.size());
        auto i = *r::find_if(
            rv::iota(1, n - 1),
            [&](auto i) {
                return inp.at(i - 1).empty() &&
                    inp.at(i).empty() &&
                    inp.at(i + 1).empty();
            }
        );

        return {
            parse_tests(inp | rv::take(i) | r::to<std::vector>()),
            parse_code(inp | rv::drop(i) | r::to<std::vector>())
        };
    }

    bool run_one_test(const test& t, op_code op) {
        const auto& instr = t.instruction;
        auto result = perform_op(t.before, op, instr[1], instr[2], instr[3]);
        for (auto [test, answer] : rv::zip(result, t.after)) {
            if (test != answer) {
                return false;
            }
        }
        return true;
    }

    int do_part_1(const std::vector<test> tests) {
        return r::count_if(
            tests,
            [](auto&& test) {
                auto num_possible_ops = r::count_if(
                    op_codes(),
                    [&](auto op) { return run_one_test(test, op); }
                );
                return num_possible_ops >= 3;
            }
        );
    }

    using code_set_tbl = std::unordered_map<int, std::unordered_set<op_code>>;
    using op_code_tbl = std::unordered_map<int, op_code>;

    code_set_tbl values_to_possible_codes(
            const std::vector<test>& tests) {

        std::unordered_map<int, std::unordered_set<op_code>> possible_codes;
        for (const auto& test : tests) {
            for (auto op : op_codes()) {
                if (run_one_test(test, op)) {
                    possible_codes[test.instruction[0]].insert(op);
                }
            }
        }
        return possible_codes;
    }

    std::tuple<int, op_code> find_one_op_code_val(const code_set_tbl& val_to_ops) {
        for (const auto& [val, op_set] : val_to_ops) {
            if (op_set.size() == 1) {
                return { val, *op_set.begin() };
            }
        }
        throw std::runtime_error("oh well");
    }

    void remove_from_possibilities(code_set_tbl& tbl, int val, op_code code) {
        tbl.erase(val);
        for (auto& item : tbl) {
            if (item.second.contains(code)) {
                item.second.erase(code);
            }
        }
    }

    op_code_tbl figure_out_op_code_values(const std::vector<test>& tests) {
        op_code_tbl tbl;
        auto val_to_ops = values_to_possible_codes(tests);

        while (tbl.size() < 16) {
            auto [val, code] = find_one_op_code_val(val_to_ops);
            tbl[val] = code;
            remove_from_possibilities(val_to_ops, val, code);
        }

        return tbl;
    }

    int run_code(const op_code_tbl& op_tbl, const std::vector<quadruple>& code) {
        quadruple registers = { {0,0,0,0} };
        for (const auto& instr : code) {
            registers = perform_op(registers, op_tbl.at(instr[0]), instr[1], instr[2], instr[3]);
        }
        return registers[0];
    }

    int do_part_2(const std::vector<test>& tests, const std::vector<quadruple>& code) {
        auto val_to_op = figure_out_op_code_values(tests);
        return run_code(val_to_op, code);
    }
}

void aoc::y2018::day_16(const std::string& title) {

    auto [tests, code] = parse(
        aoc::file_to_string_vector(
            aoc::input_path(2018, 16)
        )
    );

    std::println("--- Day 16: {} ---", title);
    std::println("  part 1: {}", do_part_1( tests) );
    std::println("  part 2: {}", do_part_2( tests, code) );
    
}
