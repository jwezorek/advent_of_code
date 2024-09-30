
#include "../util/util.h"
#include "../util/vec2.h"
#include "y2017.h"
#include <filesystem>
#include <functional>
#include <print>
#include <ranges>
#include <numbers>
#include <Eigen/Dense>

namespace r = std::ranges;
namespace rv = std::ranges::views;

/*------------------------------------------------------------------------------------------------*/

namespace {

    using matrix = Eigen::Matrix<double, 3, 3>;
    using vec = Eigen::Matrix<double, 3, 1>;
    using point = aoc::vec2<int>;

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
            sin, cos, 0,
            0, 0, 1;
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

    using grid = std::vector<std::string>;

    grid blank_grid(int sz) {
        return grid(sz, std::string(sz, '.'));
    }

    point apply(const matrix& mat, const point& pt) {
        vec v;
        v << static_cast<double>(pt.x) + 0.5, 
            static_cast<double>(pt.y) + 0.5, 
            1.0;
        v = mat * v;

        auto new_x = static_cast<int>(v[0]);
        auto new_y = static_cast<int>(v[1]);

        return { new_x, new_y };
    }

    grid rotate_grid(const grid& g, int quarter_turns) {
        auto sz = static_cast<int>(g.size());
        auto offset = static_cast<double>(sz) / 2.0;
        auto theta = quarter_turns * (std::numbers::pi / 2.0);
        matrix mat = translation_matrix(offset, offset) *
            rotation_matrix(std::cos(theta), std::sin(theta)) *
            translation_matrix(-offset, -offset);

        auto out = blank_grid(sz);
        for (int y = 0; y < sz; ++y) {
            for (int x = 0; x < sz; ++x) {
                if (g[y][x] == '#') {
                    auto new_pt = apply(mat, point{ x,y });
                    out[new_pt.y][new_pt.x] = '#';
                }
            }
        }
        
        return out;
    }

    void display_grid(const grid& g) {
        for (auto row : g) {
            std::println("{}", row);
        }
        std::println("");
    }
}

void aoc::y2017::day_21(const std::string& title) {

    auto inp = aoc::file_to_string_vector(
            aoc::input_path(2017, 21)
        ); 

    grid test = { ".#.","#..","###" };
    display_grid(test);
    std::println("");
    for (int i = 0; i < 4; ++i) {
        auto rotated = rotate_grid(test, i);
        display_grid(rotated);
    }

    std::println("--- Day 21: {} ---", title);
    std::println("  part 1: {}", 0);
    std::println("  part 2: {}", 0);
    
}
