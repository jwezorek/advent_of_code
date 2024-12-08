
#include "../util/util.h"
#include "../util/vec2.h"
#include "y2024.h"
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

    using point = aoc::vec2<int>;
    using point_set = aoc::vec2_set<int>;
    using antenna_map = std::unordered_map<char, std::vector<point>>;

    struct bounds {
        int wd;
        int hgt;
    };

    bool in_bounds(const bounds& bounds, const point& pt) {
        return pt.x >= 0 && pt.y >= 0 && pt.x < bounds.wd && pt.y < bounds.hgt;
    }

    std::tuple<antenna_map, bounds> parse_input(const std::vector<std::string>& grid) {
        auto bounds = ::bounds{
            static_cast<int>(grid.front().size()),
            static_cast<int>(grid.size())
        };

        antenna_map antennas;
        for (auto [x, y] : rv::cartesian_product(rv::iota(0, bounds.wd), rv::iota(0, bounds.hgt))) {
            auto tile = grid[y][x];
            if (tile != '.') {
                antennas[tile].emplace_back(x, y);
            }
        }

        return { std::move(antennas), bounds };
    }

    void antinodes_from_antennas(
            point_set& antinodes, const point& u, const point& v, const bounds& bounds) {
        auto u_to_v = v - u;
        auto v_to_u = u - v;
        auto antinode_1 = u + v_to_u;
        auto antinode_2 = v + u_to_v;
        if (in_bounds(bounds, antinode_1)) {
            antinodes.insert(antinode_1);
        }
        if (in_bounds(bounds, antinode_2)) {
            antinodes.insert(antinode_2);
        }
    }

    void antinodes_with_harmonics(
            point_set& antinodes, const point& u, const point& v, const bounds& bounds) {
        auto u_to_v = v - u;
        auto v_to_u = u - v;

        point loc = u;
        while (in_bounds(bounds, loc)) {
            if (in_bounds(bounds, loc)) {
                antinodes.insert(loc);
            }
            loc = loc + v_to_u;
        }

        loc = v;
        while (in_bounds(bounds, loc)) {
            if (in_bounds(bounds, loc)) {
                antinodes.insert(loc);
            }
            loc = loc + u_to_v;
        }
    }

    int count_antinodes(
            const antenna_map& antennas, const bounds& bounds, 
            const auto& find_antinodes) {

        point_set antinodes;
        for (const auto& locations : antennas | rv::values) {
            for (const auto& [u, v] : aoc::two_combinations(locations)) {
                find_antinodes(antinodes, u, v, bounds);
            }
        }

        return static_cast<int>(antinodes.size());
    }
}

void aoc::y2024::day_08(const std::string& title) {

    auto [antennas, bounds] = parse_input(
        aoc::file_to_string_vector(
            aoc::input_path(2024, 8)
        )
    );

    std::println("--- Day 8: {} ---", title);
    std::println("  part 1: {}", 
        count_antinodes(antennas, bounds, antinodes_from_antennas)
    );
    std::println("  part 2: {}",
        count_antinodes(antennas, bounds, antinodes_with_harmonics)
    );
    
}
