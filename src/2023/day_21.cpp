#include "days.h"
#include "../util.h"
#include <print>
#include <ranges>
#include <numeric>
#include <algorithm>
#include <unordered_set>
#include <unordered_map>
#include <tuple>
#include <queue>
#include <boost/functional/hash.hpp>


namespace r = std::ranges;
namespace rv = std::ranges::views;

namespace {

    struct loc {
        int col;
        int row;
    };

    loc operator+(const loc& lhs, const loc& rhs) {
        return {
            lhs.col + rhs.col,
            lhs.row + rhs.row
        };
    }

    bool operator==(const loc& lhs, const loc& rhs) {
        return lhs.col == rhs.col && lhs.row == rhs.row;
    }

    struct loc_hash {
        size_t operator()(const loc& loc) const {
            size_t seed = 0;
            boost::hash_combine(seed, loc.col);
            boost::hash_combine(seed, loc.row);
            return seed;
        }
    };

    template<typename T>
    using loc_map = std::unordered_map<loc, T, loc_hash>;

    using loc_set = std::unordered_set<loc, loc_hash>;
    using grid = std::vector<std::string>;

    std::tuple<int, int> dimensions(const grid& g) {
        return {
            static_cast<int>(g.front().size()),
            static_cast<int>(g.size())
        };
    }

    loc find_start(grid& g) {
        auto [cols, rows] = dimensions(g);
        for (int row = 0; row < rows; ++row) {
            for (int col = 0; col < cols; ++col) {
                if (g[col][row] == 'S') {
                    g[col][row] = '.';
                    return { col,row };
                }
            }
        }
        throw std::runtime_error("no start");
    }

    bool in_bounds(const loc& v, int rows, int cols) {
        return v.row >= 0 && v.row < rows && v.col >= 0 && v.col < cols;
    }

    std::vector<loc> neighbors(const grid& g, const loc& v) {
        auto [cols, rows] = dimensions(g);
        static const std::array<loc, 4> offsets = {{
            {0,-1}, {-1,0}, {0,1}, {1,0}
        }};
        return offsets | rv::transform(
            [&v](auto&& offset) {
                return v + offset;
            }
        ) | rv::filter(
            [&](auto&& v) {
                return in_bounds(v, rows, cols) && g[v.col][v.row] == '.';
            }
       ) | r::to< std::vector<loc>>();
    }

    struct state {
        loc pos;
        int64_t count;
    };

    loc_map<int> find_distances(const grid& g, const loc& start, int n) {
        loc_map<int> dist;
        std::queue<state> queue;
        queue.push({ start,0 });

        while (!queue.empty()) {
            auto curr_state = queue.front();
            queue.pop();
            if (dist.contains(curr_state.pos)) {
                continue;
            }
            dist[curr_state.pos] = curr_state.count;
            if (curr_state.count <= n) {
                for (auto neighbor : neighbors(g, curr_state.pos)) {
                    queue.push({ neighbor, curr_state.count + 1 });
                }
            }
        }

        return dist;
    }

    int count_even_steps(const loc_map<int>& distances) {
        return r::fold_left(
            distances | rv::values | rv::transform(
                [](int steps) {
                    return (steps + 1) % 2;
                }
            ),
            0,
            std::plus<>()
        );
    }

    int count_odd_steps(const loc_map<int>& distances) {
        return r::fold_left(
            distances | rv::values | rv::transform(
                [](int steps) {
                    return steps% 2;
                }
            ),
            0,
            std::plus<>()
        );
    }

    int get_repetition_constant(int tile_dim, int steps) {
        auto k = (static_cast<double>(steps) - static_cast<double>(tile_dim) / 2.0) / tile_dim;
        return static_cast<int>(std::ceil(k));
    }

    int get_steps_in_fundamental_region(int tile_dim, int steps) {
        auto n = get_repetition_constant(tile_dim, steps) - 1;
        auto final_steps = (steps - tile_dim / 2) % tile_dim;
        if (final_steps == 0) {
            final_steps = tile_dim;
        }
        auto val = (tile_dim / 2) + tile_dim + final_steps;
        return val;
    }

    grid make_fundamental_region(const grid& inp) {
        auto one_big_row = inp | rv::transform(
                [](auto&& row) {
                    return std::format("{}{}{}{}{}", row, row, row, row, row);
                }
            ) | r::to<grid>();

        grid output;
        output.reserve(inp.size() * 5);
        for (int i = 0; i < 5; ++i) {
            r::copy(one_big_row, std::back_inserter(output));
        }

        return output;
    }

    loc_map<int> find_distances_in_fundamental_region(const grid& g, const loc& start, int steps) {
        auto [cols, rows] = dimensions(g);
        auto tile_dim = cols;
        auto fund_region = make_fundamental_region(g);
        auto fund_steps = get_steps_in_fundamental_region(tile_dim, steps);
        return find_distances(fund_region, start + loc{ 2 * tile_dim, 2 * tile_dim }, fund_steps);
    }

    loc_map<int> fund_region_piece(const loc_map<int>& fund, int col, int row, int tile_dim) {
        loc_map<int> output;
        for (int j = row * tile_dim; j < row * tile_dim + tile_dim; ++j) {
            for (int i = col * tile_dim; i < col * tile_dim + tile_dim; ++i) {
                if (fund.contains({ i,j })) {
                    output[{ i, j }] = fund.at({i, j});
                }
            }
        }
        return output;
    }
}

/*------------------------------------------------------------------------------------------------*/

void aoc::y2023::day_21(const std::string& title) {

    auto grid = aoc::file_to_string_vector(aoc::input_path(2023, 21));
    auto start = find_start(grid);

    auto [cols, rows] = dimensions(grid);

    std::println("--- Day 21: {0} ---\n", title);
    std::println("  part 1: {}", count_even_steps(find_distances(grid, start, 64)));

    auto tile_dim = cols;
    int part_2_steps = 26501365;
    loc_map<int> fund_dist = find_distances_in_fundamental_region(grid, start, part_2_steps);
    for (int row = 0; row < 5; ++row) {
        for (int col = 0; col < 5; ++col) {
            auto piece = fund_region_piece(fund_dist, col, row, tile_dim);
            int odd_count = r::fold_left(
                piece | rv::values | rv::transform(
                    [](int steps) {
                        return steps % 2;
                    }
                ),
                0,
                std::plus<>()
            );
            std::print("{:5}", odd_count);
        }
        std::println("");
    }
    

}