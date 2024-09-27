
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

    constexpr uint64_t k_gen_a_factor = 16807;
    constexpr uint64_t k_gen_b_factor = 48271;
    constexpr uint64_t k_modulus = 2147483647;
    constexpr uint64_t k_mask = 0xffff;

    uint64_t next_value(uint64_t v, uint64_t k, uint64_t mod) {
        const static auto next_val_fn = [](uint64_t v, uint64_t k)->uint64_t {
                return (v * k) % k_modulus;
            };
        do {
            v = next_val_fn(v, k);
        } while (v % mod != 0);
        return v;
    }

    std::tuple<uint64_t, uint64_t> next_values(
            uint64_t a, uint64_t b, uint64_t mod_a, uint64_t mod_b) {
        return {
            next_value(a, k_gen_a_factor, mod_a),  next_value(b, k_gen_b_factor, mod_b) 
        };
    }

    uint64_t count_matches(
            uint64_t a, uint64_t b, uint64_t n, uint64_t mod_a = 1, uint64_t mod_b = 1) {
        uint64_t count = 0;
        for (int i = 0; i < n; ++i) {
            std::tie(a, b) = next_values(a, b, mod_a, mod_b);
            if ((a & k_mask) == (b & k_mask)) {
                ++count;
            }
        }
        return count;
    }

}

void aoc::y2017::day_15(const std::string& title) {

    auto [gen_a, gen_b] = create_tuple<2>(
            extract_numbers(aoc::file_to_string( aoc::input_path(2017, 15) ))
        );
    std::println("--- Day 15: {} ---", title);
    std::println("  part 1: {}", count_matches(gen_a, gen_b, 40000000));
    std::println("  part 2: {}", count_matches(gen_a, gen_b, 5000000, 4, 8));
    
}
