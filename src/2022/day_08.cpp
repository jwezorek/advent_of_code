#include "../util.h"
#include "../concat.h"
#include "y2022.h"
#include <filesystem>
#include <functional>
#include <boost/functional/hash.hpp>
#include <print>
#include <ranges>
#include <unordered_set>
#include <numeric>

// this solution used ranges-v3 concat and partial_sum which
// are not standard C++ yet...

/*------------------------------------------------------------------------------------------------*/

/*
namespace r = std::ranges;
namespace rv = std::ranges::views;

namespace {
    using grid = std::vector<std::vector<int>>;

    struct grid_loc {
        int value;
        int col;
        int row;

        grid_loc(int v = -1, int c = -1, int r = -1) :
            value(v), col(c), row(r)
        {};
    };

    struct hash_grid_loc {
        size_t operator()(const grid_loc& grid_loc) const {
            size_t seed = 0;
            boost::hash_combine(seed, grid_loc.col);
            boost::hash_combine(seed, grid_loc.row);
            return seed;
        }
    };

    struct grid_loc_equal {
        bool operator()(const grid_loc& loc1, const grid_loc& loc2) const {
            return loc1.col == loc2.col && loc1.row == loc2.row;
        }
    };

    using grid_loc_set = std::unordered_set<grid_loc, hash_grid_loc, grid_loc_equal>;

    bool is_in_grid(const grid& ary, int col, int row) {
        int columns = static_cast<int>(ary[0].size());
        int rows = static_cast<int>(ary.size());
        return row >= 0 && row < rows && col >= 0 && col < columns;
    }

    auto line(const grid& ary, int c, int r, int col_offset, int row_offset) {
        return rv::iota(0) |
            rv::transform(
                [c, r, col_offset, row_offset](int n)->std::tuple<int, int> {
                    return {
                        c + n * col_offset,
                        r + n * row_offset
                    };
                }
            ) | rv::take_while(
                [&ary](auto&& c_r) {
                    auto [col, row] = c_r;
                    return is_in_grid(ary, col, row);
                }
            ) | rv::transform(
                [&ary](auto&& c_r)->grid_loc {
                    auto [col, row] = c_r;
                    return {
                        ary[row][col],
                        col,
                        row
                    };
                }
                );
    }

    auto rows(const grid& ary) {
        int n = static_cast<int>(ary.size());
        return rv::iota(0, n) |
            rv::transform(
                [&ary](int row_index) {
                    return line(ary, 0, row_index, 1, 0);
                }
        );
    }

    auto columns(const grid& ary) {
        int n = static_cast<int>(ary[0].size());
        return rv::iota(0, n) |
            rv::transform(
                [&ary](int col_index) {
                    return line(ary, col_index, 0, 0, 1);
                }
        );
    }

    auto all_grid_locs(const grid& ary) {
        return rows(ary) | rv::join;
    }

    auto running_max(auto rng) {
        return rv::partial_sum(rng, [](int lhs, int rhs) { return std::max(lhs, rhs); });
    }

    auto visible(auto rng) {
        return rv::zip(
            aoc::concat(rv::single(-1), running_max(rng | rv::transform([](auto&& grid_loc) {return grid_loc.value; }))),
            rng
        ) | 
            rv::filter(
                [](auto&& tup) {
                    auto [max, grid_loc] = tup;
                    return grid_loc.value > max;
                }
            ) |
            rv::transform(
                [](auto&& tup) {
                    const auto& [max, grid_loc] = tup;
                    return grid_loc;
                }
           );
    }

    int num_visible(const grid& ary) {
        auto visible_set = aoc::concat(rows(ary), columns(ary)) |
            rv::transform([](auto rng) {
                return aoc::concat(
                    visible(rng),
                    visible(rv::reverse(rng))
                );
                }
            ) | rv::join | r::to<grid_loc_set>();
        return static_cast<int>(visible_set.size());
    }

    int viewing_distance(const grid& ary, int col, int row, int col_offset, int row_offset) {
        int max_height = ary[row][col];
        int count = 0;
        for (int v : line(ary, col, row, col_offset, row_offset) |
            rv::drop(1) | rv::transform([](const auto& grid_loc) {return grid_loc.value; })) {
            count++;
            if (v >= max_height) {
                return count;
            }
        }
        return count;
    }

    int scenic_score(const grid& ary, int col, int row) {
        const static std::array<std::tuple<int, int>, 4> directions = { {
            {1, 0},
            {0, 1},
            {-1,0},
            {0,-1}
        } };
        return r::fold_left(
            directions |
            rv::transform(
                [&ary, col, row](auto&& tup) {
                    auto [col_offset, row_offset] = tup;
                    return viewing_distance(ary, col, row, col_offset, row_offset);
                }
            ),
            1,
            std::multiplies<int>()
        );
    }

    int highest_scenic_score(const grid& ary) {
        return r::max(
            all_grid_locs(ary) |
            rv::transform(
                [&ary](auto grid_loc)->int {
                    return scenic_score(ary, grid_loc.col, grid_loc.row);
                }
            )
        );
    }
}
*/

void aoc::y2022::day_08(const std::string& title) {
    auto input = file_to_string_vector(input_path(2022, 8));

    std::println("--- Day 8: {} ---", title);
    std::println("  part 1: {}", 0);
    std::println("  part 2: {}", 0);
}