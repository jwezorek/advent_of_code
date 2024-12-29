
#include "../util/util.h"
#include "../util/vec2.h"
#include "y2016.h"
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

    struct node {
        point loc;
        int size;
        int used;
        int avail;
        int pcnt;
    };

    node parse_node(const std::string& str) {
        auto v = aoc::extract_numbers(str);
        return {
            {v[0],v[1]},
            v[2],
            v[3],
            v[4],
            v[5]
        };
    }

    int count_viable_pairs(const std::vector<node>& nodes) {
        return r::count_if(
            aoc::two_combinations(nodes),
            [](auto&& pair) {
                const auto [a, b] = pair;
                return (a.used > 0 && a.used <= b.avail) || 
                    (b.used > 0 && b.used <= a.avail);
            }
        );
    }
}

void aoc::y2016::day_22(const std::string& title) {

    auto inp = aoc::file_to_string_vector(
            aoc::input_path(2016, 22)
        ) | rv::drop(2) | rv::transform(
            parse_node
        ) | r::to<std::vector>();

    std::println("--- Day 22: {} ---", title);
    std::println("  part 1: {}", count_viable_pairs(inp) );
    std::println("  part 2: {}", 0);
    
}
