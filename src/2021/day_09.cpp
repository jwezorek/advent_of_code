#include "../util/util.h"
#include "y2021.h"
#include <filesystem>
#include <functional>
#include <print>
#include <ranges>
#include <unordered_set>

namespace r = std::ranges;
namespace rv = std::ranges::views;

/*------------------------------------------------------------------------------------------------*/

namespace {

    std::vector<int> get_neighbors(const std::vector<std::vector<int>>& height_map, int x, int y) {
        int wd = height_map[0].size();
        int hgt = height_map.size();

        std::vector<int> neighbors;

        if (x != 0) {
            neighbors.push_back(height_map[y][x - 1]);
        }
        if (y != 0) {
            neighbors.push_back(height_map[y - 1][x]);
        }
        if (x < wd - 1) {
            neighbors.push_back(height_map[y][x + 1]);
        }
        if (y < hgt - 1) {
            neighbors.push_back(height_map[y + 1][x]);
        }

        return neighbors;
    }

    std::vector<std::tuple<int, int>> find_minima(const std::vector<std::vector<int>>& height_map) {
        int wd = height_map[0].size();
        int hgt = height_map.size();

        std::vector<std::tuple<int, int>>  minima;
        for (int y = 0; y < hgt; ++y) {
            for (int x = 0; x < wd; ++x) {
                auto neighbors = get_neighbors(height_map, x, y);
                auto val = height_map[y][x];

                bool is_minima = true;
                for (const auto neighbor : neighbors) {
                    if (neighbor <= val) {
                        is_minima = false;
                        break;
                    }
                }
                if (is_minima) {
                    minima.push_back({ x,y });
                }
            }
        }
        return minima;
    }

    int score_puzzle_1(const std::vector<std::vector<int>>& height_map, const std::vector<std::tuple<int, int>>& minima) {
        return r::fold_left(
            minima | rv::transform(
                [&](const auto& pt) {
                    auto [x, y] = pt;
                    return 1 + height_map[y][x];
                }
            ),
            0,
            std::plus<>()
        );
    }

    using key_t = std::tuple<int, int>;

    struct tuple_hash_t {
        std::size_t operator()(const key_t& k) const {
            return std::hash<int>{}(std::get<0>(k) ^ std::get<1>(k));
        }
    };

    std::vector<std::tuple<int, int>> get_4cell_neighbor_locs(const std::vector<std::vector<int>>& g, int x, int y) {
        int hgt = g.size();
        int wd = g[0].size();

        std::vector<std::tuple<int, int>> neighbors;
        if (x != 0) {
            neighbors.push_back({ x - 1, y });
        }
        if (y != 0) {
            neighbors.push_back({ x, y - 1 });
        }
        if (x < wd - 1) {
            neighbors.push_back({ x + 1, y });
        }
        if (y < hgt - 1) {
            neighbors.push_back({ x, y + 1 });
        }

        return neighbors;
    }

    int find_basin_size(const std::vector<std::vector<int>>& height_map, int x, int y) {
        std::unordered_set<std::tuple<int, int>, tuple_hash_t> visited;
        std::function<int(int, int)> visit;

        int wd = height_map[0].size();
        int hgt = height_map.size();

        visit = [&](int x, int y)->int {
            if (height_map[y][x] == 9 || visited.find(key_t{ x,y }) != visited.end()) {
                return 0;
            }
            visited.insert(key_t{ x,y });
            auto neighbors = get_4cell_neighbor_locs(height_map, x, y);
            int count = 1;
            for (auto [neigh_x, neigh_y] : neighbors) {
                count += visit(neigh_x, neigh_y);
            }

            return count;
            };

        return visit(x, y);
    }

    int puzzle_2(const std::vector<std::vector<int>>& height_map, const std::vector<std::tuple<int, int>>& minima) {
        std::vector<int> basin_size = minima | rv::transform(
                [&](const auto& basin_loc)->int {
                    auto [x, y] = basin_loc;
                    return find_basin_size(height_map, x, y);
                }
            ) | r::to<std::vector<int>>();
        r::sort(basin_size);
        r::reverse(basin_size);

        return basin_size[0] * basin_size[1] * basin_size[2];
    }
}

void aoc::y2021::day_09(const std::string& title) {
    auto input = aoc::file_to_string_vector(aoc::input_path(2021, 9));
    auto grid = strings_to_2D_array_of_digits(input);
    auto minima = find_minima(grid);

    std::println("--- Day 9: {} ---", title);
    std::println("  part 1: {}", score_puzzle_1(grid, minima));
    std::println("  part 2: {}", puzzle_2(grid, minima));
}