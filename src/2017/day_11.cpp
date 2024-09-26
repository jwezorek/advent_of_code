
#include "../util/util.h"
#include "../util/vec3.h"
#include "y2017.h"
#include <filesystem>
#include <functional>
#include <print>
#include <ranges>
#include <unordered_map>

namespace r = std::ranges;
namespace rv = std::ranges::views;

/*------------------------------------------------------------------------------------------------*/

namespace {
    using hex_coords = aoc::vec3<int>;
    enum hex_dir {
        n, ne, se, s, sw, nw
    };

    hex_coords dir_to_offset(hex_dir d) {
        static const std::array<hex_coords, 6> tbl = { {
            {-1,0,1}, //n
            {-1,1,0}, //ne
            {0,1,-1}, //se
            {1,0,-1}, //s
            {1,-1,0}, //sw
            {0,-1,1}, //nw
        } };
        return tbl[static_cast<int>(d)];
    }

    hex_dir str_to_hex_dir(const std::string& str) {
        static const std::unordered_map<std::string, hex_dir> tbl = {
            {"n",n}, {"ne",ne}, {"se",se}, {"s", s}, {"sw", sw}, {"nw", nw}
        };
        return tbl.at(str);
    }

    int distance_from_origin(const hex_coords& pos) {
        return (std::abs(pos.x) + std::abs(pos.y) + std::abs(pos.z)) / 2;
    }

    std::tuple<int,int> traverse_hex_grid(const std::vector<hex_dir>& directions) {
        int furthest_dist = 0;
        hex_coords pos = { 0,0,0 };
        for (const auto dir : directions) {
            pos = pos + dir_to_offset(dir);
            furthest_dist = std::max(distance_from_origin(pos), furthest_dist);
        }
        return { distance_from_origin(pos), furthest_dist };
    }
}

void aoc::y2017::day_11(const std::string& title) {

    auto [ending_distance, furthest_distance] = traverse_hex_grid(
        aoc::split(
            aoc::trim(
                aoc::file_to_string(
                    aoc::input_path(2017, 11)
                )
            ),
            ','
        ) | rv::transform(
            str_to_hex_dir
        ) | r::to<std::vector>()
    );

    std::println("--- Day 11: {} ---", title);
    std::println("  part 1: {}", ending_distance );
    std::println("  part 2: {}", furthest_distance );
    
}
