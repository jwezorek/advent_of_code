#include "../util.h"
#include "../concat.h"
#include "y2020.h"
#include <filesystem>
#include <functional>
#include <print>
#include <ranges>
#include <unordered_set>
#include <boost/functional/hash.hpp>

namespace r = std::ranges;
namespace rv = std::ranges::views;

/*------------------------------------------------------------------------------------------------*/

namespace {
    struct vec3 {
        int x;
        int y;
        int z;

        bool operator==(const vec3 rhs) const {
            return x == rhs.x && y == rhs.y && z == rhs.z;
        }
    };

    vec3 operator+(const vec3& lhs, const vec3& rhs) {
        return {
            lhs.x + rhs.x,
            lhs.y + rhs.y,
            lhs.z + rhs.z
        };
    }

    struct hash_vec3 {
        size_t operator()(const vec3& v) const {
            size_t seed = 0;
            boost::hash_combine(seed, v.x);
            boost::hash_combine(seed, v.y);
            boost::hash_combine(seed, v.z);
            return seed;
        }
    };

    using vec3_set = std::unordered_set<vec3, hash_vec3>;

    auto neighbors(const vec3& p) {
        static const std::vector<vec3> neighborhood = rv::cartesian_product(
                rv::iota(-1, 2), rv::iota(-1, 2), rv::iota(-1, 2)
            ) | rv::transform(
                [](auto&& triple)->vec3 {
                    auto [x, y, z] = triple;
                    return { x, y, z };
                }
            ) | rv::filter(
                [](auto&& p) {
                    return p != vec3{ 0,0,0 };
                }
                ) | r::to<std::vector<vec3>>();
        return neighborhood |
            rv::transform(
                [&p](auto&& delta)->vec3 {
                    return p + delta;
                }
        );
    };

    vec3_set set_and_neighbors(const vec3_set& cells) {
        auto adjacent_cells = cells | rv::transform(neighbors) | rv::join | r::to<vec3_set>();
        return aoc::concat(cells, adjacent_cells) | r::to<vec3_set>();
    }

    vec3_set do_one_generation(const vec3_set& cells) {
        vec3_set next_gen;
        auto neighborhood = set_and_neighbors(cells);
        for (const auto& cell : neighborhood) {
            int neighbor_count = r::fold_left(
                neighbors(cell) | rv::transform(
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

    int do_part_1(const vec3_set& inp) {
        vec3_set cells = inp;
        for (int i = 0; i < 6; ++i) {
            cells = do_one_generation(cells);
        }
        return cells.size();
    }

    vec3_set parse_input(const std::vector<std::string> inp) {
        int cols = static_cast<int>( inp.front().size() );
        int rows = static_cast<int>(inp.size());
        vec3_set cells;
        for (int row = 0; row < rows; ++row) {
            for (int col = 0; col < cols; ++col) {
                if (inp[row][col] == '#') {
                    cells.insert(vec3{ col, row, 0 });
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
    std::println("  part 1: {}", do_part_1(input) );
    std::println("  part 2: {}", 0);
}