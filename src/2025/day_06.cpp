
#include "../util/util.h"
#include "y2025.h"
#include <filesystem>
#include <functional>
#include <print>
#include <ranges>
#include <unordered_set>

namespace r = std::ranges;
namespace rv = std::ranges::views;

/*------------------------------------------------------------------------------------------------*/

namespace {
    struct problem {
        std::vector<int64_t> args;
        bool is_addition;
    };

    std::vector<problem> parse_input(const std::vector<std::string>& inp) {
        auto args_matrix = inp | rv::take(inp.size() - 1) | rv::transform([](auto&& str) {
            return aoc::extract_numbers_int64(str, true);
                })
            | r::to<std::vector>();

        auto ops = aoc::split(aoc::collapse_whitespace(inp.back()), ' ');
        int n = static_cast<int>(args_matrix.front().size());

        return rv::iota(0, n) | 
            rv::transform(
                [&](int i) -> problem {
                    return {
                        args_matrix | rv::transform(
                            [&](const auto& row) { 
                                return row.at(i); 
                            }
                        ) | r::to<std::vector>(),


                     ops[i] == "+"
                    };
                }
            ) | r::to<std::vector>();
    }

    using eval_problem_fn = std::function<int64_t(const problem&)>;

    int64_t simple_eval(const problem& p) {
        std::function<int64_t(int64_t, int64_t)> op;
        if (p.is_addition) {
            op = std::plus<int64_t>();
        } else {
            op = std::multiplies<int64_t>();
        }

        int64_t start_val = p.is_addition ? 0 : 1;

        return r::fold_left(
            p.args,
            start_val,
            op
        );
    }

    int64_t sum_of_columns(const std::vector<problem>& inp, eval_problem_fn fn) {
        return r::fold_left(
            inp | rv::transform(fn),
            0,
            std::plus<int64_t>()
        );
    }

    int64_t eval_part_2(const problem& p) {
        auto strings = p.args | rv::transform(
                [](auto v) {return std::to_string(v); }
            ) | r::to<std::vector>();
        auto max_digits = r::max(strings | rv::transform([](auto&& s) {return s.size(); }));
        return 0;
    }
}

void aoc::y2025::day_06(const std::string& title) {

    auto inp = parse_input(
        aoc::file_to_string_vector(
            aoc::input_path(2025, 6)
        )
    );

    std::println("--- Day 6: {} ---", title);
    std::println("  part 1: {}", sum_of_columns(inp, simple_eval) );
    std::println("  part 2: {}", sum_of_columns(inp, eval_part_2));
    
}
