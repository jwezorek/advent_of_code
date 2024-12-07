
#include "../util/util.h"
#include "y2024.h"
#include <filesystem>
#include <functional>
#include <print>
#include <ranges>
#include <unordered_set>

namespace r = std::ranges;
namespace rv = std::ranges::views;

/*------------------------------------------------------------------------------------------------*/

namespace {

    struct equation {
        int64_t result;
        std::vector<int64_t> args;
    };

    equation parse_equation(const std::string& str) {
        auto vals = aoc::extract_numbers_int64(str, true);
        return {
            vals.front(),
            vals | rv::drop(1) | r::to<std::vector>()
        };
    }

    enum op {
        multiply = 0,
        add,
        concat
    };

    auto mults_and_adds(int n) {
        uint64_t max = (static_cast<uint64_t>(1) << n);
        return rv::iota(static_cast<uint64_t>(0), max) | rv::transform(
            [n](auto v)->std::vector<op> {
                std::vector<op> args;
                for (int j = 0; j < n; j++) {
                    int digit = 1 << j;
                    args.push_back(v & digit ? multiply : add);
                }
                return args;
            }
        );
    }

    auto all_ops(int n) {
        uint64_t max = static_cast<uint64_t>(std::pow(3, n));
        return rv::iota(static_cast<uint64_t>(0), max) | rv::transform(
            [n](auto number)->std::vector<op> {
                std::vector<int> digits;
                while (number > 0) {
                    digits.push_back(number % 3);
                    number /= 3;
                }
                std::reverse(digits.begin(), digits.end());
                while (digits.size() < static_cast<size_t>(n)) {
                    digits.insert(digits.begin(), 0);
                }
                return digits | rv::transform(
                    [](int v) {
                        return static_cast<op>(v); 
                    }
                ) | r::to<std::vector>();
            }
        );
    }

    int num_digits(int64_t v) {
        if (v < 10) {
            return 1;
        } else if (v == 10) {
            return 2;
        }
        return static_cast<int>(std::ceil(std::log10(v)));
    }

    int64_t concatenate(int64_t lhs, int64_t rhs) {
        int64_t pow_of_10 = std::pow(10.0, static_cast<double>(num_digits(rhs)));
        return rhs + pow_of_10 * lhs;
    }

    int64_t eval_equation(const std::vector<int64_t>& args, const std::vector<op>& ops) {
        return r::fold_left(
            rv::zip(ops, args | rv::drop(1)),
            args.front(),
            [](int64_t lhs, auto&& rhs)->int64_t {
                const auto& [op, arg] = rhs;
                switch (op) {
                    case multiply:
                        return lhs * arg;
                    case add:
                        return lhs + arg;
                    case concat:
                        return concatenate(lhs, arg);
                }
            }
       );
    }

    bool is_solvable_mult_and_add(const equation& eq) {
        for (auto operations : mults_and_adds(eq.args.size() - 1)) {
            auto result = eval_equation(eq.args, operations);
            if (result == eq.result) {
                return true;
            }
        }
        return false;
    }

    bool is_solvable_all_ops(const equation& eq) {
        for (auto operations : all_ops(eq.args.size() - 1)) {
            auto result = eval_equation(eq.args, operations);
            if (result == eq.result) {
                return true;
            }
        }
        return false;
    }

    using is_solvable_fn = std::function<int64_t(const equation&)>;

    int64_t sum_solved_equations(const std::vector<equation>& equs, 
            const is_solvable_fn& is_solvable) {
        return r::fold_left(
            equs | rv::transform(
                [&](auto&& equ) {
                    return (is_solvable(equ)) ? equ.result : 0ll;
                }
            ),
            static_cast<int64_t>(0),
            std::plus<int64_t>()
        );
    }
}

void aoc::y2024::day_07(const std::string& title) {

    auto inp = aoc::file_to_string_vector(
            aoc::input_path(2024, 7)
        ) | rv::transform(
            parse_equation
        ) | r::to<std::vector>();
    
    std::println("--- Day 7: {} ---", title);
    std::println("  part 1: {}", 
        sum_solved_equations(inp, is_solvable_mult_and_add)
    );
    std::println("  part 2: {}", 
        sum_solved_equations(inp, is_solvable_all_ops)
    );
    
}
