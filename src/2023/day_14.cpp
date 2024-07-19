#include "y2023.h"
#include "../util/util.h"
#include <print>
#include <ranges>
#include <numeric>
#include <algorithm>
#include <Eigen/Dense>
#include <boost/functional/hash.hpp>
#include <unordered_set>

namespace r = std::ranges;
namespace rv = std::ranges::views;

namespace {
    using matrix = Eigen::Matrix<double, 3, 3>;
    using grid = std::vector<std::string>;
    using vec = Eigen::Matrix<double, 3, 1>;

    void print_rocks(const grid& rocks) {
        for (const auto& row : rocks) {
            std::println("{}", row);
        }
    }

    bool shift_row_north(std::string& row, std::string& row_above) {
        int n = static_cast<int>(row.size());
        bool something_changed = false;
        for (int i = 0; i < n; ++i) {
            if (row_above[i] == '.' && row[i] == 'O') {
                something_changed = true;
                row_above[i] = 'O';
                row[i] = '.';
            }
        }
        return something_changed;
    }

    bool shift_rows_north_once(grid& rocks) {
        int rows = static_cast<int>(rocks.size());
        bool something_changed = false;
        for (int row = 1; row < rows; ++row) {
            something_changed = (shift_row_north(rocks[row], rocks[row - 1]) || something_changed);
        }
        return something_changed;
    }

    void shift_rows_north(grid& rocks) {
        bool something_changed = false;
        do {
            something_changed = shift_rows_north_once(rocks);
        } while (something_changed);
    }

    int score_rocks(const grid& rocks) {
        int rows = static_cast<int>(rocks.size());
        return r::fold_left(
            rv::enumerate(rocks) | rv::transform(
                [rows](auto&& i_row)->int {
                    auto [row_index, row] = i_row;
                    return r::fold_left(
                        row | rv::transform(
                            [rows, row_index](char ch)->int {
                                return ch == 'O' ? rows - row_index : 0;
                            }
                        ),
                        0,
                        std::plus<>()
                    );
                }
            ), 0, std::plus<>()
        );
    }

    matrix identity_mat() {
        matrix ident;
        ident <<
            1, 0, 0,
            0, 1, 0,
            0, 0, 1;
        return ident;
    }

    matrix rotation_matrix(double cos, double sin) {
        matrix rotation;
        rotation <<
            cos, -sin, 0,
            sin,  cos, 0,
              0,    0, 1;
        return rotation;
    }

    matrix translation_matrix(double x, double y) {
        matrix translation;
        translation <<
            1, 0, x,
            0, 1, y,
            0, 0, 1;
        return translation;
    }

    struct loc {
        int col;
        int row;
    };

    loc transform(const loc& pt, const matrix& mat) {

        vec v;
        v << pt.col + 0.5, pt.row + 0.5, 1.0f;
        v = mat * v;

        return { 
            static_cast<int>(v[0]), 
            static_cast<int>(v[1])
        };
    }

    matrix rotate_about_center_mat(int dim, int cos, int sin) {
        auto trans = translation_matrix(-dim / 2.0, -dim / 2.0);
        auto trans_back = translation_matrix(dim / 2.0, dim / 2.0);
        auto rot = rotation_matrix(cos, sin);
        return trans_back * rot * trans;
    }

    grid make_grid(int dim) {
        return grid(dim, std::string(dim, '.'));
    }

    grid rotate(const grid& g, const matrix& mat) {
        int dim = static_cast<int>(g.size());
        auto rotated = make_grid(dim);
        for (int row = 0; row < dim; ++row) {
            for (int col = 0; col < dim; ++col) {
                auto rotated_loc = transform({col,row}, mat);
                rotated[rotated_loc.row][rotated_loc.col] = g[row][col];
            }
        }
        return rotated;
    }

    enum class direction {
        north = 0,
        west,
        south,
        east
    };

    matrix to_north_mat(direction from, int dim) {
        const std::array<matrix, 4> mats = {
            identity_mat(),
            rotate_about_center_mat(dim, 0, 1),
            rotate_about_center_mat(dim, -1, 0),
            rotate_about_center_mat(dim, 0, -1),
        };
        return mats[static_cast<int>(from)];
    }

    matrix from_north_mat(direction to, int dim) {
        const std::array<matrix, 4> mats = {
            identity_mat(),
            rotate_about_center_mat(dim, 0, -1),
            rotate_about_center_mat(dim, -1, 0),
            rotate_about_center_mat(dim, 0, 1)
        };
        return mats[static_cast<int>(to)];
    }

    grid tilt(const grid& rocks, direction dir) {
        int dim = static_cast<int>(rocks.size());
        auto rotated = rotate(rocks, to_north_mat(dir, dim));
        shift_rows_north(rotated);
        return rotate(rotated, from_north_mat(dir, dim));
    }

    grid do_one_cycle(const grid& inp) {
        auto output = inp;
        for (auto dir = 0; dir < 4; ++dir) {
            output = tilt(output, static_cast<direction>(dir));
        }
        return output;
    }

    struct grid_equate {
        bool operator()(const grid& lhs, const grid& rhs) const {
            for (const auto [left, right] : rv::zip(lhs, rhs)) {
                if (left != right) {
                    return false;
                }
            }
            return true;
        }
    };

    struct grid_hash {
        size_t operator()(const grid& g) const {
            size_t seed = 0;
            for (const auto& row : g) {
                boost::hash_combine(seed, row);
            }
            return seed;
        }
    };

    using grid_set = std::unordered_set<grid, grid_hash, grid_equate>;

    std::tuple<int,grid> find_first_repeat(const grid& g) {
        auto rocks = g;
        grid_set seen;
        int count = 0;

        do {
            seen.insert(rocks);
            rocks = do_one_cycle(rocks);
            ++count;
        } while (!seen.contains(rocks));

        return { count, std::move(rocks) };
    }

    std::tuple<int, int, grid> find_preamble_and_cycle(const grid& g) {
        auto [preamble, new_rocks] = find_first_repeat(g);
        auto [cycle, new_rocks2] = find_first_repeat(new_rocks);
        if (!grid_equate{}(new_rocks2, new_rocks)) {
            throw std::runtime_error("something is wrong");
        }
        return { preamble, cycle, std::move(new_rocks2) };
    }

    int do_part_2(const grid& inp, int n) {
        auto [preamble, cycle, cycle_start] = find_preamble_and_cycle(inp);
        auto steps_past_cycle = (n - preamble) % cycle;
        
        grid rocks = cycle_start;
        for (int i = 0; i < steps_past_cycle; ++i) {
            rocks = do_one_cycle(rocks);
        }
        return score_rocks(rocks);
    }

}

/*------------------------------------------------------------------------------------------------*/

void aoc::y2023::day_14(const std::string& title) {

    auto input = aoc::file_to_string_vector(aoc::input_path(2023, 14));
    std::println("--- Day 14: {0} ---\n", title);

    auto rocks = input;
    shift_rows_north(rocks);
    std::println("  part 1: {}", score_rocks(rocks));
    std::println("  part 2: {}", do_part_2(input, 1000000000));
    

}