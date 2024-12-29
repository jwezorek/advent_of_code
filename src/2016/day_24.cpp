
#include "../util/util.h"
#include "../util/vec2.h"
#include "../util/concat.h"
#include "y2016.h"
#include <filesystem>
#include <functional>
#include <print>
#include <ranges>
#include <unordered_map>
#include <queue>

namespace r = std::ranges;
namespace rv = std::ranges::views;

/*------------------------------------------------------------------------------------------------*/

namespace {

    using matrix = std::vector<std::vector<int>>;
    using point = aoc::vec2<int>;
    using point_set = aoc::vec2_set<int>;

    size_t matrix_size(const std::vector<std::string>& grid) {
        auto str = grid | rv::join | r::to<std::string>();
        return static_cast<size_t>(r::max(aoc::extract_numbers(str))) + 1;
    }

    void populate_matrix(matrix& mat, const point& start, const std::vector<std::string>& grid) {
        auto u = grid[start.y][start.x] - '0';
        point_set visited;
        std::queue<std::tuple<point, int>> queue;
        queue.emplace(start, 0);
        while (!queue.empty()) {
            auto [loc, dist] = queue.front();
            queue.pop();

            if (visited.contains(loc)) {
                continue;
            }
            visited.insert(loc);

            auto tile = grid[loc.y][loc.x];
            if (std::isdigit(tile)) {
                mat[u][tile - '0'] = dist;
            }

            const static std::array<point, 4> deltas = {{ {0,-1},{1,0},{0,1},{-1,0} }};
            for (const auto& delta : deltas) {
                auto next_loc = loc + delta;
                if (grid[next_loc.y][next_loc.x] == '#') {
                    continue;
                }
                queue.emplace(next_loc, dist + 1);
            }
        }
    }

    matrix build_graph_matrix(const std::vector<std::string>& grid) {
        auto dim = matrix_size(grid);

        matrix mat(dim, std::vector<int>(dim, 0));

        for (auto y = 0; y < grid.size(); ++y) {
            for (auto x = 0; x < grid.front().size(); ++x) {
                auto tile = grid[y][x];
                if (std::isdigit(tile)) {
                    populate_matrix(mat, { x,y }, grid);
                }
            }
        }

        return mat;
    }

    int do_part_1(const std::vector<std::string>& grid) {
        auto matrix = build_graph_matrix(grid);

        auto min = std::numeric_limits<int>::max();
        auto n = static_cast<int>(matrix.size());
        auto path = rv::iota(1, n) | r::to<std::vector>();

        do {
            auto full_path = aoc::concat(rv::single(0), path) | r::to<std::vector>();
            auto path_len = r::fold_left(
                full_path | rv::adjacent_transform<2>(
                    [&](int u, int v) {
                        return matrix[u][v];
                    }
                ),
                0,
                std::plus<>()
            );
            min = std::min(min, path_len);
        } while (r::next_permutation(path).found);

        return min;
    }

    int do_part_2(const std::vector<std::string>& grid) {
        auto matrix = build_graph_matrix(grid);

        auto min = std::numeric_limits<int>::max();
        auto n = static_cast<int>(matrix.size());
        auto path = rv::iota(1, n) | r::to<std::vector>();

        do {
            auto full_path = aoc::concat(
                    rv::single(0), path, rv::single(0)
                ) | r::to<std::vector>();
            auto path_len = r::fold_left(
                full_path | rv::adjacent_transform<2>(
                    [&](int u, int v) {
                        return matrix[u][v];
                    }
                ),
                0,
                std::plus<>()
            );
            min = std::min(min, path_len);
        } while (r::next_permutation(path).found);

        return min;
    }
}

void aoc::y2016::day_24(const std::string& title) {

    auto inp = aoc::file_to_string_vector(
            aoc::input_path(2016, 24)
        ); 

    std::println("--- Day 24: {} ---", title);
    std::println("  part 1: {}", do_part_1(inp) );
    std::println("  part 2: {}", do_part_2(inp) );
    
}
