
#include "../util/util.h"
#include "../util/vec2.h"
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

    using vec2 = aoc::vec2<int>;

    struct claw_game {
        vec2 button_a;
        vec2 button_b;
        vec2 prize;
    };

    claw_game parse_claw_game(const std::vector<std::string>& inp) {
        auto vals = inp | rv::transform(
                [](auto&& str) {
                    return aoc::extract_numbers(str);
                }
            ) | r::to<std::vector>();
        return {
            {vals[0][0], vals[0][1]},
            {vals[1][0], vals[1][1]},
            {vals[2][0], vals[2][1]}
        };
    }

    std::optional<int> fewest_tokens_to_win(const claw_game& cg) {
        std::optional<int> fewest_tokens = {};
        for (auto [a_clicks, b_clicks] : rv::cartesian_product(rv::iota(0, 100), rv::iota(0, 100))) {
            auto loc = a_clicks * cg.button_a + b_clicks * cg.button_b;
            if (loc == cg.prize) {
                int tokens = 3 * a_clicks + b_clicks;
                if (!fewest_tokens || tokens < *fewest_tokens) {
                    fewest_tokens = tokens;
                }
            }
        }
        return fewest_tokens;
    }

    int do_part_1(const std::vector<claw_game>& games) {
        int fewest_tokens = 0;
        for (const auto& game : games) {
            auto toks = fewest_tokens_to_win(game);
            if (toks) {
                fewest_tokens += *toks;
            }
        }
        return fewest_tokens;
    }
}

void aoc::y2024::day_13(const std::string& title) {

    auto inp = aoc::group_strings_separated_by_blank_lines(
            aoc::file_to_string_vector(
                aoc::input_path(2024, 13)
            )
        ) | rv::transform(
            parse_claw_game
        ) | r::to<std::vector>();
 
    std::println("--- Day 13: {} ---", title);
    std::println("  part 1: {}", do_part_1(inp) );
    std::println("  part 2: {}", 0);
    
}
