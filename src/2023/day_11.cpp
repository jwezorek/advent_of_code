#include "y2023.h"
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
        int64_t col;
        int64_t row;
    };

    std::tuple<int64_t, int64_t> dimensions(const std::vector<std::string>& inp) {
        return {
            static_cast<int64_t>(inp.front().size()),
            static_cast<int64_t>(inp.size())
        };
    }

    std::vector<int64_t> bound(const std::vector<int64_t>& vals, int64_t max) {
        std::vector<int64_t> bounded = { -1 };
        r::copy(vals, std::back_inserter(bounded));
        bounded.emplace_back(max);
        return bounded;
    }

    std::vector<int64_t> empty_rows(const std::vector<std::string>& inp) {
        auto [cols, rows] = dimensions(inp);
        return bound(
            rv::enumerate(inp) | rv::filter(
                    [](auto&& i_row)->bool {
                        const auto& row = std::get<1>(i_row);
                        return row.find('#') == std::string::npos;
                    }
                ) | rv::transform(
                    [](auto&& i_row)->int64_t {
                        return std::get<0>(i_row);
                    }
                ) | r::to<std::vector<int64_t>>(),
            rows
        );
    }

    std::vector<int64_t> empty_cols(const std::vector<std::string>& inp) {
        auto [cols, rows] = dimensions(inp);
        return bound(
            rv::iota(0, cols) | rv::filter(
                    [&](int64_t col)->bool {
                        auto column = rv::iota(0, rows) |
                            rv::transform(
                                [&](int64_t row)->char {
                                    return inp.at(row).at(col);
                                }
                        ) | r::to<std::string>();
                        return column.find('#') == std::string::npos;
                    }
                ) | r::to<std::vector<int64_t>>(),
            cols
        );
    }

    std::tuple<std::vector<loc>, std::vector<int64_t>, std::vector<int64_t>> parse_input(
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

    int64_t count_between(const std::vector<int64_t>& vals, int64_t u, int64_t v) {
        auto iter_u = r::lower_bound(vals, u);
        auto iter_v = r::lower_bound(vals, v);
        return std::abs(iter_v - iter_u);
    }

    int64_t manhattan_distance(const loc& u, const loc& v) {
        return std::abs(u.col - v.col) + std::abs(u.row - v.row);
    }

    int64_t expanded_distance(const loc& u, const loc& v,
            const std::vector<int64_t>& blank_rows,
            const std::vector<int64_t>& blank_cols,
            int64_t scale) {

        int64_t dist = manhattan_distance(u, v);
        int64_t extra_rows = count_between(blank_rows, u.row, v.row);
        int64_t extra_cols = count_between(blank_cols, u.col, v.col);

        return dist + extra_rows * (scale - 1) + extra_cols * (scale - 1);
    }

    int64_t sum_of_distances(const std::vector<loc>& stars, const std::vector<int64_t>& blank_rows,
            const std::vector<int64_t>& blank_cols, int64_t scale) {
        int64_t n = static_cast<int64_t>(stars.size());
        int64_t sum = 0;

        for (int64_t i = 0; i < n - 1; ++i) {
            for (int64_t j = i + 1; j < n; ++j) {
                sum += expanded_distance(
                    stars[i], stars[j], blank_rows, blank_cols, scale
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
        sum_of_distances(stars, blank_rows, blank_cols, 2)
    );
    std::println("  part 2: {}",
        sum_of_distances(stars, blank_rows, blank_cols, 1000000)
    );
}