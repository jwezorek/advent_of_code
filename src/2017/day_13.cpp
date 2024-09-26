
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
        int position;
        int range;
        int dir;

        scanner(int rng = 0) : position(0), range(rng), dir(1)
        {}
    };
    using scanner_map = std::unordered_map<int, scanner>;

    void update_scanner(scanner& s) {
        int next = s.position + s.dir;
        if (next == s.range || next == -1) {
            s.dir *= -1;
        }
        s.position += s.dir;
    }

    void update_scanners(scanner_map& scanners) {
        for (auto& scanner : scanners | rv::values) {
            update_scanner(scanner);
        }
    }

    std::vector<int> caught_by_scanners(const scanner_map& inp) {
        std::vector<int> caught;
        int n = r::max(inp | rv::keys);
        scanner_map scanners = inp;

        for (int i = 0; i <= n; ++i) {
            if (scanners.contains(i)) {
                if (scanners[i].position == 0) {
                    caught.push_back(i);
                }
            }
            update_scanners(scanners);
        }

        return caught;
    }

    int do_part_1(const scanner_map& inp) {
        
        auto caught = caught_by_scanners(inp);

        int severity = 0;
        for (int v : caught) {
            severity += v * inp.at(v).range;
        }

        return severity;
    }

    int do_part_2(const scanner_map& inp) {
        int delay = 0;
        std::vector<int> caught;
        do {
            ++delay;
            if (delay % 10000 == 0) {
                std::println("{}", delay);
            }
            auto scanners = inp;
            for (int i = 0; i < delay; ++i) {
                update_scanners(scanners);
            }
            caught = caught_by_scanners(scanners);
        } while (!caught.empty());
        return delay;
    }
}

void aoc::y2017::day_13(const std::string& title) {

    auto inp = aoc::file_to_string_vector(
            aoc::input_path(2017, 13)
        ) | rv::transform(
            [](auto&& str)->scanner_map::value_type {
                auto nums = aoc::extract_numbers(str);
                return { nums.front(), scanner(nums.back()) };
            }
        ) | r::to<scanner_map>();

    std::println("--- Day 13: {} ---", title);
    std::println("  part 1: {}", do_part_1(inp) );
    std::println("  part 2: {}", 0 );
    
}
