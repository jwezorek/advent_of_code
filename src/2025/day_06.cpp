
#include "../util/util.h"
#include "y2025.h"
#include <filesystem>
#include <functional>
#include <print>
#include <ranges>
#include <sstream>

namespace r = std::ranges;
namespace rv = std::ranges::views;

/*------------------------------------------------------------------------------------------------*/

namespace {

    struct column_loc {
        int x;
        int wd;
    };

    struct column {
        std::vector<std::string> args;
        bool is_addition;
    };

    std::vector<column_loc> column_locations(const std::string& inp) {
        std::vector<column_loc> columns;
        int prev = 0;
        for (const auto& [i, ch] : rv::enumerate(inp)) {
            if (ch == ' ') {
                continue;
            }
            if (!columns.empty()) {
                columns.back().wd = i - columns.back().x - 1;
            }
            columns.emplace_back(i, 0);
        }
        columns.back().wd = inp.size() - columns.back().x;
        return columns;
    }

    std::vector<column> parse_input(const std::vector<std::string>& inp) {
        std::string operations = inp.back();
        auto cols = column_locations(operations);
        auto arg_mat = inp | rv::take(inp.size() - 1) | r::to<std::vector>();

        return cols | rv::transform(
                [&](const column_loc& col)->column {
                    return {
                        arg_mat | rv::transform(
                            [&](auto&& row) {
                                return row.substr(col.x, col.wd);
                            }
                        ) | r::to<std::vector>(),
                        operations.at(col.x) == '+'
                    };
                }
            ) | r::to<std::vector>();
    }

    using eval_column_fn = std::function<int64_t(const column&)>;

    int64_t eval_column_horz(const column& p) {
        auto op = [&](int64_t lhs, const std::string& item)->int64_t {
            int64_t rhs = aoc::string_to_int64(item);
            if (p.is_addition) {
                return lhs + rhs;
            } else {
                return lhs * rhs;
            }
        };

        int64_t start_val = p.is_addition ? 0 : 1;

        return r::fold_left(
            p.args,
            start_val,
            op
        );
    }

    int64_t eval_column_vert(const column& p) {
        int n = static_cast<int>(p.args.front().size());
        auto transposed = column{
            rv::iota(0, n) |
            rv::transform(
                [&](auto i)->std::string {
                    return p.args | rv::transform(
                        [&](const auto& row)->char {
                            return row.at(i);
                        }
                    ) | r::to<std::string>();
                }
            ) | r::to<std::vector>(),
            p.is_addition
        };
        return eval_column_horz(transposed);
    }

    int64_t sum_of_columns(const std::vector<column>& inp, eval_column_fn fn) {
        return r::fold_left(
            inp | rv::transform(fn),
            0,
            std::plus<int64_t>()
        );
    }

}

void aoc::y2025::day_06(const std::string& title) {

    auto inp = parse_input(
        aoc::file_to_string_vector(
            aoc::input_path(2025, 6)
        )
    );

    std::println("--- Day 6: {} ---", title);
    std::println("  part 1: {}", sum_of_columns(inp, eval_column_horz) );
    std::println("  part 2: {}", sum_of_columns(inp, eval_column_vert));
    
}
