#include "days.h"
#include "../util.h"
#include <print>
#include <ranges>
#include <numeric>
#include <algorithm>
#include <tuple>
#include <set>

namespace r = std::ranges;
namespace rv = std::ranges::views;

namespace {
    struct loc {
        int col;
        int row;
    };

    std::tuple<int, int> dimensions(const std::vector<std::string>& inp) {
        return {
            static_cast<int>(inp.front().size()),
            static_cast<int>(inp.size())
        };
    }

    std::vector<int> bound(const std::vector<int>& vals, int max) {
        std::vector<int> bounded = { -1 };
        r::copy(vals, std::back_inserter(bounded));
        bounded.emplace_back(max);
        return bounded;
    }

    std::vector<int> empty_rows(const std::vector<std::string>& inp) {
        auto [cols, rows] = dimensions(inp);
        return bound(
            rv::enumerate(inp) | rv::filter(
                    [](auto&& i_row)->bool {
                        const auto& row = std::get<1>(i_row);
                        return row.find('#') == std::string::npos;
                    }
                ) | rv::transform(
                    [](auto&& i_row)->int {
                        return std::get<0>(i_row);
                    }
                ) | r::to<std::vector<int>>(),
            rows
        );
    }

    std::vector<int> empty_cols(const std::vector<std::string>& inp) {
        auto [cols, rows] = dimensions(inp);
        return bound(
            rv::iota(0, cols) | rv::filter(
                    [&](int col)->bool {
                        auto column = rv::iota(0, rows) |
                            rv::transform(
                                [&](int row)->char {
                                    return inp.at(row).at(col);
                                }
                        ) | r::to<std::string>();
                        return column.find('#') == std::string::npos;
                    }
                ) | r::to<std::vector<int>>(),
            cols
        );
    }

    std::tuple<std::vector<loc>, std::vector<int>, std::vector<int>> parse_input(
            const std::vector<std::string>& inp) {
        std::vector<loc> stars;
        for (const auto& [row_index, row] : rv::enumerate(inp)) {
            for (auto [col, ch] : rv::enumerate(row)) {
                if (inp[row_index][col] == '#') {
                    stars.emplace_back(col, row_index);
                }
            }
        }
        return {
            std::move(stars),
            empty_rows(inp),
            empty_cols(inp)
        };
    }

    int count_between(const std::vector<int>& vals, int u, int v) {
        auto iter_u = r::lower_bound(vals, u);
        auto iter_v = r::lower_bound(vals, v);
        return std::abs(iter_v - iter_u);
    }

    int manhattan_distance(const loc& u, const loc& v) {
        return std::abs(u.col - v.col) + std::abs(u.row - v.row);
    }

    int expanded_distance(const loc& u, const loc& v,
            const std::vector<int>& blank_rows,
            const std::vector<int>& blank_cols) {

        int dist = manhattan_distance(u, v);
        int extra_rows = count_between(blank_rows, u.row, v.row);
        int extra_cols = count_between(blank_cols, u.col, v.col);

        return dist + extra_rows + extra_cols;
    }

    int do_part_1(const std::vector<loc>& stars, const std::vector<int>& blank_rows,
            const std::vector<int>& blank_cols) {
        int n = static_cast<int>(stars.size());
        int sum = 0;

        for (int i = 0; i < n - 1; ++i) {
            for (int j = i + 1; j < n; ++j) {
                sum += expanded_distance(
                    stars[i], stars[j], blank_rows, blank_cols
                );
            }
        }
        
        return sum;
    }
}

/*------------------------------------------------------------------------------------------------*/

void aoc::y2023::day_11(const std::string& title) {

    const auto& [stars, blank_rows, blank_cols] = parse_input(
        aoc::file_to_string_vector(aoc::input_path(2023, 11))
    );


    std::println("--- Day 11: {0} ---\n", title);
    std::println("  part 1: {}",
        do_part_1(stars, blank_rows, blank_cols)
    );
}