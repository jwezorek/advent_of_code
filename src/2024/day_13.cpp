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

    using vec2 = aoc::vec2<int64_t>;

    struct claw_game {
        vec2 button_a;
        vec2 button_b;
        vec2 prize;
    };

    claw_game parse_claw_game(const std::vector<std::string>& inp) {
        auto vals = inp | rv::transform(
                [](auto&& str) {
                    return aoc::extract_numbers_int64(str);
                }
            ) | r::to<std::vector>();
        return {
            {vals[0][0], vals[0][1]},
            {vals[1][0], vals[1][1]},
            {vals[2][0], vals[2][1]}
        };
    }

    std::optional<int64_t> cost_in_tokens(const claw_game& cg) {
        auto ax = cg.button_a.x;
        auto ay = cg.button_a.y;
        auto bx = cg.button_b.x;
        auto by = cg.button_b.y;
        auto px = cg.prize.x;
        auto py = cg.prize.y;

        auto a_numer = (by * px - bx * py);
        auto a_denom = (ay * bx - ax * by);
        auto b_numer = (-(ay * px) + ax * py);
        auto b_denom = (ay * bx - ax * by);

        if (a_numer % a_denom != 0 || b_numer % b_denom != 0) {
            return {};
        }

        auto a = -(a_numer / a_denom);
        auto b = -(b_numer / b_denom);

        return 3 * a + b;
    }

    int64_t cost_for_all_wins(const std::vector<claw_game>& games) {
        int64_t fewest_tokens = 0;
        for (const auto& game : games) {
            auto toks = cost_in_tokens(game);
            if (toks) {
                fewest_tokens += *toks;
            }
        }
        return fewest_tokens;
    }

    std::vector<claw_game> make_part2_input(const std::vector<claw_game> games) {
        return games | rv::transform(
            [](auto&& cg)->claw_game {
                auto new_cg = cg;
                new_cg.prize = new_cg.prize + vec2{ 10000000000000, 10000000000000 };
                return new_cg;
            }
        ) | r::to<std::vector>();
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
    std::println("  part 1: {}", cost_for_all_wins(inp) );
    std::println("  part 2: {}", cost_for_all_wins(make_part2_input(inp)) );
    
}
