#include "../util/util.h"
#include "y2020.h"
#include <filesystem>
#include <functional>
#include <print>
#include <ranges>
#include <unordered_map>

namespace r = std::ranges;
namespace rv = std::ranges::views;

/*------------------------------------------------------------------------------------------------*/

namespace {
    using grid = std::vector<std::string>;

    std::tuple<int, int> dimensions(const grid& g) {
        return {
            static_cast<int>(g.front().size()),
            static_cast<int>(g.size())
        };
    }

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

    loc wrap_horz(const loc& loc, int cols) {
        return {
            (loc.col >= cols) ? loc.col % cols : loc.col,
            loc.row
        };
    }

    int count_trees(const grid& g, const loc& delta) {
        auto [cols, rows] = dimensions(g);
        auto pos = loc(0, 0);
        int count = 0;
        while (pos.row < rows) {
            if (g[pos.row][pos.col] == '#') {
                ++count;
            }
            pos = wrap_horz(pos + delta, cols);
        }
        return count;
    }
}

void aoc::y2020::day_03(const std::string& title) {
    auto input = aoc::file_to_string_vector(aoc::input_path(2020, 3));

    std::println("--- Day 3: {} ---", title);
    std::println("  part 1: {}", count_trees(input, {3,1}));

    std::array<loc, 5> slopes = {{ {1,1},{3,1},{5,1},{7,1},{1,2} }};
    std::println("  part 2: {}",
        r::fold_left(
            slopes | rv::transform(
                [&input](const loc& slope)->int64_t {
                    return count_trees(input, slope);
                }
            ),
            1,
            std::multiplies<int64_t>()
        )
    );
}