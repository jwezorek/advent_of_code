#include "../util/util.h"
#include "y2021.h"
#include <filesystem>
#include <functional>
#include <print>
#include <ranges>
#include <unordered_map>
#include <iostream>

namespace r = std::ranges;
namespace rv = std::ranges::views;

/*------------------------------------------------------------------------------------------------*/

namespace {

    struct fold {
        bool is_x_axis;
        int value;
    };

    std::string to_string(const fold& f) {
        std::stringstream ss;
        ss << "fold along " << (f.is_x_axis ? 'x' : 'y') << "=" << f.value;
        return ss.str();
    }

    using folds_t = std::vector<fold>;
    using grid_t = std::vector<std::vector<int>>;

    std::tuple<std::vector<std::string>, std::vector<std::string>> split_string_array(const std::vector<std::string>& input) {
        auto separator = std::find_if(input.begin(), input.end(), [](const auto& str) {return str.empty(); });

        std::vector<std::string> top(std::distance(input.begin(), separator));
        std::copy(input.begin(), separator, top.begin());

        std::vector<std::string> bottom(std::next(separator), input.end());
        std::copy(std::next(separator), input.end(), bottom.begin());

        return { std::move(top) , std::move(bottom) };
    }

    std::vector<std::tuple<int, int>> parse_points(const std::vector<std::string>& lines) {
        std::vector<std::tuple<int, int>> points(lines.size());
        std::transform(lines.begin(), lines.end(), points.begin(),
            [](const std::string& str)->std::tuple<int, int> {
                auto x_y = aoc::split(str, ',');
                return {
                    std::stoi(x_y[0]),
                    std::stoi(x_y[1])
                };
            }
        );
        return points;
    }

    std::tuple<int, int> get_dimensions_cols_and_rows(const std::vector<std::tuple<int, int>>& points) {
        int cols = -1;
        int rows = -1;
        for (const auto& [x, y] : points) {
            cols = (x > cols) ? x : cols;
            rows = (y > rows) ? y : rows;
        }
        return { cols + 1, rows + 1 };
    }

    grid_t make_grid(int cols, int rows) {
        return grid_t(rows, std::vector<int>(cols, 0));
    }

    int get_num_rows(const grid_t& grid) {
        return grid.size();
    }

    int get_num_cols(const grid_t& grid) {
        return grid[0].size();
    }

    std::tuple<int, int> get_cols_and_rows(const grid_t& grid) {
        return { get_num_cols(grid), get_num_rows(grid) };
    }

    grid_t points_to_grid(const std::vector<std::string>& lines) {
        auto points = parse_points(lines);
        auto [cols, rows] = get_dimensions_cols_and_rows(points);

        grid_t ary = make_grid(cols, rows);
        for (const auto& [x, y] : points) {
            ary[y][x] = 1;
        }

        return ary;
    }

    fold string_to_fold(const std::string& str) {
        auto words = aoc::split(str, ' ');
        auto fold_parts = aoc::split(words[2], '=');

        fold f;
        f.is_x_axis = (fold_parts[0] == "x");
        f.value = std::stoi(fold_parts[1]);

        return f;
    }

    folds_t parse_folds(const std::vector<std::string>& lines) {
        folds_t folds(lines.size());
        std::transform(lines.begin(), lines.end(), folds.begin(), string_to_fold);
        return folds;
    }

    std::tuple<grid_t, folds_t> parse_folding_instructions(const std::vector<std::string>& input) {
        auto [grid_lines, fold_lines] = split_string_array(input);
        return {
            points_to_grid(grid_lines),
            parse_folds(fold_lines)
        };
    }

    grid_t clip_last_column(const grid_t& inp) {
        auto [cols, rows] = get_cols_and_rows(inp);
        auto out = make_grid(cols - 1, rows);
        for (int y = 0; y < rows; ++y) {
            for (int x = 0; x < cols - 1; ++x) {
                out[y][x] = inp[y][x];
            }
        }
        return out;
    }

    grid_t clip_last_row(const grid_t& inp) {
        auto out = inp;
        out.pop_back();
        return out;
    }

    void fold_column(const grid_t& src, int src_col, grid_t& dst, int dst_col) {
        int rows = get_num_rows(src);
        for (int y = 0; y < rows; ++y) {
            dst[y][dst_col] = std::max(src[y][src_col], dst[y][dst_col]);
        }
    }

    void fold_vert(const grid_t& src, int src_x, int src_dir, grid_t& dst, int dst_x, int dst_dir, int count) {
        for (int i = 0; i < count; ++i) {
            fold_column(src, src_x + i * src_dir, dst, dst_x + i * dst_dir);
        }
    }

    grid_t make_vert_fold(const grid_t& g, int x) {
        auto [cols, rows] = get_cols_and_rows(g);
        int left_cols = x + 1;
        int right_cols = cols - x;

        int num_folded_cols = std::max(right_cols, left_cols);
        auto folded_grid = make_grid(num_folded_cols, rows);

        fold_vert(g, x, -1, folded_grid, num_folded_cols - 1, -1, x + 1);
        fold_vert(g, x, 1, folded_grid, num_folded_cols - 1, -1, cols - x);

        return clip_last_column(folded_grid); // clip_last_... necessary because i misread the instructions
    }

    void fold_row(const grid_t& src, int src_row, grid_t& dst, int dst_row) {
        int cols = get_num_cols(src);
        for (int x = 0; x < cols; ++x) {
            dst[dst_row][x] = std::max(src[src_row][x], dst[dst_row][x]);
        }
    }

    void fold_horz(const grid_t& src, int src_y, int src_dir, grid_t& dst, int dst_y, int dst_dir, int count) {
        for (int i = 0; i < count; ++i) {
            fold_row(src, src_y + i * src_dir, dst, dst_y + i * dst_dir);
        }
    }

    grid_t make_horz_fold(const grid_t& g, int y) {
        auto [cols, rows] = get_cols_and_rows(g);
        int top_rows = y + 1;
        int bottom_rows = rows - y;

        int num_folded_rows = std::max(top_rows, bottom_rows);
        auto folded_grid = make_grid(cols, num_folded_rows);

        fold_horz(g, y, -1, folded_grid, num_folded_rows - 1, -1, y + 1);
        fold_horz(g, y, 1, folded_grid, num_folded_rows - 1, -1, rows - y);

        return clip_last_row(folded_grid);
    }

    grid_t perform_fold(const grid_t& g, const fold& fold) {
        return (fold.is_x_axis) ?
            make_vert_fold(g, fold.value) :
            make_horz_fold(g, fold.value);
    }

    int count_dots(const grid_t& g) {
        int sum = 0;
        for (const auto& row : g) {
            for (int v : row) {
                sum += v;
            }
        }
        return sum;
    }

    void display_grid(const grid_t& ary) {
        for (const auto& row : ary) {
            for (int value : row) {
                std::cout << ((value == 0) ? ' ' : '#');
            }
            std::cout << "\n";
        }
    }
}

void aoc::y2021::day_13(const std::string& title) {
    auto input = aoc::file_to_string_vector(aoc::input_path(2021, 13));
    auto [grid, folds] = parse_folding_instructions(input);

    std::println("--- Day 13: {} ---", title);
    std::println("  part 1: {}", count_dots(perform_fold(grid, folds[0])));
    std::println("  part 2: \n");
    for (const auto& f : folds) {
        grid = perform_fold(grid, f);
    }
    display_grid(grid);
}