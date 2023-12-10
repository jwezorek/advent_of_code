#include "days.h"
#include "../util.h"
#include <print>
#include <ranges>
#include <numeric>
#include <algorithm>
#include <unordered_set>
#include <unordered_map>
#include <array>
#include <boost/functional/hash.hpp>
#include <tuple>
#include <optional>

namespace r = std::ranges;
namespace rv = std::ranges::views;

namespace {
    struct loc {
        int col;
        int row;
    };

    loc operator+(const loc& lhs, const loc& rhs) {
        return {
            lhs.col + rhs.col,
            lhs.row + rhs.row
        };
    }

    struct loc_equal {
        bool operator()(const loc& lhs, const loc& rhs) const {
            return lhs.col == rhs.col && lhs.row == rhs.row;
        }
    };

    struct loc_hash {
        size_t operator()(const loc& loc) const {
            size_t seed = 0;
            boost::hash_combine(seed, loc.col);
            boost::hash_combine(seed, loc.row);
            return seed;
        }
    };

    using loc_set = std::unordered_set<loc, loc_hash, loc_equal>;

    template<typename T>
    using loc_map = std::unordered_map<loc, T, loc_hash, loc_equal>;

    bool operator==(const loc& lhs, const loc& rhs) {
        return loc_equal()(lhs, rhs);
    }

    using cell = std::array<loc, 2>;

    cell cell_from_char(const loc& cell_loc, char ch) {
        if (ch == '.' || ch == 'S') {
            return { loc{-1,-1}, loc{-1,-1} };
        }
        static const std::unordered_map<char, cell> char_to_offsets = {
            {'J', { loc{0,-1}, loc{-1,0}}},
            {'|', { loc{0,-1}, loc{0,1}}},
            {'-', { loc{-1,0}, loc{1,0}}},
            {'L', { loc{0,-1}, loc{1,0}}},
            {'7', { loc{-1,0}, loc{0,1}}},
            {'F', { loc{1,0}, loc{0,1}}}
        };
        const auto& offsets = char_to_offsets.at(ch);
        return {
            cell_loc + offsets[0],
            cell_loc + offsets[1]
        };
    }

    cell make_empty_cell() {
        return { loc{-1,-1}, loc{-1,-1} };
    }

    cell fill_in_start_cell(const std::vector<std::vector<cell>>& grid, const loc& start) {
        int i = 0;
        int rows = static_cast<int>(grid.size());
        int cols = static_cast<int>(grid.front().size());
        static const std::array<loc, 4> neighbors = { loc{0,-1}, loc{1,0}, loc{0,1}, loc{-1,0} };
        cell output;
        for (const auto& offset : neighbors) {
            auto neighbor = start + offset;
            if (neighbor.row < 0 || neighbor.col < 0 ||
                    neighbor.row >= rows || neighbor.col >= cols) {
                continue;
            }
            for (auto neighbor_neighbor : grid[neighbor.row][neighbor.col]) {
                if (neighbor_neighbor == start) {
                    output[i++] = neighbor;
                }
            }
        }
        return output;
    }

    std::tuple<loc, std::vector<std::vector<cell>>> parse_input(const std::vector<std::string>& lines) {
        int rows = static_cast<int>(lines.size());
        int cols = static_cast<int>(lines.front().size());

        loc start;
        std::vector<std::vector<cell>> grid(rows, std::vector<cell>(cols, make_empty_cell()));
        for (int row = 0; row < rows; ++row) {
            for (int col = 0; col < cols; ++col) {
                grid[row][col] = cell_from_char({ col,row }, lines.at(row).at(col));
                if (lines.at(row).at(col) == 'S') {
                    start = { col,row };
                }
            }
        }
        grid[start.row][start.col] = fill_in_start_cell(grid, start);

        return { start, std::move(grid) };
    }
}

std::optional<loc> move_cursor(const loc_set& visited, const std::vector<std::vector<cell>>& grid, const loc& current) {
    for (const auto& next : grid[current.row][current.col]) {
        if (!visited.contains(next)) {
            return next;
        }
    }
    return {};
}

int do_part_1(const std::vector<std::vector<cell>>& grid, const loc& start) {
    loc_set visited;
    loc cursor_1 = grid[start.row][start.col].at(0);
    loc cursor_2 = grid[start.row][start.col].at(1);
    visited.insert(start);
    visited.insert(cursor_1);
    visited.insert(cursor_2);
    int dist = 1;
    while (cursor_1 != cursor_2) {
        cursor_1 = *move_cursor(visited, grid, cursor_1);
        cursor_2 = *move_cursor(visited, grid, cursor_2);
        ++dist;
        visited.insert(cursor_1);
        visited.insert(cursor_2);
    }
    return dist;
}

loc_set get_loop( const std::vector<std::vector<cell>>& grid, const loc& start) {
    loc_set visited;
    loc cursor = start;

    do {
        visited.insert(cursor);
        auto next = move_cursor(visited, grid, cursor);
        cursor = (next) ? *next : start;
    } while (cursor != start);

    return visited;
}

std::vector<std::string> paint_loop(const loc_set& loop, const std::vector<std::string>& txt_grid, char start_tile) {
    int rows = static_cast<int>(txt_grid.size());
    int cols = static_cast<int>(txt_grid.front().size());
    std::vector<std::string> output(rows, std::string(cols, '.'));
    for (const auto& p : loop) {
        auto tile = txt_grid[p.row][p.col];
        output[p.row][p.col] = (tile != 'S') ? tile : start_tile;
    }
    return output;
}

enum class wall_state {
    open_south_to_east,
    open_north_to_east,
    none
};

int count_interior_cells(const std::string& row) {
    wall_state state = wall_state::none;
    int wall_count = 0;
    int interior_count = 0;

    for (int i = 0; i < row.size(); ++i) {
        auto tile = row[i];
        if (tile == '.' && wall_count % 2 == 1) {
            interior_count++;
        } else if (tile == 'L') {
            state = wall_state::open_north_to_east;
        } else if( tile == 'F') {
            state = wall_state::open_south_to_east;
        } else if (tile == 'J') {
            if (state == wall_state::open_south_to_east) {
                wall_count++;
            }
            state = wall_state::none;
        } else if (tile == '7') {
            if (state == wall_state::open_north_to_east) {
                wall_count++;
            }
            state = wall_state::none;
        } else if (tile == '|') {
            wall_count++;
        }
    }
    return interior_count;
}

char get_start_tile(const std::vector<std::vector<cell>>& grid, loc start) {
    return '|';
}

/*------------------------------------------------------------------------------------------------*/

void aoc::y2023::day_10(const std::string& title) {

    const auto input = aoc::file_to_string_vector(aoc::input_path(2023, 10));
    const auto [start, grid] = parse_input(input);

    std::println("--- Day 10: {0} ---\n", title);
    std::println("  part 1: {}", do_part_1(grid, start));
    auto loop = get_loop(grid, start);
    auto loop_painting = paint_loop(loop, input, get_start_tile(grid, start));

    int sum = 0;
    for (auto row : loop_painting) {
        sum += count_interior_cells(row);
    }
    std::println("  part 2: {}",sum);
}