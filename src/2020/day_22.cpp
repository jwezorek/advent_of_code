#include "../util.h"
#include "y2020.h"
#include <filesystem>
#include <functional>
#include <print>
#include <ranges>
#include <unordered_map>
#include <deque>

namespace r = std::ranges;
namespace rv = std::ranges::views;

/*------------------------------------------------------------------------------------------------*/

namespace {
    using deck = std::deque<int>;
    std::tuple<deck, deck> parse(const std::vector<std::string>& inp) {
        auto groups = aoc::group_strings_separated_by_blank_lines(inp);
        auto to_deck = [](const std::vector<std::string>& strs) {
                return strs | rv::drop(1) | rv::transform(
                    [](auto&& str) {return std::stoi(str); }
                ) | r::to<deck>();
            };
        return { to_deck(groups.front()),to_deck(groups.back()) };
    }

    int pop_deck(deck& deck) {
        auto top = deck.front();
        deck.pop_front();
        return top;
    }

    deck winning_deck(const deck& deck_1, const deck& deck_2) {
        auto player_1 = deck_1;
        auto player_2 = deck_2;
        while (!player_1.empty() && !player_2.empty()) {
            int card_1 = pop_deck(player_1);
            int card_2 = pop_deck(player_2);
            if (card_1 > card_2) {
                player_1.push_back(card_1);
                player_1.push_back(card_2);
            } else {
                player_2.push_back(card_2);
                player_2.push_back(card_1);
            }
        }
        return (player_1.empty()) ? player_2 : player_1;
    }

    int64_t score_deck(const deck& d) {
        int n = static_cast<int>(d.size());
        return r::fold_left(
            rv::zip_transform(
                [n](int card, int i) {
                    return card * (n - i);
                }, d, rv::iota(0, n)
            ), 0, std::plus<int64_t>()
        );
    }
}

void aoc::y2020::day_22(const std::string& title) {
    auto [player_1, player_2] = parse(
        aoc::file_to_string_vector(aoc::input_path(2020, 22))
    );

    std::println("--- Day 22: {} ---", title);
    std::println("  part 1: {}", score_deck(winning_deck(player_1, player_2)) );
    std::println("  part 2: {}", 0);
}