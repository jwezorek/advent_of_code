#include "days.h"
#include "../util.h"
#include <print>
#include <ranges>
#include <numeric>
#include <algorithm>
#include <sstream>

namespace r = std::ranges;
namespace rv = std::ranges::views;

namespace {

    struct loc {
        int col;
        int row;
    };

    std::vector<loc> adjacent_locs(int col, int row, int len) {
        return {};
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
        int x1 = col - 1;
        int y1 = row - 1;
        int x2 = col + len;
        int y2 = row + 1;
        for (auto x = x1; x <= x2; ++x) {
            if (is_symbol(get_grid_item(grid, x, y1))) {
                return true;
            }
            if (is_symbol(get_grid_item(grid, x, y2))) {
                return true;
            }
        }
        if (is_symbol(get_grid_item(grid, x1, row))) {
            return true;
        }
        if (is_symbol(get_grid_item(grid, x2, row))) {
            return true;
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

    int do_part_1(const std::vector<std::string>& grid) {
        int cols = static_cast<int>(grid.front().size());
        int rows = static_cast<int>(grid.size());
        
        int sum = 0;
        for (int row = 0; row < rows; ++row) {
            for (int col = 0; col < cols; ++col) {
                if (std::isdigit(grid[row][col])) {
                    auto num_str = valid_number_str(grid, col, row);
                    if (!num_str.empty()) {
                        sum += std::stoi(num_str);
                        col += static_cast<int>(num_str.size()) - 1;
                    }
                }
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
}