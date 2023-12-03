#include "days.h"
#include "../util.h"
#include <print>
#include <ranges>
#include <numeric>
#include <algorithm>
#include <sstream>
#include <unordered_map>
#include <boost/functional/hash.hpp>

namespace r = std::ranges;
namespace rv = std::ranges::views;

namespace {

    struct loc {
        int col;
        int row;
    };

    struct loc_hasher {
        size_t operator()(const loc& loc) const {
            size_t seed = 0;
            boost::hash_combine(seed, loc.col);
            boost::hash_combine(seed, loc.row);
            return seed;
        }
    };

    struct loc_equal {
        bool operator()(const loc& loc1, const loc& loc2) const {
            return loc1.col == loc2.col && loc1.row == loc2.row;
        }
    };

    template<typename T>
    using loc_tbl = std::unordered_map<loc, T, loc_hasher, loc_equal>;

    std::vector<loc> adjacent_locs(int col, int row, int len) {
        std::vector<loc> adj_locs;

        int x1 = col - 1;
        int y1 = row - 1;
        int x2 = col + len;
        int y2 = row + 1;

        for (auto x = x1; x <= x2; ++x) {
            adj_locs.push_back({ x, y1 });
            adj_locs.push_back({ x, y2 });
        }

        adj_locs.push_back({ x1, row });
        adj_locs.push_back({ x2, row });

        return adj_locs;
    }

    char get_grid_item(const std::vector<std::string>& grid, int col, int row) {
        int cols = static_cast<int>(grid.front().size());
        int rows = static_cast<int>(grid.size());
        if (col < 0 || col >= cols || row < 0 || row >= rows) {
            return '.';
        }
        return grid[row][col];
    }

    bool is_symbol(char ch) {
        return ch != '.' && !std::isdigit(ch);
    }

    bool adjacent_to_symbol(const std::vector<std::string>& grid, int col, int row, int len) {
        for (const auto& loc : adjacent_locs(col, row, len)) {
            if (is_symbol(get_grid_item(grid, loc.col, loc.row))) {
                return true;
            }
        }
        return false;
    }

    std::string valid_number_str(const std::vector<std::string>& grid, int col, int row) {
        std::stringstream ss;
        int i = col;
        int cols = static_cast<int>(grid.front().size());
        while (i < cols && std::isdigit(grid[row][i])) {
            ss << grid[row][i++];
        }
        if (adjacent_to_symbol(grid, col, row, i - col)) {
            return ss.str();
        } else {
            return {};
        }
    }

    template<typename F> requires std::invocable<F, int, int>
    void visit_numbers(const std::vector<std::string>& grid, F visit) {
        int cols = static_cast<int>(grid.front().size());
        int rows = static_cast<int>(grid.size());
        for (int row = 0; row < rows; ++row) {
            for (int col = 0; col < cols; ++col) {
                if (std::isdigit(grid[row][col])) {
                    auto skip_cols = visit(col, row);
                    col += skip_cols;
                }
            }
        }
    }

    int do_part_1(const std::vector<std::string>& grid) {
        int sum = 0;
        visit_numbers(grid,
            [&](int col, int row)->int {
                auto num_str = valid_number_str(grid, col, row);
                if (!num_str.empty()) {
                    sum += std::stoi(num_str);
                    return static_cast<int>(num_str.size()) - 1;
                }
                return 0;
            }
        );
        return sum;
    }

    loc_tbl<std::vector<int>> build_gear_table(const std::vector<std::string>& grid) {

        loc_tbl<std::vector<int>> tbl;
        visit_numbers(grid,
            [&](int col, int row)->int {
                auto num_str = valid_number_str(grid, col, row);
                if (!num_str.empty()) {
                    int len = static_cast<int>(num_str.size());
                    for (const auto& adj : adjacent_locs(col, row, len)) {
                        if (get_grid_item(grid, adj.col, adj.row) == '*') {
                            tbl[adj].push_back(std::stoi(num_str));
                        }
                    }
                    return static_cast<int>(num_str.size()) - 1;
                }
                return 0;
            }
        );
        return tbl;
    }

    int do_part_2(const std::vector<std::string>& grid) {
        auto tbl = build_gear_table(grid);
        int sum = 0;
        for (const auto& [key, val] : tbl) {
            if (val.size() == 2) {
                sum += val.front() * val.back();
            }
        }
        return sum;
    }
}

/*------------------------------------------------------------------------------------------------*/

void aoc::y2023::day_03(const std::string& title) {

    auto input = aoc::file_to_string_vector(aoc::input_path(2023, 3));

    std::println("--- Day 3: {0} ---\n", title);
    std::println("  part 1: {}", do_part_1(input));
    std::println("  part 2: {}", do_part_2(input));
}