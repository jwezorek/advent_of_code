#include "days.h"
#include "../util.h"
#include <print>
#include <ranges>
#include <numeric>
#include <algorithm>
#include <unordered_set>
#include <regex>

namespace r = std::ranges;
namespace rv = std::ranges::views;

namespace {
    struct card {
        int id;
        std::unordered_set<int> winning_numbers;
        std::vector<int> numbers_owned;
    };

    card parse_card(const std::string& line) {
        auto halves = aoc::split(line, '|');
        auto id_and_winners = aoc::split(halves.front(), ':');
        return {
            std::stoi(aoc::remove_nonnumeric(id_and_winners.front())),
            aoc::extract_numbers(id_and_winners.back()) | r::to<std::unordered_set<int>>(),
            aoc::extract_numbers(halves.back())
        };
    }

    int count_wins(const card& card) {
        return r::fold_left(
            card.numbers_owned | rv::transform(
                [&](int num) {
                    return card.winning_numbers.contains(num) ? 1 : 0;
                }
            ), 0, std::plus<>()
        );
    }

    int score_card(const card& card) {
        return 1 << count_wins(card) - 1;
    }
    
    int do_part_1(const std::vector<card>& cards) {
        return r::fold_left(
            cards | rv::transform(score_card),
            0, 
            std::plus<>()
        );
    }

    int do_part_2(const std::vector<card>& cards) {
        std::vector<int> counts(cards.size(), 1);
        for (int i = 0; i < static_cast<int>(cards.size()); ++i) {
            int duplicates = counts[i];
            int wins = count_wins(cards[i]);
            for (int j = i + 1; j < i + 1 + wins; ++j) {
                counts[j] += duplicates;
            }
        }
        return r::fold_left(counts, 0, std::plus<>());
    }
}

/*------------------------------------------------------------------------------------------------*/

void aoc::y2023::day_04(const std::string& title) {

    auto cards = aoc::file_to_string_vector(aoc::input_path(2023, 4)) |
        rv::transform(parse_card) | r::to<std::vector<card>>();

    std::println("--- Day 4: {0} ---\n", title);

    std::println("  part 1: {}", do_part_1(cards));
    std::println("  part 2: {}", do_part_2(cards));
}