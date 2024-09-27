
#include "../util/util.h"
#include "../util/vec2.h"
#include "knot_hash.h"
#include "y2017.h"
#include <filesystem>
#include <functional>
#include <print>
#include <ranges>
#include <unordered_set>
#include <bitset>
#include <queue>

namespace r = std::ranges;
namespace rv = std::ranges::views;

/*------------------------------------------------------------------------------------------------*/

namespace {

    using loc = aoc::vec2<int>;
    using loc_set = aoc::vec2_set<int>;

    std::string to_binary(int val) {
        std::bitset<8> binary(val); 
        return binary.to_string();
    }

    std::string generate_row(const std::string& key, int row) {
        std::string row_key = key + "-" + std::to_string(row);
        auto vals = aoc::knot_hash_to_values(row_key);
        auto binary_str = vals | rv::transform(
                to_binary
            ) | rv::join | r::to<std::string>();
        return binary_str | rv::transform(
                [](auto ch) {
                    return (ch == '1') ? '#' : '.';
                }
            ) | r::to<std::string>();
    }

    std::vector<std::string> generate_grid(const std::string& key) {
        return rv::iota(0, 128) |
            rv::transform(
                [key](auto row) {
                    return generate_row(key, row);
                }
            ) | r::to<std::vector>();
    }

    int count_used(const std::vector<std::string>& grid) {
        return r::fold_left(
            grid | rv::transform(
                [](auto&& row)->int {
                    return r::count_if(row, [](auto ch) {return ch == '#'; });
                }
            ),
            0,
            std::plus<>()
        );
    }

    auto neighbors(const loc& loc) {
        static const std::array<::loc, 4> deltas = { {
            {0,-1},{1,0},{0,1},{-1,0}
        } };

        return deltas | rv::transform(
                [loc](::loc delta)->::loc {
                    return loc + delta;
                }
            ) | rv::filter(
                [](auto loc) {
                    return loc.x >= 0 && loc.x < 128 && loc.y >= 0 && loc.y < 128;
                }
            );
    }

    void find_connected_component(
            const std::vector<std::string>& grid, loc_set& visited, const loc& start) {

        std::queue<loc> queue;
        queue.push(start);

        while (!queue.empty()) {
            auto curr = queue.front();
            queue.pop();

            if (visited.contains(curr)) {
                continue;
            }
            visited.insert(curr);

            for (auto adj : neighbors(curr)) {
                if (grid[adj.y][adj.x] == '#') {
                    queue.push(adj);
                }
            }
        }
    }

    int count_connected_components(const std::vector<std::string>& grid) {
        loc_set visited;
        int count = 0;
        for (int y = 0; y < 128; ++y) {
            for (int x = 0; x < 128; ++x) {
                loc start(x, y);
                if (grid[y][x] == '#' && !visited.contains(start)) {
                    ++count;
                    find_connected_component(grid, visited, start);
                }
            }
        }
        return count;
    }
}

void aoc::y2017::day_14(const std::string& title) {

    auto inp = aoc::file_to_string(
            aoc::input_path( 2017, 14 )
        ); 

    auto grid = generate_grid( inp );

    std::println("--- Day 14: {} ---", title);
    std::println("  part 1: {}", count_used(grid) );
    std::println("  part 2: {}", count_connected_components(grid) );
    
}
