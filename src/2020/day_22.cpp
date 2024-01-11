#include "../util.h"
#include "y2020.h"
#include <filesystem>
#include <functional>
#include <print>
#include <ranges>
#include <unordered_set>
#include <deque>
#include <boost/functional/hash.hpp>

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

    deck play_combat(const deck& deck_1, const deck& deck_2) {
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

    bool operator==(const deck& lhs, const deck& rhs) {
        for (auto [l, r] : rv::zip(lhs, rhs)) {
            if (l != r) {
                return false;
            }
        }
        return true;
    }

    struct game_state {
        deck player1;
        deck player2;
    };

    struct game_state_equal {
        bool operator()(const game_state& gs1, const game_state& gs2) const {
            return gs1.player1 == gs2.player1 && gs1.player2 == gs2.player2;
        }
    };

    struct game_state_hash {
        size_t operator()(const game_state& gs) const {
            size_t seed = 0;
            for (auto v : gs.player1) {
                boost::hash_combine(seed, v);
            }
            for (auto v : gs.player2) {
                boost::hash_combine(seed, v);
            }
            return seed;
        }
    };
    
    using game_state_set = std::unordered_set<game_state, game_state_hash, game_state_equal>;

    std::tuple<int,deck> play_recursive_combat_subgame(const deck& deck_1, const deck& deck_2) {
        game_state_set previous_states;
        game_state state = { deck_1, deck_2 };
        while (!state.player1.empty() && !state.player2.empty()) {
            if (previous_states.contains(state)) {
                return { 1, state.player1 };
            }
            previous_states.insert(state);

            int card1 = pop_deck(state.player1);
            int card2 = pop_deck(state.player2);

            std::optional<int> winner_of_subgame;
            if (card1 <= state.player1.size() && card2 <= state.player2.size()) {
                auto [winner, winning_deck] = play_recursive_combat_subgame(
                    state.player1 | rv::take(card1) | r::to<deck>(),
                    state.player2 | rv::take(card2) | r::to<deck>()
                );
                winner_of_subgame = winner;
            }
            int high_card_holder = (card1 > card2) ? 1 : 2;
            int winner_of_round = (winner_of_subgame) ? *winner_of_subgame : high_card_holder;
            if (winner_of_round == 1) {
                state.player1.push_back(card1);
                state.player1.push_back(card2);
            } else {
                state.player2.push_back(card2);
                state.player2.push_back(card1);
            }
        }
        return (state.player1.empty()) ? 
            std::tuple<int, deck>{ 2, state.player2 } : 
            std::tuple<int, deck>{ 1, state.player1 };
    }

    deck play_recursive_combat(const deck& deck_1, const deck& deck_2) {
        return std::get<1>(
            play_recursive_combat_subgame(deck_1, deck_2)
        );
    }
}

void aoc::y2020::day_22(const std::string& title) {
    auto [player_1, player_2] = parse(
        aoc::file_to_string_vector(aoc::input_path(2020, 22))
    );

    std::println("--- Day 22: {} ---", title);
    std::println("  part 1: {}", score_deck(play_combat(player_1, player_2)));
    std::println("  part 2: {}", score_deck(play_recursive_combat(player_1, player_2)));
}