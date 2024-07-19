#include "../util/util.h"
#include "../util/concat.h"
#include "y2020.h"
#include <filesystem>
#include <functional>
#include <print>
#include <ranges>
#include <unordered_set>
#include <unordered_map>
#include <boost/functional/hash.hpp>

namespace r = std::ranges;
namespace rv = std::ranges::views;

/*------------------------------------------------------------------------------------------------*/

namespace {
    struct vec4 {
        int w;
        int x;
        int y;
        int z;

        bool operator==(const vec4 rhs) const {
            return w == rhs.w && x == rhs.x && y == rhs.y && z == rhs.z;
        }
    };

    vec4 operator+(const vec4& lhs, const vec4& rhs) {
        return {
            lhs.w + rhs.w,
            lhs.x + rhs.x,
            lhs.y + rhs.y,
            lhs.z + rhs.z
        };
    }

    struct hash_vec4 {
        size_t operator()(const vec4& v) const {
            size_t seed = 0;
            boost::hash_combine(seed, v.w);
            boost::hash_combine(seed, v.x);
            boost::hash_combine(seed, v.y);
            boost::hash_combine(seed, v.z);
            return seed;
        }
    };

    using vec4_set = std::unordered_set<vec4, hash_vec4>;

    std::vector<vec4> basic_neighborhood(int dimensions) {
        auto fourth_dimension = (dimensions == 4) ? rv::iota(-1, 2) : rv::iota(0, 1);
        return rv::cartesian_product(
            fourth_dimension, rv::iota(-1, 2), rv::iota(-1, 2), rv::iota(-1, 2)
        ) | rv::transform(
            [](auto&& quadruple)->vec4 {
                auto [w, x, y, z] = quadruple;
                return {w, x, y, z };
            }
        ) | rv::filter(
            [](auto&& p) {
                return p != vec4{ 0,0,0,0 };
            }
        ) | r::to<std::vector<vec4>>();
    }

    auto neighbors(const vec4& p, int dimensions) {
        static const std::unordered_map<int, std::vector<vec4>> tbl = {
            {3, basic_neighborhood(3)},
            {4, basic_neighborhood(4)}
        };
        return tbl.at(dimensions) |
            rv::transform(
                [&p](auto&& delta)->vec4 {
                    return p + delta;
                }
            );
    };

    vec4_set set_and_neighbors(const vec4_set& cells, int dimensions) {
        auto adjacent_cells = cells | rv::transform(
                [dimensions](auto&& c) { return neighbors(c, dimensions); }
            ) | rv::join | r::to<vec4_set>();
        return aoc::concat(cells, adjacent_cells) | r::to<vec4_set>();
    }

    vec4_set do_one_generation(const vec4_set& cells, int dimensions) {
        vec4_set next_gen;
        auto neighborhood = set_and_neighbors(cells, dimensions);
        for (const auto& cell : neighborhood) {
            int neighbor_count = r::fold_left(
                neighbors(cell, dimensions) | rv::transform(
                    [&cells](const auto& loc)->int {
                        return cells.contains(loc) ? 1 : 0;
                    }
                ),
                0,
                std::plus<>()
            );
            if (cells.contains(cell)) {
                if (neighbor_count >= 2 && neighbor_count <= 3) {
                    next_gen.insert(cell);
                }
            } else {
                if (neighbor_count == 3) {
                    next_gen.insert(cell);
                }
            }
        }
        return next_gen;
    }

    int alive_after_six_generations(const vec4_set& inp, int dimensions) {
        vec4_set cells = inp;
        for (int i = 0; i < 6; ++i) {
            cells = do_one_generation(cells, dimensions);
        }
        return cells.size();
    }

    vec4_set parse_input(const std::vector<std::string> inp) {
        int cols = static_cast<int>( inp.front().size() );
        int rows = static_cast<int>(inp.size());
        vec4_set cells;
        for (int row = 0; row < rows; ++row) {
            for (int col = 0; col < cols; ++col) {
                if (inp[row][col] == '#') {
                    cells.insert(vec4{ 0, col, row, 0 });
                }
            }
        }
        return cells;
    }
}

void aoc::y2020::day_17(const std::string& title) {
    auto input = parse_input(
        aoc::file_to_string_vector(aoc::input_path(2020, 17))
    );

    std::println("--- Day 17: {} ---", title);
    std::println("  part 1: {}", alive_after_six_generations(input, 3) );
    std::println("  part 2: {}", alive_after_six_generations(input, 4) );
}