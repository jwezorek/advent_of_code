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
#include <map>
#include <sstream>
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
                    return steps % 2;
                }
            ),
            0,
            std::plus<>()
        );
    }

    grid tile_region(const grid& inp, int sz) {
        auto one_big_row = inp | rv::transform(
                [sz](auto&& row) {
                    std::stringstream ss;
                    for (int i = 0; i < sz; ++i) {
                        ss << row;
                    }
                    return ss.str();
                }
            ) | r::to<grid>();

        grid output;
        output.reserve(inp.size() * sz);
        for (int i = 0; i < sz; ++i) {
            r::copy(one_big_row, std::back_inserter(output));
        }

        return output;
    }

    loc_map<int> find_distances_in_tiled_region(const grid& g, int sz) {
        auto [cols, rows] = dimensions(g);
        int tile_dim = cols;
        int steps = (sz * tile_dim) / 2;
        auto region = tile_region(g, sz);
        int start = (tile_dim * sz) / 2 + 1;
        return find_distances(region, { start,start }, steps);
    }

    loc_map<int> tile_distances(const loc_map<int>& rgn, int col, int row, int tile_dim) {
        loc_map<int> output;
        for (int j = row * tile_dim; j < row * tile_dim + tile_dim; ++j) {
            for (int i = col * tile_dim; i < col * tile_dim + tile_dim; ++i) {
                if (rgn.contains({ i,j })) {
                    output[{ i, j }] = rgn.at({i, j});
                }
            }
        }
        return output;
    }
}

std::string summarize(const loc_map<int>& region, int sz, int tile_dim) {
    std::map<int, int> unique_nums;
    int sum = 0;
    for (int row = 0; row < sz; ++row) {
        for (int col = 0; col < sz; ++col) {
            auto td = tile_distances(region, col, row, tile_dim);
            auto count = count_odd_steps(td);
            sum += count;
            unique_nums[count] += 1;
        }
    }
    std::stringstream ss;
    ss << std::format("{:6} ", sum);
    for (auto [num, count] : unique_nums) {
        if (num > 0) {
            ss << std::format("{:5}:{:3} ", num, count);
        }
    }
    return ss.str();
}

int64_t get_index(int64_t sz) {
    return sz / 4;
}

std::string summarize2(const loc_map<int>& region, int sz, int tile_dim) {
    std::map<int, int> unique_nums;
    int sum = 0;
    for (int row = 0; row < sz; ++row) {
        for (int col = 0; col < sz; ++col) {
            auto td = tile_distances(region, col, row, tile_dim);
            auto count = count_odd_steps(td);
            sum += count;
            unique_nums[count] += 1;
        }
    }

    std::map<int, int> count_to_num_sum;
    for (auto [num, count] : unique_nums) {
        count_to_num_sum[count] += num;
    }

    std::stringstream ss;
    ss << std::format("[{:6}] ", sum);
    for (auto [count, num] : count_to_num_sum) {
        if (num > 0) {
            ss << std::format("{:5}:{:3} ", num, count);
        }
    }

    return ss.str();
}

int64_t count_for_size(int64_t sz) {

    auto i = get_index(sz);
    int64_t count = 23386;
    count += 27313 * (i * 2);
    count += 4062 * (i * 2 + 1);

    int64_t j = 2 * i;
    int64_t squ = j * j;
    count += 7747 * squ;

    j++;
    squ = j * j;
    count += 7702 * squ;

    return count;
}

/*------------------------------------------------------------------------------------------------*/

void aoc::y2023::day_21(const std::string& title) {

    auto grid = aoc::file_to_string_vector(aoc::input_path(2023, 21));
    auto start = find_start(grid);

    auto [cols, rows] = dimensions(grid);

    std::println("--- Day 21: {0} ---\n", title);
    std::println("  part 1: {}", count_even_steps(find_distances(grid, start, 64)));

    int tile_dim = cols;
    int64_t part_2_steps = 26501365;
    int64_t part_2_sz = (2*part_2_steps + 1)/tile_dim;

    std::println("  part 2: {}", count_for_size(part_2_sz));

    std::println("part 2 sz => {}, sanity check => {}", part_2_sz, (part_2_sz * tile_dim) / 2);
    for (int sz = 127; sz < 183; sz += 4) {
        auto region = find_distances_in_tiled_region(grid, sz);
        std::println("{} {} {}", sz, count_for_size(sz), count_odd_steps(region));  
    }
    
}

