#include "y2023.h"
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

    loc_map<int> find_distances(const grid& g, const loc& start, std::optional<int> n = {}) {
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
            if (!n.has_value() || curr_state.count <= *n) {
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

    int64_t do_part_2(const grid& g) {
        auto all_distances = find_distances(g, { 65,65 });

        int64_t corners_odd = 0;
        int64_t corners_even = 0;
        int64_t full_odd = 0;
        int64_t full_even = 0;

        for (const auto dist : all_distances | rv::values) {
            if (dist > 65) {
                if (dist % 2 == 0) {
                    ++corners_even;
                } else {
                    ++corners_odd;
                }
            } 
            if (dist % 2 == 0) {
                ++full_even;
            } else {
                ++full_odd;
            }
        }

        int64_t n = ((26501365 - (g.size() / 2)) / g.size());

        return ((n + 1) * (n + 1)) * full_odd + (n * n) * full_even - (n + 1) * corners_odd + n * corners_even;
    }
}

/*------------------------------------------------------------------------------------------------*/

void aoc::y2023::day_21(const std::string& title) {

    auto grid = aoc::file_to_string_vector(aoc::input_path(2023, 21));
    auto start = find_start(grid);

    auto [cols, rows] = dimensions(grid);

    std::println("--- Day 21: {0} ---\n", title);
    std::println("  part 1: {}", count_even_steps(find_distances(grid, start, 64)));
    std::println("  part 2: {}", do_part_2(grid));
}

