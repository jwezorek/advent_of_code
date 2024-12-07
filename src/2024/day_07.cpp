
#include "../util/util.h"
#include "y2024.h"
#include <filesystem>
#include <functional>
#include <print>
#include <ranges>
#include <unordered_map>

namespace r = std::ranges;
namespace rv = std::ranges::views;

/*------------------------------------------------------------------------------------------------*/

namespace {

    struct equation {
        int64_t lhs;
        std::vector<int64_t> rhs;
    };

    equation parse_equation(const std::string& str) {
        auto vals = aoc::extract_numbers_int64(str, true);
        return {
            vals.front(),
            vals | rv::drop(1) | r::to<std::vector>()
        };
    }

    enum operation {
        multiply,
        add,
        concat
    };

    const std::vector<operation>& ops(bool all_ops) {
        static const std::vector<operation> all = { concat, multiply, add };
        static const std::vector<operation> mult_and_add = { multiply, add };
        if (all_ops) {
            return all;
        } else {
            return mult_and_add;
        }
    }

    std::optional<int64_t> subtract(int64_t lhs, int64_t rhs) {
        if (lhs - rhs >= 0) {
            return lhs - rhs;
        } else {
            return {};
        }
    }

    std::optional<int64_t> divide(int64_t lhs, int64_t rhs) {
        if (lhs % rhs == 0) {
            return lhs / rhs;
        }else {
            return {};
        }
    }

    int num_digits(int64_t n) {
        if (n < 10) {
            return 1;
        } else if (n == 10) {
            return 2;
        }
        return static_cast<int>(std::ceil(std::log10(n)));
    }

    std::optional<int64_t> unconcatenate(int64_t lhs, int64_t rhs) {
        auto lhs_digits = num_digits(lhs);
        auto rhs_digits = num_digits(rhs);
        if (rhs_digits > lhs_digits) {
            return {};
        }
        int64_t pow_of_10 = r::fold_left(
            rv::repeat(10ll) | rv::take(rhs_digits),
            1ll,
            std::multiplies<int64_t>()
        );
        if (lhs % pow_of_10 != rhs) {
            return {};
        }
        return lhs / pow_of_10;
    }

    std::optional<equation> undo_last_operation(const equation& equ, operation op) {
        using undo_fn = std::function<std::optional<int64_t>(int64_t, int64_t)>;
        const static std::unordered_map<operation, undo_fn> undo = {
            { add, subtract },
            { multiply, divide },
            { concat, unconcatenate }
        };
        auto new_lhs = undo.at(op)(equ.lhs, equ.rhs.back());
        if (!new_lhs) {
            return {};
        }
        return equation{
            *new_lhs,
            equ.rhs | rv::take(equ.rhs.size() - 1) | r::to<std::vector>()
        };
    }

    bool is_solvable_equation(const equation& equ, bool all_ops) {
        if (equ.rhs.size() == 1) {
            return equ.lhs == equ.rhs.front();
        }
        for (auto op : ops(all_ops)) {
            auto new_equ = undo_last_operation(equ, op);
            if (new_equ && is_solvable_equation(*new_equ, all_ops)) {
                return true;
            }
        }
        return false;
    }

    int64_t sum_solved_equations(const std::vector<equation>& equs, bool all_ops) {
        return r::fold_left(
            equs | rv::transform(
                [&](auto&& equ) {
                    return (is_solvable_equation(equ, all_ops)) ? equ.lhs : 0ll;
                }
            ),
            0ll,
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
        sum_solved_equations(inp, false)
    );
    std::println("  part 2: {}", 
        sum_solved_equations(inp, true)
    );
    
}
