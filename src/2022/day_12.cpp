#include "../util/util.h"
#include "y2022.h"
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

    template<typename T>
    using grid = std::vector<std::vector<T>>;

    struct grid_loc {
        int col;
        int row;

        bool operator==(const grid_loc& loc) const {
            return col == loc.col && row == loc.row;
        }
    };

    struct height_map {
        std::vector<grid_loc> src;
        grid_loc end;
        grid<int> grid;
    };

    height_map input_to_height_map(const std::vector<std::string>& input) {
        height_map hm;
        auto to_loc = [](auto x, auto y)->grid_loc {
            return { static_cast<int>(x), static_cast<int>(y) };
            };
        hm.grid = rv::enumerate(input) |
            rv::transform(
                [&](auto&& y_row)->std::vector<int> {
                    const auto [y, row] = y_row;
                    return rv::enumerate(row) |
                        rv::transform(
                            [&](auto x_item) {
                                auto [x, ch] = x_item;
                                if (ch == 'S') {
                                    hm.src.push_back(to_loc(x, y));
                                    return 0;
                                }
                                if (ch == 'E') {
                                    hm.end = to_loc(x, y);
                                    return 25;
                                }
                                return static_cast<int>(ch - 'a');
                            }
                        ) | r::to<std::vector<int>>();
                }
            ) | r::to<grid<int>>();
        return hm;
    }

    struct grid_loc_hasher {
        size_t operator()(const grid_loc& loc) const {
            size_t seed = 0;
            boost::hash_combine(seed, loc.col);
            boost::hash_combine(seed, loc.row);
            return seed;
        }
    };

    template<typename T>
    using loc_map = std::unordered_map<grid_loc, T, grid_loc_hasher>;

    template<typename T>
    std::tuple<int, int> dimensions(const grid<T>& g) {
        return {
            static_cast<int>(g[0].size()),
            static_cast<int>(g.size())
        };
    }

    template<typename T>
    grid<T> init_grid(std::tuple<int, int> dim, T val) {
        auto [wd, hgt] = dim;
        return grid<T>(
            hgt,
            std::vector<T>(wd, val)
        );
    }

    template<typename T>
    auto all_grid_locs(const grid<T>& g) {
        auto [wd, hgt] = dimensions(g);
        return rv::iota(0, hgt) |
            rv::transform(
                [wd](int row) {
                    return rv::iota(0, wd) |
                        rv::transform(
                            [row](auto col)->grid_loc {
                                return { col,row };
                            }
                    );
                }
        ) | rv::join;
    }

    auto neighbors(const grid<int>& g, const grid_loc& loc) {
        const static std::array<grid_loc, 4> neighborhood = { {
            {0,-1}, {1, 0}, {0,1}, {-1,0}
        } };
        auto [wd, hgt] = dimensions(g);
        auto val = g[loc.row][loc.col];
        return neighborhood |
            rv::transform(
                [loc](auto&& offset)->grid_loc {
                    return {
                        loc.col + offset.col,
                        loc.row + offset.row
                    };
                }
            ) | rv::filter(
                [val, wd, hgt, &g](auto&& loc) {
                    auto [x, y] = loc;
                    if (x < 0 || x >= wd) {
                        return false;
                    }
                    if (y < 0 || y >= hgt) {
                        return false;
                    }
                    if (g[y][x] > val + 1) {
                        return false;
                    }
                    return true;
                }
            );
    }

    int dijkstra_shortest_path(const height_map& hgt_map) {
        auto [wd, hgt] = dimensions(hgt_map.grid);

        grid<int> dist = init_grid({ wd,hgt }, wd * hgt + 1);
        for (auto loc : hgt_map.src) {
            dist[loc.row][loc.col] = 0;
        }

        aoc::priority_queue<grid_loc, grid_loc_hasher> queue;
        for (grid_loc loc : all_grid_locs(hgt_map.grid)) {
            queue.insert(loc, dist[loc.row][loc.col]);
        }

        while (!queue.empty()) {
            auto u = queue.extract_min();
            for (auto v : neighbors(hgt_map.grid, u)) {
                auto dist_through_u = dist[u.row][u.col] + 1;
                if (dist_through_u < dist[v.row][v.col]) {
                    dist[v.row][v.col] = dist_through_u;
                    queue.change_priority(v, dist_through_u);
                }
            }
        }

        return dist[hgt_map.end.row][hgt_map.end.col];
    }

}

void aoc::y2022::day_12(const std::string& title) {
    auto input = file_to_string_vector(input_path(2022, 12));
    auto hgt_map = input_to_height_map(input);

    std::println("--- Day 12: {} ---", title);
    std::println("  part 1: {}", dijkstra_shortest_path(hgt_map));
    std::println("  part 2: {}", 
        dijkstra_shortest_path(
            height_map{
                .src = all_grid_locs(hgt_map.grid) | rv::filter(
                        [&hgt_map](auto&& loc) { return hgt_map.grid[loc.row][loc.col] == 0; }
                    ) | r::to<std::vector<grid_loc>>(),
                .end = hgt_map.end,
                .grid = hgt_map.grid
            }
        )
    );
}