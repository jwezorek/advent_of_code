
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

    enum op {
        multiply,
        add,
        concat
    };

    auto ops(bool all_ops) {
        static const std::vector<op> all = { concat, multiply, add };
        static const std::vector<op> mult_and_add = { multiply, add };
        if (all_ops) {
            return rv::all( all );
        } else {
            return rv::all( mult_and_add );
        }
    }

    std::optional<int64_t> unconcatenate(int64_t lhs, int64_t rhs) {
        auto lhs_str = std::to_string(lhs);
        auto rhs_str = std::to_string(rhs);
        if (rhs_str.size() > lhs_str.size()) {
            return {};
        }
        auto n = lhs_str.size() - rhs_str.size();
        if (lhs_str.substr(n) != rhs_str) {
            return {};
        }
        auto result = lhs_str.substr(0, n);
        return aoc::string_to_int64(result);
    }

    bool is_solvable(const equation & eq, const auto & enum_ops) {
        for (auto operations : enum_ops(eq.rhs.size() - 1)) {
            if (eval_equation(eq.lhs, eq.rhs, operations)) {
                return true;
            }
        }
        return false;
    }

    std::optional<int64_t> new_lhs(int64_t old_lhs, int64_t rhs, op op) {
        std::optional<int64_t> result;
        switch (op) {
            case add:
                if (old_lhs - rhs >= 0) {
                    return old_lhs - rhs;
                }
                else {
                    return {};
                }
                break;
            case multiply:
                if (old_lhs % rhs == 0) {
                    return old_lhs / rhs;
                } else {
                    return {};
                }
                break;
            case concat: 
                return unconcatenate(old_lhs, rhs);
        }
    }

    std::optional<equation> reduce_equation(const equation& equ, op op) {
        auto lhs = new_lhs(equ.lhs, equ.rhs.back(), op);
        if (!lhs) {
            return {};
        }
        return equation{
            *lhs,
            equ.rhs | rv::take(equ.rhs.size() - 1) | r::to<std::vector>()
        };
    }

    bool is_solvable_equation(const equation& equ, bool all_ops) {
        if (equ.rhs.size() == 1) {
            return equ.lhs == equ.rhs.front();
        }
        for (auto op : ops(all_ops)) {
            auto new_equ = reduce_equation(equ, op);
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
