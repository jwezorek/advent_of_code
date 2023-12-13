#include "days.h"
#include "../util.h"
#include <print>
#include <ranges>
#include <numeric>
#include <algorithm>
#include <tuple>

namespace r = std::ranges;
namespace rv = std::ranges::views;

namespace {
    using grid = std::vector<std::string>;

    std::vector<grid> parse_input(const std::vector<std::string>& input) {
        return input | rv::chunk_by(
            [](const std::string& str1, const std::string& str2)->bool {
                return str1.size() != 0 && str2.size() != 0;
            }
        ) | rv::transform(
            [](auto rng)->std::vector<std::string> {
                return rng | r::to<std::vector<std::string>>();
            }
        ) | rv::filter(
            [](auto&& vec) {
                return vec.size() > 1;
            }
            ) | r::to<std::vector<grid>>();
    }

    std::tuple<int, int> dimensions(const grid& g) {
        return {
            static_cast<int>(g.front().size()),
            static_cast<int>(g.size())
        };
    }

    grid make_grid(int cols, int rows) {
        return grid(rows, std::string(cols, '.'));
    }

    grid transpose(const grid& g) {
        auto [cols, rows] = dimensions(g);
        auto transposed = make_grid(rows, cols);
        for (int row = 0; row < rows; ++row) {
            for (int col = 0; col < cols; ++col) {
                transposed[col][row] = g[row][col];
            }
        }
        return transposed;
    }

    bool is_reflection_row(int row, const grid& g) {
        int i = row;
        int j = row + 1;
        if (j == g.size()) {
            return false;
        }
        while (g[i] == g[j]) {
            --i;
            ++j;
            if (i < 0 || j == g.size()) {
                return true;
            }
        }
        return false;
    }

    std::optional<int> find_reflection_rows(const grid& g, std::optional<int> old = {}) {
        auto vec = rv::iota(0, static_cast<int>(g.size())) | rv::filter(
                [&](int row) {
                    return is_reflection_row(row, g);
                }
            ) | r::to<std::vector<int>>();
        if (vec.empty()) {
            return {};
        }
        if (vec.size() == 1) {
            return vec.front();
        }
        for (auto row : vec) {
            if (row != *old) {
                return row;
            }
        }
        return {};
    }

    std::optional<int> find_reflection_cols(const grid& g, std::optional<int> old = {}) {
        auto transposed = transpose(g);
        return find_reflection_rows(transposed, old);
    }

    std::tuple<int, int> get_scores(const grid& g, std::optional<int> old_row = {}, std::optional<int> old_col = {}) {
        auto row = find_reflection_rows(g, old_row);
        auto col = find_reflection_cols(g, old_col);

        auto row_val = (row) ? 100 * (*row + 1) : 0;
        auto col_val = (col) ? *col + 1 : 0;

        return { col_val, row_val };
    }

    int summarize_pattern(const grid& g) {
        auto [col_val, row_val] = get_scores(g);
        return row_val + col_val;
    }

    int summarize_pattern_with_smudge(const grid& g) {

        auto old_row = find_reflection_rows(g);
        auto old_col = find_reflection_cols(g);
        auto [old_col_val, old_row_val] = get_scores(g);
        auto [cols, rows] = dimensions(g);
        for (int row = 0; row < rows; ++row) {
            for (int col = 0; col < cols; ++col) {
                auto smudge_grid = g;
                smudge_grid[row][col] = (g[row][col] == '.') ? '#' : '.';
                auto [new_col_val, new_row_val] = get_scores(smudge_grid, old_row, old_col);
                if (new_col_val == 0 && new_row_val == 0) {
                    continue;
                }
                if (new_col_val == old_col_val && new_row_val != old_row_val) {
                    return new_row_val;
                } 
                if (new_row_val == old_row_val && new_col_val != old_col_val) {
                    return new_col_val;
                }
            }
        }

        throw std::runtime_error("something is wrong");
    }
}

/*------------------------------------------------------------------------------------------------*/

void aoc::y2023::day_13(const std::string& title) {

    auto input = parse_input(aoc::file_to_string_vector(aoc::input_path(2023, 13)));

    //auto foo = summarize_pattern_with_smudge(input.back());

    std::println("--- Day 13: {0} ---\n", title);
    std::println("  part 1: {}",
        r::fold_left(
            input | rv::transform( summarize_pattern),
            0, std::plus<>()
        )
    );
    
    std::println("  part 2: {}",
        r::fold_left(
            input | rv::transform(summarize_pattern_with_smudge),
            0, std::plus<>()
        )
    );
}