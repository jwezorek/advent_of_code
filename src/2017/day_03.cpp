
#include "../util/util.h"
#include "../util/vec2.h"
#include "y2017.h"
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
    using point_map = aoc::vec2_map<int, int>;

    point ulam_spiral_coordinates(int n) {

        // Layer of the spiral: the smallest k where (2k-1)^2 < n <= (2k+1)^2
        int k = static_cast<int>(std::ceil((std::sqrt(n) - 1) / 2));

        // The start and end of the layer
        int layer_start = (2 * k - 1) * (2 * k - 1) + 1;
        int layer_end = (2 * k + 1) * (2 * k + 1);

        int side_length = 2 * k;  // Each side of this layer is 2k long
        int offset = n - layer_start;  // Position within this layer

        point result;

        // Adjust for counterclockwise movement with first step to the right:
        if (offset < side_length) {
            result.x = k;
            result.y = -k + offset + 1;
        } else if (offset < 2 * side_length) {
            result.x = k - (offset - side_length + 1);
            result.y = k;
        } else if (offset < 3 * side_length) {
            result.x = -k;
            result.y = k - (offset - 2 * side_length + 1);
        } else {
            result.x = -k + (offset - 3 * side_length + 1);
            result.y = -k;
        }

        return result;
    }

    int manhattan_distance_from_origin(const point& pt) {
        return std::abs(pt.x) + std::abs(pt.y);
    }

    int do_part_two(int n) {

        static const std::array<point, 8> neighbors = {{
            {1,0},{1,1},{0,1},{-1,1},{-1,0},{-1,-1},{0,-1},{1,-1}
        }};

        point_map spiral;
        spiral[{0, 0}] = 1;
        int i = 2;
        int val = 0;

        do {
            auto loc = ulam_spiral_coordinates(i++);
            val = 0;
            for (const auto& adj : neighbors | rv::transform([&](auto v) {return v + loc; })) {
                val += spiral.contains(adj) ? spiral.at(adj) : 0;
            }
            spiral[loc] = val;
        } while (val < n);

        return val;
    }
}

void aoc::y2017::day_03(const std::string& title) {

    auto inp = std::stoi(
            aoc::file_to_string(
                aoc::input_path(2017, 3)
            )
        );

    std::println("--- Day 3: {} ---", title);
    std::println("  part 1: {}", 
        manhattan_distance_from_origin(
            ulam_spiral_coordinates(inp)
        )
    );
    std::println("  part 2: {}", do_part_two(inp) );
    
}
