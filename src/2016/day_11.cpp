
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

        std::string to_string() const {
            std::stringstream ss;
            for (const auto& [floor, floor_items] : rv::enumerate(items_on_floor)) {
                if (floor == curr_floor) {
                    ss << "[*] : ";
                } else {
                    ss << "[ ] : ";
                }
                auto itms = floor_items | rv::transform(item_to_string) | r::to<std::vector>();
                r::sort(itms);
                ss << (itms | rv::join_with(' ') | r::to<std::string>()) << "\n";
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

    std::vector<state> next_states(const state& s) {
        return {};
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

            auto state_key = curr_state.to_string();
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

}

void aoc::y2016::day_11(const std::string& title) {

    auto start_state = state(
        parse_input(
            aoc::file_to_string_vector(
                aoc::input_path(2016, 11)
            )
        )
    );

    std::println("{}", start_state.to_string());

    std::println("--- Day 11: {} ---", title);
    std::println("  part 1: {}", 0);
    std::println("  part 2: {}", 0);
    
}
