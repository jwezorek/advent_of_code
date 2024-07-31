
#include "../util/util.h"
#include "../util/vec2.h"
#include "y2018.h"
#include <filesystem>
#include <functional>
#include <print>
#include <ranges>
#include <unordered_set>
#include <unordered_map>

namespace r = std::ranges;
namespace rv = std::ranges::views;

/*------------------------------------------------------------------------------------------------*/

namespace {
    using grid = std::vector<std::string>;
    using point = aoc::vec2<int>;

    grid make_grid(int wd, int hgt) {
        return grid(hgt, std::string(wd, '.'));
    }

    auto neighbors(const point& pt, int wd, int hgt) {
        return rv::cartesian_product(
            rv::iota(-1, 2),
            rv::iota(-1, 2)
        ) | rv::transform(
            [pt](auto&& tup)->point {
                const auto& [x, y] = tup;
                return pt + point{ x,y };
            }
        ) | rv::filter(
            [pt,wd,hgt](auto&& adj) {
                return !(adj == pt || adj.x < 0 || adj.y < 0 || adj.x >= wd || adj.y >= hgt);
            }
        );
    }

    std::tuple<int, int> dimensions(const grid& g) {
        return {
            static_cast<int>(g.front().size()),
            static_cast<int>(g.size())
        };
    }

    std::tuple<int, int> neighbor_counts(const grid& g, const point& loc) {
        auto [wd, hgt] = dimensions(g);
        int trees = 0;
        int lumberyards = 0;
        for (auto&& adj : neighbors(loc, wd, hgt)) {
            auto tile = g.at(adj.y).at(adj.x);
            trees += (tile == '|') ? 1 : 0;
            lumberyards += (tile == '#') ? 1 : 0;
        }
        return { trees, lumberyards };
    }

    grid do_one_generation(const grid& g) {
        auto [wd, hgt] = dimensions(g);
        grid next = make_grid(wd, hgt);

        for (int y = 0; y < hgt; ++y) {
            for (int x = 0; x < wd; ++x) {
                point loc = { x,y };
                auto [trees, lumberyards] = neighbor_counts(g, loc);
                char next_tile = '.';
                switch (g.at(y).at(x)) {
                    case '.':
                        next_tile = (trees >= 3) ? '|' : '.';
                        break;
                    case '|':
                        next_tile = (lumberyards >= 3) ? '#' : '|';
                        break;
                    case '#':
                        next_tile = (lumberyards >= 1 && trees >= 1) ? '#' : '.';
                        break;
                }
                next[y][x] = next_tile;
            }
        }

        return next;
    }

    int total_resource_value(const grid& grid) {
        int lumberyards = r::count_if(grid | rv::join,
            [](char tile) { return tile == '#'; }
        );
        int trees = r::count_if(grid | rv::join,
            [](char tile) { return tile == '|'; }
        );
        return lumberyards * trees;
    }

    int do_part_1(const grid& inp) {
        auto grid = inp;
        for (int i = 0; i < 10; ++i) {
            grid = do_one_generation(grid);
        }
        return total_resource_value(grid);
    }

    std::string grid_to_string(const grid& g) {
        return g | rv::join | r::to<std::string>();
    }

    int do_part_2(const grid& inp, int64_t n) {

        struct state_record {
            int generation;
            int value;
        };

        auto grid = inp;
        std::unordered_map<std::string, state_record> state_to_value;

        int gen = 0;
        do {
            auto value = total_resource_value(grid);
            state_to_value[grid_to_string(grid)] = { gen, value };

            grid = do_one_generation(grid);
            gen++;

        } while (!state_to_value.contains(grid_to_string(grid)));

        int64_t i = state_to_value[grid_to_string(grid)].generation;
        int64_t R = gen;
        int64_t j = R - i;

        std::vector<int> s(R);
        for (const auto& val : state_to_value | rv::values) {
            s[val.generation] = val.value;
        }
        return s.at((n - i) % j + i);
    }
}

void aoc::y2018::day_18(const std::string& title) {

    auto inp = aoc::file_to_string_vector(
            aoc::input_path(2018, 18)
        ); 

    std::println("--- Day 18: {} ---", title);
    std::println("  part 1: {}", do_part_1(inp) );
    std::println("  part 2: {}", do_part_2(inp, 1000000000) );
    
}
