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

    std::vector<int> find_reflection_rows(const grid& g) {
        return rv::iota(0, static_cast<int>(g.size())) | rv::filter(
                [&](int row) {
                    return is_reflection_row(row, g);
                }
            ) | r::to<std::vector<int>>();
    }

    std::vector<int> find_reflection_cols(const grid& g) {
        auto transposed = transpose(g);
        return find_reflection_rows(transposed);
    }

    int summarize_pattern(const grid& g) {
        auto rows = find_reflection_rows(g);
        auto cols = find_reflection_cols(g);
        return r::fold_left(
                rows | rv::transform( [](int row)->int { return 100 * (row + 1); }), 
                0, 
                std::plus<>()) +
             r::fold_left(cols | rv::transform( [](int col)->int {  return col + 1; } ), 
                 0, 
                 std::plus<>()
             );
    }
}

/*------------------------------------------------------------------------------------------------*/

void aoc::y2023::day_13(const std::string& title) {

    auto input = parse_input(aoc::file_to_string_vector(aoc::input_path(2023, 13)));
    auto foo = find_reflection_rows(input.front());

    std::println("--- Day 13: {0} ---\n", title);
    std::println("  part 1: {}",
        r::fold_left(
            input | rv::transform( summarize_pattern),
            0, std::plus<>()
        )
    );
}