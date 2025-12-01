
#include "../util/util.h"
#include "y2025.h"
#include <filesystem>
#include <functional>
#include <print>
#include <ranges>
#include <unordered_set>

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

        if (part_1) {
            auto new_val = wrap(curr.val + rot, mod);
            return {
                new_val,
                (new_val == 0) ? curr.count + 1 : curr.count
            };
        }

        // just brute force this by doing n part_1 style steps of a single unit
        // because I couldnt figure out the math here...
        auto sign = rot < 0 ? -1 : 1;
        state s = curr;
        for (int i = 0; i < std::abs(rot); i++) {
            s = apply_rotation(s, sign, mod, true);
        }

        return s;
    }
}

void aoc::y2025::day_01(const std::string& title) {

    auto inp = parse_input(
        aoc::file_to_string_vector(
            aoc::input_path(2025, 1)
        )
    );

    std::println("--- Day 1: {} ---", title);
    std::println("  part 1: {}",
        r::fold_left(
            inp,
            state{ 50, 0 },
            [](const state& s, int rot)->state {
                return apply_rotation(s, rot, 100, true);
            }
        ).count
    );

    std::println("  part 2: {}",
        r::fold_left(
            inp,
            state{ 50, 0 },
            [](const state& s, int rot)->state {
                return apply_rotation(s, rot, 100, false);
            }
        ).count
    );

}
