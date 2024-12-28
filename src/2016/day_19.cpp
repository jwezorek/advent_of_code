
#include "../util/util.h"
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
    int josephus(int n) {
        if (n == 0) {
            return n;
        }
        auto power = std::floor(std::log2(static_cast<double>(n)));
        return 2ull * (n - static_cast<uint64_t>(std::pow(2.0, power))) + 1ull;
    }

    int highest_power_of_3(int n) {
        auto log3 = std::floor(std::log10(static_cast<double>(n)) / std::log10(3.0));
        return static_cast<int>(std::pow(3.0, log3));
    }

    int n_cowboy_shootout(int n) {
        auto x = highest_power_of_3(n);
        if (x == n) {
            return x;
        }
        if (n < 2 * x) {
            return n % x;
        }
        return x + 2 * (n % x);

    }
}

void aoc::y2016::day_19(const std::string& title) {

    auto inp = std::stoi(
        aoc::file_to_string(
            aoc::input_path(2016, 19)
        )
    );

    std::println("--- Day 19: {} ---", title);
    std::println("  part 1: {}", josephus(inp) );
    std::println("  part 2: {}", n_cowboy_shootout(inp) );
    
}
