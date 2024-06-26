#include "../util.h"
#include "y2019.h"
#include <filesystem>
#include <functional>
#include <print>
#include <ranges>
#include <unordered_set>

namespace r = std::ranges;
namespace rv = std::ranges::views;

/*------------------------------------------------------------------------------------------------*/

namespace {
    struct point {
        int x;
        int y;

        bool operator==(const point& p) const {
            return x == p.x && y == p.y;
        }
    };

    point operator+(const point& lhs, const point& rhs) {
        return  { lhs.x + rhs.x, lhs.y + rhs.y };
    }

    constexpr int k_dim = 5;

    uint64_t memoize(const std::vector<std::string>& grid) {
        uint64_t memo = 0;
        int binary_digit = 0;
        for (int y = 0; y < k_dim; ++y) {
            for (int x = 0; x < k_dim; ++x) {
                if (grid[y][x] == '#') {
                    memo = (memo | (static_cast<uint64_t>(1) << binary_digit));
                }
                binary_digit++;
            }
        }
        return memo;
    }

    char cell(const std::vector<std::string>& g, int x, int y) {
        if (x < 0 || y < 0 || x >= k_dim || y >= k_dim) {
            return '.';
        }
        return g.at(y).at(x);
    }

    std::vector<std::string> next_generation(const std::vector<std::string>& grid) {
        static const std::array<point, 4> deltas = {{ {0,-1}, {1,0}, {0,1}, {-1,0} }};
        std::vector<std::string> next_grid(k_dim, std::string(k_dim, '.'));
        for (int y = 0; y < k_dim; ++y) {
            for (int x = 0; x < k_dim; ++x) {
                int neighbors = r::count_if(
                    deltas | rv::transform(
                        [&](auto&& delta) {
                            return point{ x,y } + delta;
                        }
                    ),
                    [&](auto&& pt) {
                        return cell(grid, pt.x, pt.y) == '#';
                    }
                );
                if (grid[y][x] == '.') {
                    next_grid[y][x] = (neighbors == 1 || neighbors == 2) ? '#' : '.';
                } else {
                    next_grid[y][x] = (neighbors == 1) ? '#' : '.';
                }
            }
        }

        return next_grid;
    }

    void display(const std::vector<std::string>& grid) {
        std::println("-----");
        for (const auto& row : grid) {
            std::println("{}", row);
        }
        std::println("");
    }

    uint64_t do_part_1(const std::vector<std::string>& g) {
        auto grid = g;
        std::unordered_set<uint64_t> set;
        uint64_t memo = memoize(grid); 
        while (!set.contains(memo)) {
            set.insert(memo);
            grid = next_generation(grid);
            memo = memoize(grid);
        }
        return memo;
    }
}

void aoc::y2019::day_24(const std::string& title) {

    auto inp = aoc::file_to_string_vector(aoc::input_path(2019, 24));


    std::println("--- Day 24: {} ---", title);
    std::println("  part 1: {}",
        do_part_1(inp)
    );
    std::println("  part 2: {}",
        0
    );
}