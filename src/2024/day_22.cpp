
#include "../util/util.h"
#include "y2024.h"
#include <filesystem>
#include <functional>
#include <print>
#include <ranges>
#include <unordered_set>

namespace r = std::ranges;
namespace rv = std::ranges::views;

/*------------------------------------------------------------------------------------------------*/

namespace {

    int64_t mix(int64_t u, int64_t v) {
        return u ^ v;
    }

    int64_t prune(int64_t u) {
        return u % 16777216;
    }

    int64_t do_one_generation(int64_t v) {
        v = prune(mix(v * 64, v));
        v = prune(mix(v / 32, v));
        v = prune(mix(v * 2048, v));
        return v;
    }

    int64_t do_part_1(const std::vector<int64_t>& secrets) {
        return r::fold_left(
            secrets | rv::transform(
                [](auto secret) {
                    for (int i = 0; i < 2000; ++i) {
                        secret = do_one_generation(secret);
                    }
                    return secret;
                }
            ),
            0ll,
            std::plus<int64_t>()
        );
    }
}

void aoc::y2024::day_22(const std::string& title) {

    auto inp = aoc::file_to_string_vector(
            aoc::input_path(2024, 22)
        ) | rv::transform(
            [](auto&& str) {
                return aoc::string_to_int64(str);
            }
        ) | r::to<std::vector>();

    std::println("--- Day 22: {} ---", title);
    std::println("  part 1: {}", do_part_1(inp) );
    std::println("  part 2: {}", 0);
    
}
