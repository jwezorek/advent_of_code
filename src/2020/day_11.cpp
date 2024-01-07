#include "../util.h"
#include "y2020.h"
#include <filesystem>
#include <functional>
#include <print>
#include <ranges>
#include <unordered_map>

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

    bool operator==(const loc& lhs, const loc& rhs) {
        return lhs.col == rhs.col && lhs.row == rhs.row;
    }

    auto direction_deltas() {
        static const std::array<loc, 8> deltas = {
            loc{-1,-1}, loc{0,-1}, loc{1,-1},
            loc{-1, 0},            loc{1, 0},
            loc{-1, 1}, loc{0, 1}, loc{1, 1},
        };
        return rv::all(deltas);
    }

    bool in_bounds(const loc& p, int cols, int rows) {
        return p.col >= 0 && p.col < cols &&
            p.row >= 0 && p.row < rows;
    }

    auto neighbors(const loc& pos, int cols, int rows) {
        return direction_deltas() | rv::transform(
            [pos](auto&& delta)->loc {
                return pos + delta;
            }
        ) | rv::filter(
            [cols,rows](auto&& p)->bool {
                return in_bounds(p, cols, rows);
            }
        );
    }

    auto all_locs(int cols, int rows) {
        return rv::cartesian_product(
                rv::iota(0, cols), rv::iota(0, rows)
            ) | rv::transform(
                [](auto&& tup)->loc {
                    auto [col, row] = tup;
                    return loc{ col,row };
                }
            );
    }

    grid make_grid(int cols, int rows) {
        return grid(rows, std::string(cols, '.'));
    }

    using occupied_fn = std::function<int(const grid&, const loc&)>;

    int immediate_occupied_neigbors(const grid& g, const loc& where) {
        auto [cols, rows] = dimensions(g);
        return r::fold_left(
            neighbors(where, cols, rows) | rv::transform(
                [&g](auto&& pt)->int {
                    return g[pt.row][pt.col] == '#' ? 1 : 0;
                }
            ),
            0,
            std::plus<>()
        );
    }

    char line_of_sight(const grid& g, const loc& pos, const loc& delta) {
        auto [cols, rows] = dimensions(g);
        loc los_pt = pos + delta;
        while (in_bounds(los_pt, cols, rows) && g[los_pt.row][los_pt.col] == '.') {
            los_pt = los_pt + delta;
        }
        return in_bounds(los_pt, cols, rows) ? g[los_pt.row][los_pt.col] : '.';
    }

    int occupied_neigbors_line_of_sight(const grid& g, const loc& where) {
        return r::fold_left(
            direction_deltas() | rv::transform(
                [&](auto&& delta)->int {
                    return line_of_sight(g, where, delta) == '#';
                }
            ),
            0,
            std::plus<>()
        );
    }

    std::tuple<grid, bool> do_one_iteration(const grid& g, occupied_fn neighbor_fn, int occupied_threshold) {
        auto [cols, rows] = dimensions(g);
        auto next = make_grid(cols, rows);
        bool change_occurred = false;
        for (int row = 0; row < rows; ++row) {
            for (int col = 0; col < cols; ++col) {
                auto tile = g[row][col];
                if (tile == '.') {
                    continue;
                }
                int occupied_neighbors = neighbor_fn(g, { col,row });
                char new_tile = tile;
                if (tile == 'L' && occupied_neighbors == 0) {
                    new_tile = '#';
                    change_occurred = true;
                } else if (tile == '#' && occupied_neighbors >= occupied_threshold){
                    new_tile = 'L';
                    change_occurred = true;
                }
                next[row][col] = new_tile;
            }
        }
        return { std::move(next), change_occurred };
    }

    int count_occupied(const grid& g) {
        auto [cols, rows] = dimensions(g);
        return r::fold_left(
            all_locs(cols, rows) | rv::transform(
                [&g](auto&& pos)->int {
                    return g[pos.row][pos.col] == '#' ? 1 : 0;
                }
            ),
            0,
            std::plus<>()
        );
    }

    void display(const grid& g) {
        for (const auto& row : g) {
            std::println("{}", row);
        }
        std::println("");
    }

    int count_occupied_at_equilibrium(const grid& g, 
            occupied_fn neighbor_fn, int occupied_threshold) {
        auto grid = g;
        bool change_occurred = false;
        do {
            std::tie(grid, change_occurred) = do_one_iteration(
                grid, 
                neighbor_fn,
                occupied_threshold
            );
        } while (change_occurred);

        return count_occupied(grid);
    }
}

void aoc::y2020::day_11(const std::string& title) {
    auto input = aoc::file_to_string_vector(aoc::input_path(2020, 11));

    std::println("--- Day 11: {} ---", title);
    
    std::println("  part 1: {}", 
        count_occupied_at_equilibrium(input, immediate_occupied_neigbors, 4) 
    );
    std::println("  part 2: {}",
        count_occupied_at_equilibrium(input, occupied_neigbors_line_of_sight, 5)
    );
}