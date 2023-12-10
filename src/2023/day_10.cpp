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

    loc operator*(int k, const loc& p) {
        return { k * p.col, k * p.row };
    }

    loc operator+(const loc& lhs, const loc& rhs) {
        return {
            lhs.col + rhs.col,
            lhs.row + rhs.row
        };
    }

    loc operator-(const loc& lhs, const loc& rhs) {
        return {
            lhs.col - rhs.col,
            lhs.row - rhs.row
        };
    }

    struct loc_equal {
        bool operator()(const loc& lhs, const loc& rhs) const {
            return lhs.col == rhs.col && lhs.row == rhs.row;
        }
    };

    bool operator==(const loc& lhs, const loc& rhs) {
        return loc_equal()(lhs, rhs);
    }

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

    std::array<loc, 2> neighbor_offsets(char ch) {
        if (ch == '.' || ch == 'S') {
            return { loc{-1,-1}, loc{-1,-1} };
        }
        static const std::unordered_map<char, std::array<loc, 2>> char_to_offsets = {
            {'J', { loc{-1,0}, loc{0,-1} }},
            {'|', { loc{0,-1}, loc{0,1}}},
            {'-', { loc{-1,0}, loc{1,0}}},
            {'L', { loc{0,-1}, loc{1,0}}},
            {'7', { loc{-1,0}, loc{0,1}}},
            {'F', { loc{0,1}, loc{1,0}, }}
        };
        return char_to_offsets.at(ch);
    }

    std::array<loc, 2> neighbors(const std::vector<std::string>& grid, const loc& loc) {
        auto offsets = neighbor_offsets(grid[loc.row][loc.col]);
        return {
            loc + offsets.front(),
            loc + offsets.back()
        };
    }

    char get_start_tile(const std::vector<std::string>& grid, const loc& start) {
        int rows = static_cast<int>(grid.size());
        int cols = static_cast<int>(grid.front().size());
        static const std::array<loc, 4> neighbor_offsets = { loc{-1,0}, loc{0,-1}, loc{0,1}, loc{1,0} };
        int i = 0;
        std::array<loc, 2> neighbors;
        for (const auto& offset : neighbor_offsets) {
            auto adjacent = start + offset;
            if (adjacent.row < 0 || adjacent.col < 0 ||
                adjacent.row >= rows || adjacent.col >= cols) {
                continue;
            }
            for (auto adj_adj : ::neighbors(grid, adjacent)) {
                if (adj_adj == start) {
                    neighbors[i++] = adjacent - start;
                }
            }
        }
        static const loc_map<char> discr_to_tile{
            {{-2, -1}, 'J'}, {{0,-1}, '|'}, {{-1,0}, '-'},
            {{1,-1}, 'L'}, {{-2,1}, '7'}, {{1,2}, 'F'}
        };
        return discr_to_tile.at(2 * neighbors.front() + neighbors.back());
    }

    std::tuple< loc, std::vector<std::string>> parse_input(const std::vector<std::string>& input) {
        auto grid = input;
        loc start;
        for (const auto& [y, row] : rv::enumerate(input)) {
            for (const auto& [x, col] : rv::enumerate(row)) {
                if (col == 'S') {
                    start = { static_cast<int>(x), static_cast<int>(y) };
                }
            }
        }
        grid[start.row][start.col] = get_start_tile(grid, start);
        return { start, std::move(grid) };
    }

    std::optional<loc> move_cursor(const loc_set& visited, const std::vector<std::string>& grid, 
            const loc& loc) {
        for (const auto& neighbor : neighbors( grid, loc)) {
            if (!visited.contains(neighbor)) {
                return neighbor;
            }
        }
        return {};
    }

    int do_part_1(const std::vector<std::string>& grid, const loc& start) {
        loc_set visited;
        const auto& start_neighbors = neighbors(grid, start);
        loc cursor_1 = start_neighbors.front();
        loc cursor_2 = start_neighbors.back();
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

    loc_set get_loop(const std::vector<std::string>& grid, const loc& start) {
        loc_set visited;
        loc cursor = start;

        do {
            visited.insert(cursor);
            auto next = move_cursor(visited, grid, cursor);
            cursor = (next) ? *next : start;
        } while (cursor != start);

        return visited;
    }

    std::vector<std::string> paint_loop(const loc_set& loop, 
            const std::vector<std::string>& grid) {
        int rows = static_cast<int>(grid.size());
        int cols = static_cast<int>(grid.front().size());
        std::vector<std::string> output(rows, std::string(cols, '.'));
        for (const auto& p : loop) {
            output[p.row][p.col] = grid[p.row][p.col];
        }
        return output;
    }

    int count_interior_of_row(const std::string& row) {
        int wall_count = 0;
        int interior_count = 0;
        for (char tile : row) {
            if (tile == '.' && wall_count % 2 == 1) {
                ++interior_count;
            }
            if (tile == '|' || tile == 'F' || tile == '7') {
                ++wall_count;
            }
        }
        return interior_count;
    }

    int count_interior(const std::vector<std::string>& grid, const loc& start) {
        auto loop = paint_loop(get_loop(grid, start), grid);
        return r::fold_left(
            loop | rv::transform(count_interior_of_row),
            0,
            std::plus<>()
        );
    }
}

/*------------------------------------------------------------------------------------------------*/

void aoc::y2023::day_10(const std::string& title) {

    const auto input = aoc::file_to_string_vector(aoc::input_path(2023, 10));
    const auto [start, grid] = parse_input(input);

    std::println("--- Day 10: {0} ---\n", title);
    std::println("  part 1: {}", do_part_1(grid, start));
    std::println("  part 2: {}", count_interior(grid, start));
}