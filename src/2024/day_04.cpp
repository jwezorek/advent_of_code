#include "../util/util.h"
#include "../util/vec2.h"
#include "y2024.h"
#include <filesystem>
#include <functional>
#include <print>
#include <ranges>
#include <unordered_set>

namespace r = std::ranges;
namespace rv = std::ranges::views;

/*------------------------------------------------------------------------------------------------*/

namespace {
    using point = aoc::vec2<int>;
    using grid = std::vector<std::string>;

    std::tuple<int, int> bounds(const grid& g) {
        return {
            static_cast<int>(g.front().size()),
            static_cast<int>(g.size())
        };
    }

    bool in_bounds(const grid& g, const point& loc) {
        auto [cols, rows] = bounds(g);
        return loc.x >= 0 && loc.y >= 0 && loc.x < cols && loc.y < rows;
    }

    std::string string_in_dir(const grid& g, const point& start, const point& dir, int len) {
        return rv::iota(0, len) | rv::transform(
            [&](auto i) {
                auto loc = start + i * dir;
                return (in_bounds(g, loc)) ? g[loc.y][loc.x] : ' ';
            }
        ) | r::to<std::string>();
    }

    int count_xmas_strings(const grid& g) {
        auto [cols, rows] = bounds(g);
        static const std::array<point, 8> dirs = {{
            {0,-1}, {1,-1}, {1,0}, {1,1}, {0,1}, {-1,1}, {-1,0}, {-1, -1}
        }};
        int count = 0;
        for (int row = 0; row < rows; ++row) {
            for (int col = 0; col < cols; ++col) {
                for (const auto& dir : dirs) {
                    auto str = string_in_dir(g, { col,row }, dir, 4);
                    if (str == "XMAS") {
                        ++count;
                    }
                }
            }
        }
        return count;
    }

    bool is_xmas_cross(const grid& g, const point& loc) {

        auto nw_str = string_in_dir(g, loc + point{ -1,-1 }, { 1,1 }, 3);
        if (nw_str != "MAS" && nw_str != "SAM") {
            return false;
        }

        auto ne_str = string_in_dir(g, loc + point{ 1,-1 }, { -1,1 }, 3);
        if (ne_str != "MAS" && ne_str != "SAM") {
            return false;
        }

        return true;
    }

    int count_xmas_crosses(const grid& g) {
        auto [cols, rows] = bounds(g);
        int count = 0;
        for (int row = 0; row < rows; ++row) {
            for (int col = 0; col < cols; ++col) {
                if (is_xmas_cross(g, {col,row})) {
                    ++count;
                }
            }
        }
        return count;
    }
}

void aoc::y2024::day_04(const std::string& title) {

    auto grid = aoc::file_to_string_vector(
            aoc::input_path(2024, 4)
        ); 

    std::println("--- Day 4: {} ---", title);
    std::println("  part 1: {}", count_xmas_strings(grid));
    std::println("  part 2: {}", count_xmas_crosses(grid) );
    
}
