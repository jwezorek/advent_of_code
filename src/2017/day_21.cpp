
#include "../util/util.h"
#include "../util/vec2.h"
#include "y2017.h"
#include <filesystem>
#include <functional>
#include <print>
#include <ranges>
#include <numbers>
#include <unordered_map>
#include <iostream>
#include <Eigen/Dense>
#include <boost/functional/hash.hpp>

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

    matrix reflection_matrix() {
        matrix translation;
        translation <<
            -1, 0, 0,
            0, 1, 0,
            0, 0, 1;
        return translation;
    }

    using grid = std::vector<std::string>;
    struct hash_grid {
        size_t operator()(const grid& g) const {
            size_t seed = g.size();
            for (const auto& row : g) {
                boost::hash_combine(seed, std::hash<std::string>{}(row));
            }
            return seed;
        }
    };

    struct equate_grid {
        bool operator()(const grid& lhs, const grid& rhs) const {
            if (lhs.size() != rhs.size()) {
                return false;
            }
            for (const auto& [ls, rs] : rv::zip(lhs, rhs)) {
                if (ls != rs) {
                    return false;
                }
            }
            return true;
        }
    };

    using subgrid_tbl = std::unordered_map<grid, grid, hash_grid, equate_grid>;

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

    matrix grid_rot_matrix(int quarter_turns, int sz) {
        auto offset = static_cast<double>(sz) / 2.0;
        auto theta = quarter_turns * (std::numbers::pi / 2.0);
        return translation_matrix(offset, offset) *
            rotation_matrix(std::cos(theta), std::sin(theta)) *
            translation_matrix(-offset, -offset);
    }

    matrix grid_reflect_matrix(int sz) {
        auto offset = static_cast<double>(sz) / 2.0;
        return translation_matrix(offset, offset) *
            reflection_matrix() *
            translation_matrix(-offset, -offset);
    }

    grid apply(const grid& g,  const matrix& mat) {
        auto sz = static_cast<int>(g.size());
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

    std::vector<std::vector<grid>> to_subgrids(const grid& g) {
        if (g.size() == 2 || g.size() == 3) {
            return { { g } };
        }
        int subgrid_sz = (g.size() % 2) == 0 ? 2 : 3;
        int n = g.size() / subgrid_sz;
        std::vector<std::vector<grid>> subgrids(n, std::vector<grid>(n, blank_grid(subgrid_sz)));
        for (int j = 0; j < n; ++j) {
            for (int i = 0; i < n; ++i) {
                int x = i * subgrid_sz;
                int y = j * subgrid_sz;
                for (int sub_j = 0; sub_j < subgrid_sz; ++sub_j) {
                    for (int sub_i = 0; sub_i < subgrid_sz; ++sub_i) {
                        subgrids[j][i][sub_j][sub_i] = g[y + sub_j][x + sub_i];
                    }
                }
            }
        }
        return subgrids;
    }

    grid from_subgrids(const std::vector<std::vector<grid>>& subgrids) {
        int inp_n = subgrids.size();
        int subgrid_sz = subgrids.front().front().size();
        int out_n = inp_n * subgrid_sz;
        grid g = blank_grid(out_n);
        for (int j = 0; j < inp_n; ++j) {
            for (int i = 0; i < inp_n; ++i) {
                int x = i * subgrid_sz;
                int y = j * subgrid_sz;
                for (int sub_j = 0; sub_j < subgrid_sz; ++sub_j) {
                    for (int sub_i = 0; sub_i < subgrid_sz; ++sub_i) {
                        g[y + sub_j][x + sub_i] = subgrids[j][i][sub_j][sub_i];
                    }
                }
            }
        }
        return g;
    }

    std::vector<matrix> all_transformations(int sz) {
        std::vector<matrix> matrices;

        for (int i = 0; i < 4; ++i) {
            matrix mat = grid_rot_matrix(i, sz);
            matrices.push_back(mat);
        }

        for (int i = 0; i < 4; ++i) {
            matrix mat = grid_rot_matrix(i, sz) * grid_reflect_matrix(sz);
            matrices.push_back(mat);
        }

        return matrices;
    }

    std::tuple<grid, grid> parse_rule(const std::string& str) {
        auto parts = aoc::split(str, ' ');
        return {
            aoc::split(parts.front(), '/'),
            aoc::split(parts.back(), '/')
        };
    }

    subgrid_tbl to_rules_tbl(const std::vector<std::tuple<grid, grid>>& rules) {
        subgrid_tbl tbl;

        std::unordered_map<int, std::vector<matrix>> sz_to_trans = {
            {2, all_transformations(2)},
            {3, all_transformations(3)}
        };
        
        for ( const auto& [before, after] : rules) {
            for (const auto& mat : sz_to_trans.at(before.size())) {
                grid transformed = apply(before, mat);
                tbl[transformed] = after;
            }
        }

        return tbl;
    }

    grid initial_state() {
        return grid{
            ".#.",
            "..#",
            "###"
        };
    }

    int apply_rules(const subgrid_tbl& rules, int n) {
        auto g = initial_state();
        for (int i = 0; i < n; ++i) {
            auto subs = to_subgrids(g);
            for (auto& row : subs) {
                for (auto& sg : row) {
                    sg = rules.at(sg);
                }
            }
            g = from_subgrids(subs);
        }
        int count = 0;
        for (const auto& row : g) {
            for (auto ch : row) {
                count += ch == '#' ? 1 : 0;
            }
        }
        return count;
    }
}

void aoc::y2017::day_21(const std::string& title) {

    auto rules = to_rules_tbl(
        aoc::file_to_string_vector(
            aoc::input_path(2017, 21)
        ) | rv::transform(
            parse_rule
        ) | r::to<std::vector>()
    );

    std::println("--- Day 21: {} ---", title);
    std::println("  part 1: {}", apply_rules(rules, 5) );
    std::println("  part 2: {}", apply_rules(rules, 18) );
    
}
