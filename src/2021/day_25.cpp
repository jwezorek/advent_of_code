#include "../util.h"
#include "y2021.h"
#include <filesystem>
#include <functional>
#include <print>
#include <ranges>
#include <unordered_map>
#include <tuple>

namespace r = std::ranges;
namespace rv = std::ranges::views;

/*------------------------------------------------------------------------------------------------*/

namespace {
    using grid = std::vector<std::string>;

    std::tuple<int, int> dimensions(const grid& g) {
        return {
            static_cast<int>(g.front().size()),
            static_cast<int>(g.size())
        };
    }

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

    int wrap(int v, int dim) {
        v = (v < 0) ? dim - 1 : v;
        v = (v >= dim) ? 0 : v;
        return v;
    }

    loc wrap(const loc& pos, int cols, int rows) {
        auto [col, row] = pos;
        return {
            wrap(col, cols),
            wrap(row, rows)
        };
    }

    grid make_grid(int cols, int rows) {
        return grid(rows, std::string(cols, '.'));
    }

    auto locs(int cols, int rows) {
        return rv::cartesian_product(rv::iota(0, cols), rv::iota(0, rows)) |
            rv::transform(
                [](auto&& tup)->loc {
                    const auto& [col, row] = tup;
                    return { col, row };
                }
            );
    }

    char at(const grid& g, const loc& loc) {
        return g.at(loc.row).at(loc.col);
    }

    char& at(grid& g, const loc& loc) {
        return g[loc.row][loc.col];
    }

    auto cucs_of_type(const grid& g, char cuc_type) {
        auto [cols, rows] = dimensions(g);
        return locs(cols, rows) | rv::filter(
            [cuc_type, &g](auto&& u){ return at(g,u) == cuc_type; }
        );
    }

    std::tuple<grid, bool> move_cucumbers(const grid& current, char moving_cuc, char stationary_cuc, loc delta) {
        auto [cols, rows] = dimensions(current);

        auto next = make_grid(cols, rows);
        for (auto loc : cucs_of_type(current, stationary_cuc)) {
            at(next, loc) = stationary_cuc;
        }

        bool at_least_one_moved = false;
        for (auto loc : cucs_of_type(current, moving_cuc)) {
            auto next_loc = wrap(loc + delta, cols, rows);
            if (at(current, next_loc) == '.') {
                at(next, next_loc) = moving_cuc;
                at_least_one_moved = true;
            } else {
                at(next, loc) = moving_cuc;
            }
        }
        return { std::move(next), at_least_one_moved };
    }

    std::tuple<grid, bool> do_one_update(const grid& current) {
        auto [step_right, moved_right] = move_cucumbers(current, '>', 'v', { 1, 0 });
        auto [step_down, moved_down] = move_cucumbers(step_right, 'v', '>', { 0, 1 });
        return { std::move(step_down), moved_right || moved_down };
    }

    int move_cucumbers(const grid& g) {
        int i = 0;
        bool moved = false;
        auto current = g;
        do {
            ++i;
            std::tie(current, moved) = do_one_update(current);
        } while (moved);
        return i;
    }
}

void aoc::y2021::day_25(const std::string& title) {
    auto input = aoc::file_to_string_vector(aoc::input_path(2021, 25));

    std::println("--- Day 25: {} ---", title);
    std::println("  part 1: {}", move_cucumbers(input));
    std::println("  part 2: {}", "<xmas freebie>");
}