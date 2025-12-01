
#include "../util/util.h"
#include "y2025.h"
#include <filesystem>
#include <functional>
#include <print>
#include <ranges>

namespace r = std::ranges;
namespace rv = std::ranges::views;

/*------------------------------------------------------------------------------------------------*/

namespace {
    struct state {
        int val;
        int count;
    };

    std::vector<int> parse_input(const std::vector<std::string>& inp) {
        return inp | rv::transform(
            [](auto&& str)->int {
                return ((str.front() == 'R') ? 1 : -1) *
                    std::stoi(aoc::remove_nonnumeric(str));
            }
       ) | r::to<std::vector>();
    }

    int wrap(int num, int mod) {
        int wrapped = num % mod;
        return (wrapped < 0) ? wrapped + mod : wrapped;
    }

    state apply_rotation(const state& curr, int rot, int mod, bool part_1) {

        auto new_val = wrap(curr.val + rot, mod);
        if (part_1) {
            return {
                new_val,
                (new_val == 0) ? curr.count + 1 : curr.count
            };
        } else {
            auto rot_remainder = rot % mod;
            auto zero_count = curr.count + std::abs(rot) / mod;
            
            if (rot_remainder != 0 && curr.val != 0) {
                auto unwrapped = curr.val + rot_remainder;
                if (unwrapped >= mod || unwrapped <= 0) {
                    ++zero_count;
                }
            }

            return {
                new_val,
                zero_count
            };
        }
    }

    int count_zeros(const std::vector<int>& rotations, bool part_1) {
        return r::fold_left(
            rotations,
            state{ 50, 0 },
            [part_1](const state& s, int rot)->state {
                return apply_rotation(s, rot, 100, part_1);
            }
        ).count;
    }
}

void aoc::y2025::day_01(const std::string& title) {

    auto inp = parse_input(
        aoc::file_to_string_vector(
            aoc::input_path(2025, 1)
        )
    );

    std::println("--- Day 1: {} ---", title);
    std::println("  part 1: {}", count_zeros(inp, true) );
    std::println("  part 2: {}", count_zeros(inp, false) );

}
