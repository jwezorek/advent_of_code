
#include "../util/util.h"
#include "y2016.h"
#include <filesystem>
#include <functional>
#include <print>
#include <ranges>
#include <unordered_set>
#include <queue>
#include <boost/functional/hash.hpp>

namespace r = std::ranges;
namespace rv = std::ranges::views;

/*------------------------------------------------------------------------------------------------*/

namespace {

    enum item_type {
        generator,
        microchip
    };

    struct item {
        char label;
        item_type type;

        bool operator==(const item& it) const {
            return label == it.label && type == it.type;
        }
    };

    struct hash_item {
        size_t operator()(const item& it) const {
            size_t seed = 0;
            boost::hash_combine(seed, it.label);
            boost::hash_combine(seed, it.type);
            return seed;
        }
    };

    std::string item_to_string(const item& it) {
        std::string str(2,' ');
        str[0] = it.label;
        str[1] = it.type == microchip ? 'M' : 'G';
        return str;
    }

    using item_set = std::unordered_set<item, hash_item>;
    struct state {
        int curr_floor;
        std::vector<item_set> items_on_floor;

        state(const std::vector<std::vector<item>>& items) : 
                curr_floor(0), items_on_floor(4) {
            for (const auto& [floor, floor_items] : rv::enumerate(items)) {
                items_on_floor[floor] = floor_items | r::to<item_set>();
            }
        }

        std::string tokenize() const {

            std::unordered_map<char, char> tbl;
            char canon_char = 'a';
            std::stringstream ss;
            ss << curr_floor;
            for (const auto& itm_set : items_on_floor) {
                auto itms = itm_set | r::to<std::vector>();
                r::sort(itms,
                    [](auto&& lhs, auto&& rhs) {
                        if (lhs.label < rhs.label) {
                            return true;
                        }
                        if (lhs.label > rhs.label) {
                            return false;
                        }
                        return lhs.type < rhs.type;
                    }
                );
                for (auto item : itms) {
                    char lbl;
                    if (!tbl.contains(item.label)) {
                        lbl = canon_char++;
                        tbl[item.label] = lbl;
                    }
                    else {
                        lbl = tbl[item.label];
                    }
                    ss << lbl;
                    ss << ((item.type == generator) ? 'G' : 'M');
                }
                ss << '|';
            }
            return ss.str();
        }
    };

    std::vector<std::vector<item>> parse_input(const std::vector<std::string>& inp) {
        static const std::unordered_set<std::string> irrelevant_words = {
            "The", "first", "floor", "contains", "a", "second", "third",
            "fourth", "nothing", "relevant", "and", "compatible"
        };
        auto items = inp | rv::transform(
                [&](auto&& line) {
                    return aoc::extract_alphabetic(line) | rv::filter(
                        [](auto&& word) {
                            return !irrelevant_words.contains(word);
                        }
                    ) | r::to<std::vector>();
                }
            ) | r::to<std::vector>();

        std::vector<std::vector<item>> floors(4);
        for (const auto& [floor, floor_items] : rv::enumerate(items)) {
            for (auto pair : floor_items | rv::chunk(2)) {
                floors[floor].emplace_back(
                    std::toupper(pair[0].front()),
                    pair[1] == "microchip" ? microchip : generator
                );
            }
        }

        return floors;
    }

    std::vector<state> next_state_candidates(const state& s) {
        std::vector<state> candidates;
        const auto& items = s.items_on_floor[s.curr_floor];
        std::vector<int> destinations = std::array<int, 2>{{
                s.curr_floor + 1, s.curr_floor - 1
            }} | rv::filter(
                [](int floor) {
                    return floor >= 0 && floor < 4;
                }
            ) | r::to<std::vector>();

        auto v = aoc::two_combinations(items | r::to<std::vector>()) | r::to<std::vector>();
        for (auto [dest, itms] : rv::cartesian_product(destinations, v)) {
            std::array<item, 2> carried_pair = { {
                std::get<0>(itms), get<1>(itms)
            } };
            auto new_state = s;
            new_state.curr_floor = dest;
            for (const auto& carried : carried_pair) {
                new_state.items_on_floor[s.curr_floor].erase(carried);
                new_state.items_on_floor[dest].insert(carried);
            }
            candidates.push_back(new_state);
        }

        for (auto [dest, carried] : rv::cartesian_product(destinations, items)) {
            auto new_state = s;
            new_state.curr_floor = dest;
            new_state.items_on_floor[s.curr_floor].erase(carried);
            new_state.items_on_floor[dest].insert(carried);
            candidates.push_back(new_state);
        }

        return candidates;
    }

    bool is_legal_floor(const item_set& floor) {

        auto unpaired_generators = r::count_if(
            floor | rv::filter(
                [](auto&& i) {
                    return i.type == generator;
                }
            ),
            [&](auto&& g) {
                return !floor.contains({ g.label, microchip });
            }
        );

        if (unpaired_generators == 0) {
            return true;
        }

        auto unpaired_microchips = r::count_if(
            floor | rv::filter(
                [](auto&& i) {
                    return i.type == microchip;
                }
            ),
            [&](auto&& g) {
                return !floor.contains({ g.label, generator });
            }
        );

        return unpaired_microchips == 0;
    }

    bool is_legal_state(const state& s) {

        bool good = true;
        for (const auto& floor : s.items_on_floor) {
            if (!is_legal_floor(floor)) {
                good = false;
                break;
            }
        }

        return good;
    }

    std::vector<state> next_states(const state& s) {
        auto candidates = next_state_candidates(s);
        return candidates | rv::filter(
                is_legal_state
            ) | r::to<std::vector>();
    }

    bool is_complete(const state& s) {
        return s.items_on_floor[0].empty() &&
            s.items_on_floor[1].empty() &&
            s.items_on_floor[2].empty();
    }

    int minimum_moves(const state& start) {
        std::unordered_set<std::string> visited;
        std::queue<std::tuple<int, state>> queue;

        queue.emplace(0, start);
        while (!queue.empty()) {
            auto [moves, curr_state] = queue.front();
            queue.pop();

            if (is_complete(curr_state)) {
                return moves;
            }

            auto state_key = curr_state.tokenize();
            if (visited.contains(state_key)) {
                continue;
            }
            visited.insert(state_key);

            auto next = next_states(curr_state);
            for (const auto& s : next) {
                queue.emplace(moves + 1, s);
            }
        }

        return -1;
    }

    state make_part2_input(const state& s) {
        auto part_2 = s;

        part_2.items_on_floor[0].emplace( 'E', microchip );
        part_2.items_on_floor[0].emplace( 'E', generator );
        part_2.items_on_floor[0].emplace( 'D', microchip );
        part_2.items_on_floor[0].emplace( 'D', generator );

        return part_2;
    }

}

void aoc::y2016::day_11(const std::string& title) {

    auto start_state = state(
        parse_input(
            aoc::file_to_string_vector(
                aoc::input_path(2016, 11)
            )
        )
    );

    std::println("--- Day 11: {} ---", title);
    std::println("  part 1: {}", minimum_moves(start_state));
    std::println("  part 2: {}", minimum_moves(make_part2_input(start_state)));
    
}
