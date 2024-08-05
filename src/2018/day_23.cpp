
#include "../util/util.h"
#include "../util/vec3.h"
#include "y2018.h"
#include <filesystem>
#include <functional>
#include <print>
#include <ranges>
#include <unordered_set>

namespace r = std::ranges;
namespace rv = std::ranges::views;

/*------------------------------------------------------------------------------------------------*/

namespace {
    using point = aoc::vec3<int64_t>;

    struct nanobot {
        point loc;
        int64_t radius;
    };

    int64_t manhattan_distance(const point& lhs, const point& rhs) {
        return std::abs(lhs.x - rhs.x) +
            std::abs(lhs.y - rhs.y) +
            std::abs(lhs.z - rhs.z);
    }

    nanobot string_to_nanobot(const std::string& inp) {
        auto values = aoc::extract_numbers_int64(inp, true);
        return {
            {values[0], values[1], values[2]},
            values[3]
        };
    }

    int do_part_1(const std::vector<nanobot>& inp) {
        auto strongest_nanobot = r::max(
            inp,
            [](auto&& lhs, auto&& rhs) {
                return lhs.radius < rhs.radius;
            }
        );
        return r::count_if(
            inp,
            [&](auto&& nano) {
                return manhattan_distance(nano.loc, strongest_nanobot.loc) <= strongest_nanobot.radius;
            }
        );
    }
}

void aoc::y2018::day_23(const std::string& title) {

    auto inp = aoc::file_to_string_vector(
            aoc::input_path(2018, 23)
        ) | rv::transform(
            string_to_nanobot
        ) | r::to<std::vector>();

    std::println("--- Day 23: {} ---", title);
    std::println("  part 1: {}", do_part_1(inp) );
    std::println("  part 2: {}", 0);
    
}
