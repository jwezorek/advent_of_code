#include "y2023.h"
#include "../util.h"
#include <print>
#include <ranges>
#include <numeric>
#include <algorithm>
#include <sstream>
#include <unordered_map>
#include <functional>
#include <set>

namespace r = std::ranges;
namespace rv = std::ranges::views;

namespace {

    struct hand_and_bid {
        std::string hand;
        int bid;
    };

    std::vector<hand_and_bid> parse_input(const std::vector<std::string> lines) {
        return lines |
            rv::transform(
                [](const std::string& line)->hand_and_bid {
                    auto parts = aoc::split(line, ' ');
                    return {
                        parts.front(),
                        std::stoi(parts.back())
                    };
                }
        ) | r::to<std::vector<hand_and_bid>>();
    }

    struct hand_stats {
        int pairs;
        int triples;
        int quads;
        int fives;
    };

    hand_stats get_hand_stats(const std::string& hand) {
        std::unordered_map<char, int> counts;
        for (auto card : hand) {
            counts[card]++;
        }
        std::vector<int> stats(6, 0);
        for (auto [card, count] : counts) {
            stats[count]++;
        }
        return {
            stats[2],
            stats[3],
            stats[4],
            stats[5]
        };
    }

    enum class hand_type {
        high_card = 0,
        one_pair,
        two_pair,
        three_of_a_kind,
        full_house,
        four_of_a_kind,
        five_of_a_kind
    };

    int card_strength(char card, bool use_jokers) {
        std::unordered_map<char, int> strengths = {
            {'A',12}, {'K',11}, {'Q',10}, {'J', use_jokers ? -1 : 9}, {'T',8}, {'9',7}, {'8',6}, {'7',5}, {'6',4},
            {'5',3}, {'4',2}, {'3',1}, {'2', 0}
        };
        return strengths.at(card);
    }

    hand_type get_hand_type(const std::string& hand) {
        std::vector<std::tuple<hand_type, std::function<bool(const hand_stats&)>>> hand_typers = {
            { hand_type::five_of_a_kind,
                [](const hand_stats& stats) {
                    return stats.fives == 1;
                }
            } , { hand_type::four_of_a_kind,
                [](const hand_stats& stats) {
                    return stats.quads == 1;
                }
            } , { hand_type::full_house,
                [](const hand_stats& stats) {
                    return stats.triples == 1 && stats.pairs == 1;
                }
            } , { hand_type::three_of_a_kind,
                [](const hand_stats& stats) {
                    return stats.triples == 1;
                }
            } , { hand_type::two_pair,
                [](const hand_stats& stats) {
                    return stats.pairs == 2;
                }
            } , { hand_type::one_pair,
                [](const hand_stats& stats) {
                    return stats.pairs == 1;
                }
            } , { hand_type::high_card,
                [](const hand_stats& stats) {
                    return true;
                }
            }
        };
        return std::get<0>(
            *r::find_if(
                hand_typers,
                [&hand](const auto& tup) {
                    auto [type, fn] = tup;
                    return fn(get_hand_stats(hand));
                }
            )
        );
    }

    std::vector<std::string> combos_with_repitition(const std::string& items, int k)
    {
        if (k == 0) {
            return { "" };
        } else if (!items.empty()) {
            std::vector<std::string> output;
            for (auto combo : combos_with_repitition(items, k - 1)) {
                output.push_back(std::string(1, items[0]) + combo );
            }
            for (auto combo : combos_with_repitition(items | rv::drop(1) | r::to<std::string>(), k)) {
                output.push_back(combo);
            }
            return output;
        }
        return {};
    }

    hand_type get_hand_type_with_jokers(const std::string& hand) {
        if (r::find_if(hand, [](char card) {return card == 'J'; }) == hand.end()) {
            return get_hand_type(hand);
        }

        std::string nonjokers = hand |
            rv::filter([](char card) {return card != 'J'; }) | r::to<std::string>();

        std::string unique_nonjokers_plus_ace = (nonjokers + "A") | 
            r::to<std::set<char>>() | r::to<std::string>();

        int num_jokers = 5 - static_cast<int>(nonjokers.size());

        return r::max(
            combos_with_repitition(unique_nonjokers_plus_ace, num_jokers) |
                rv::transform(
                    [&nonjokers](const std::string& joker_assignment)->hand_type {
                        return get_hand_type(nonjokers + joker_assignment);
                    }
                )
        );
    }

    bool compare_hands(const std::string& lhs, const std::string& rhs, bool use_jokers) {
        auto lhs_type = (use_jokers) ?
            get_hand_type_with_jokers(lhs) :
            get_hand_type(lhs);

        auto rhs_type = (use_jokers) ? 
            get_hand_type_with_jokers(rhs) :
            get_hand_type(rhs);

        if (lhs_type < rhs_type) {
            return true;
        }
        if (lhs_type > rhs_type) {
            return false;
        }
        for (auto [lhs_card, rhs_card] : rv::zip(lhs, rhs)) {
            auto lhs_card_strength = card_strength(lhs_card, use_jokers);
            auto rhs_card_strength = card_strength(rhs_card, use_jokers);
            if (lhs_card_strength < rhs_card_strength) {
                return true;
            }
            if (lhs_card_strength > rhs_card_strength) {
                return false;
            }
        }

        throw std::runtime_error("equal hands");
    }

    std::vector<hand_and_bid> sort_hands(const std::vector<hand_and_bid>& inp, bool use_jokers) {
        auto hands = inp;
        r::sort(hands,
            [use_jokers](const auto& lhs, const auto& rhs)->bool {
                return compare_hands(lhs.hand, rhs.hand, use_jokers);
            }
        );
        return hands;
    }

    int calc_winnings(const std::vector<hand_and_bid>& input, bool use_jokers) {
        auto hands = sort_hands(input, use_jokers);
        return r::fold_left(
            rv::enumerate(hands) | rv::transform(
                [](auto tup) {
                    const auto& [index, hand] = tup;
                    return (index + 1) * hand.bid;
                }
            ), 0, std::plus<>()
        );
    }

}

/*------------------------------------------------------------------------------------------------*/

void aoc::y2023::day_07(const std::string& title) {

    auto inp = parse_input(aoc::file_to_string_vector(aoc::input_path(2023, 7)));

    std::println("--- Day 7: {0} ---\n", title);
    std::println("  part 1: {}", calc_winnings(inp, false));
    std::println("  part 2: {}", calc_winnings(inp, true)); 
}