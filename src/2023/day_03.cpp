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

    struct loc_hash {
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
    using loc_tbl = std::unordered_map<loc, T, loc_hash, loc_equal>;

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

    std::tuple<int, int> grid_dimensions(const std::vector<std::string>& grid) {
        return {
            static_cast<int>(grid.front().size()),
            static_cast<int>(grid.size())
        };
    }

    char get_grid_item(const std::vector<std::string>& grid, int col, int row) {
        auto [cols, rows] = grid_dimensions(grid);
        if (col < 0 || col >= cols || row < 0 || row >= rows) {
            return '.';
        }
        return grid[row][col];
    }

    bool is_symbol(char ch) {
        return ch != '.' && !std::isdigit(ch);
    }

    bool adjacent_to_symbol(const std::vector<std::string>& grid, int col, int row, int len) {
        auto adj_locs = adjacent_locs(col, row, len);
        return r::find_if(adj_locs, 
            [&](auto&& loc) { 
                return is_symbol(get_grid_item(grid, loc.col, loc.row)); 
            }
        ) != adj_locs.end();
    }

    template<typename F> requires std::invocable<F, std::string, int, int>
    void visit_numbers(const std::vector<std::string>& grid, F visit) {
        auto [cols, rows] = grid_dimensions(grid);
        for (int row = 0; row < rows; ++row) {
            for (int col = 0; col < cols; ++col) {
                if (std::isdigit(grid[row][col])) {
                    std::stringstream ss;
                    int i = col;
                    while (std::isdigit(grid[row][i])) {
                        ss << get_grid_item(grid, i++, row);
                    }
                    visit(ss.str(), col, row);
                    col += static_cast<int>(ss.str().size() - 1);
                }
            }
        }
    }

    int do_part_1(const std::vector<std::string>& grid) {
        int sum = 0;
        visit_numbers(grid,
            [&](const std::string& str, int col, int row) {
                if (adjacent_to_symbol(grid, col, row, static_cast<int>(str.size()))) {
                    sum += std::stoi(str);
                }
            }
        );
        return sum;
    }

    loc_tbl<std::vector<int>> build_gear_table(const std::vector<std::string>& grid) {
        loc_tbl<std::vector<int>> tbl;
        visit_numbers(grid,
            [&](const std::string& num_str, int col, int row) {
                int len = static_cast<int>(num_str.size());
                for (const auto& adj : adjacent_locs(col, row, len)) {
                    if (get_grid_item(grid, adj.col, adj.row) == '*') {
                        tbl[adj].push_back(std::stoi(num_str));
                    }
                }
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