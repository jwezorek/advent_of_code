
#include "../util/util.h"
#include "../util/vec2.h"
#include "y2025.h"
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
    using point_set = aoc::vec2_set<int>;

    std::tuple<point, point_set> parse_input(const std::vector<std::string>& inp) {
        int wd = static_cast<int>(inp.front().size());
        int hgt = static_cast<int>(inp.size());
        point start = { -1,-1 }; 
        point_set splitters;
        for (auto y = 0; y < hgt; ++y) {
            for (int x = 0; x < wd; ++x) {
                auto ch = inp[y][x];
                if (ch == 'S') {
                    start = { x,y };
                    continue;
                }
                if (ch == '^') {
                    splitters.insert({ x,y });
                }
            }
        }
        return { start, splitters };
    }

    int do_part_1(const point& start, const point_set& splitters) {
        std::unordered_set<int> beams_xpos = { start.x };
        int y = start.y;
        int hgt = r::max(splitters | rv::transform([](auto&& p) {return p.y; })) + 1;
        int count = 0;

        while (y < hgt) {
            std::unordered_set<int> new_beams;
            for (auto x : beams_xpos) {
                if (splitters.contains({ x, y })) {
                    new_beams.insert( x - 1 );
                    new_beams.insert( x + 1 );
                    ++count;
                } else {
                    new_beams.insert( x );
                }
            }
            beams_xpos = new_beams;
            ++y;
        }

        return count;
    }

}

void aoc::y2025::day_07(const std::string& title) {

    auto [start, splitters] = parse_input(
            aoc::file_to_string_vector(
                aoc::input_path(2025, 7)
            )
        );

    std::println("--- Day 7: {} ---", title);
    std::println("  part 1: {}", do_part_1(start, splitters));
    std::println("  part 2: {}", 0);
    
}
