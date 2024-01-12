#include "../util.h"
#include "y2020.h"
#include <filesystem>
#include <functional>
#include <print>
#include <ranges>
#include <unordered_map>

namespace r = std::ranges;
namespace rv = std::ranges::views;

/*------------------------------------------------------------------------------------------------*/

namespace {

    int64_t transform_subj_number(int64_t subj_num, int loop_size) {
        int64_t n = 1;
        for (int i = 0; i < loop_size; ++i) {
            n = n * subj_num;
            n = n % 20201227;
        }
        return n;
    }

    int64_t find_loop_size(int64_t public_key) {
        int64_t n = 1;
        int64_t subj_num = 7;
        int64_t loop_size = 0;

        do {
            n = n * subj_num;
            n = n % 20201227;
            loop_size++;
        } while (n != public_key);

        return loop_size;
    }

    int64_t find_encryption_key(int64_t card_public_key, int64_t door_public_key) {
        auto card_loop_sz = find_loop_size(card_public_key);
        return transform_subj_number(door_public_key, card_loop_sz);
    }
}

void aoc::y2020::day_25(const std::string& title) {
    auto input = aoc::file_to_string_vector(aoc::input_path(2020, 25)) |
        rv::transform(aoc::string_to_int64) | r::to<std::vector<int64_t>>();

    std::println("--- Day 25: {} ---", title);
    std::println("  part 1: {}", find_encryption_key( input[0] , input[1] ));
    std::println("  part 2: {}", "<xmas freebie>");
}