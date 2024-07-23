#include <filesystem>
#include <functional>
#include <print>
#include <ranges>
#include <unordered_set>
#include "../util/util.h"
#include "../util/grid.h"
#include "y2018.h"

namespace r = std::ranges;
namespace rv = std::ranges::views;

/*------------------------------------------------------------------------------------------------*/

namespace {
    struct fabric_claim {
        int id;
        int x;
        int y;
        int wd;
        int hgt;
    };

    using grid = aoc::grid<int>;

    void insert_claim(const fabric_claim& c, grid& g) {
        for (int y = c.y; y < c.y + c.hgt; ++y) {
            for (int x = c.x; x < c.x + c.wd; ++x) {
                g(x, y) += 1;
            }
        }
    }

    grid find_counts(const std::vector<fabric_claim>& claims) {
        grid counts(1000,1000, 0);
        for (const auto& claim : claims) {
            insert_claim(claim, counts);
        }
        return counts;
    }

    bool is_non_overlapping_claim(const grid& g, const fabric_claim& c) {
        for (int y = c.y; y < c.y + c.hgt; ++y) {
            for (int x = c.x; x < c.x + c.wd; ++x) {
                if (g(x, y) != 1) {
                    return false;
                }
            }
        }
        return true;
    }

    int find_non_overlapping_claim(const grid& g, const std::vector<fabric_claim>& claims) {
        for (const auto& claim : claims) {
            if (is_non_overlapping_claim(g, claim)) {
                return claim.id;
            }
        }
        return -1;
    }
}

void aoc::y2018::day_03(const std::string& title) {

    auto inp = aoc::file_to_string_vector(
            aoc::input_path(2018, 3)
        ) | rv::transform(
            [](auto&& line)->fabric_claim {
                auto vals = aoc::extract_numbers(line, false);
                return {
                    vals[0],
                    vals[1],
                    vals[2],
                    vals[3],
                    vals[4]
                };
            }
        ) | r::to<std::vector>();

    std::println("--- Day 3: {} ---", title);

    auto grid = find_counts(inp);

    std::println("  part 1: {}", 
        r::count_if(
            grid, 
            [](auto v) {
                return v >= 2; 
            }
        )
    );

    std::println("  part 2: {}", find_non_overlapping_claim(grid, inp));
    
}
