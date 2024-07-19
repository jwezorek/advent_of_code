#include "../util/util.h"
#include "y2015.h"
#include <filesystem>
#include <functional>
#include <print>
#include <ranges>
#include <unordered_set>

namespace r = std::ranges;
namespace rv = std::ranges::views;

/*------------------------------------------------------------------------------------------------*/

namespace {
    
    std::vector<int64_t> all_divisors(int64_t n)
    {
        std::vector<int64_t> divisors;
        for (int64_t i = 1; i <= std::sqrt(n); i++) {
            if (n % i == 0) {
                if (n / i == i) {
                    divisors.push_back( i );
                } else {
                    divisors.push_back(i);
                    divisors.push_back(n / i);
                }
            }
        }
        return divisors;
    }
    
    int64_t do_part_1(int64_t num) {
        int64_t v = 0;
        int i = 1;
        do {
            v = r::fold_left(
                rv::all(all_divisors(i)),
                0,
                std::plus<int64_t>()
            );
            v *= 10;
            ++i;
        } while (v < num);
        return i - 1;
    }

    int64_t do_part_2(int64_t num) {
        int64_t v = 0;
        int i = 1;
        do {
            auto sum_of_div = r::fold_left(
                rv::all(all_divisors(i)) | rv::filter(
                    [&](auto div) {
                        return i / div <= 50;
                    }
                ),
                0,
                std::plus<int64_t>()
            );
            v = sum_of_div * 11;
            ++i;
        } while (v < num);
        return i - 1;
    }
}

void aoc::y2015::day_20(const std::string& title) {

    auto num = aoc::string_to_int64(aoc::file_to_string(aoc::input_path(2015, 20)));

    std::println("--- Day 20: {} ---", title);
    std::println("  part 1: {}",
        do_part_1(num)
    );
    std::println("  part 2: {}",
        do_part_2(num)
    );
}