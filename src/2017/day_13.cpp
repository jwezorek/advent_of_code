
#include "../util/util.h"
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

    struct scanner {
        int layer;
        int range;
    };

    bool caught(int delay, const scanner& s) {
        return (delay + s.layer) % (2 * (s.range-1)) == 0;
    }

    std::vector<scanner> caught_by_scanners(const std::vector<scanner>& scanners) {
        return scanners | rv::filter(
            [](auto&& scanner) {
                return caught(0, scanner);
            }
        ) | r::to<std::vector>();
    }

    bool was_caught_by_scanners(int delay, const std::vector<scanner>& scanners) {
        return r::find_if(scanners,
                [delay](auto&& scanner) {
                    return caught(delay, scanner);
                }
            ) != scanners.end();
    }

    int do_part_1(const std::vector<scanner>& scanners) {
        auto caught = caught_by_scanners(scanners);

        int severity = 0;
        for (auto v : caught) {
            severity += v.layer * v.range;
        }

        return severity;
    }

    int do_part_2(const std::vector<scanner>& scanners) {
        int delay = 1;
        while (was_caught_by_scanners(delay, scanners)) {
            ++delay;
        }
        return delay;
    }
}

void aoc::y2017::day_13(const std::string& title) {

    auto inp = aoc::file_to_string_vector(
            aoc::input_path(2017, 13)
        ) | rv::transform(
            [](auto&& str)->scanner {
                auto nums = aoc::extract_numbers(str);
                return { nums.front(), nums.back() };
            }
        ) | r::to<std::vector>();

    std::println("--- Day 13: {} ---", title);
    std::println("  part 1: {}", do_part_1(inp) );
    std::println("  part 2: {}", do_part_2(inp) );
    
}
