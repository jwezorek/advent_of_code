#include "../util.h"
#include "y2015.h"
#include <filesystem>
#include <functional>
#include <print>
#include <ranges>
#include <unordered_set>
#include "md5.h"

namespace r = std::ranges;
namespace rv = std::ranges::views;

/*------------------------------------------------------------------------------------------------*/

namespace {
    uint64_t find_hash(const std::string& key, int n) {
        auto target = std::string(n, '0');
        for (uint64_t i = 1; i < std::numeric_limits<uint64_t>::max(); ++i) {
            auto test = std::format("{}{}", key, std::to_string(i));
            auto hash = md5(test);
            if (hash.substr(0, n) == target) {
                return i;
            }
        }
        throw std::runtime_error("???");
    }
}

void aoc::y2015::day_04(const std::string& title) {

    auto key = aoc::file_to_string(aoc::input_path(2015, 4));

    std::println("--- Day 4: {} ---", title);
    std::println("  part 1: {}",
        find_hash(key, 5)
    );
    std::println("  part 2: {}",
        find_hash(key, 6)
    );
}