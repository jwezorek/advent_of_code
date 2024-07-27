
#include "../util/util.h"
#include "y2018.h"
#include <filesystem>
#include <functional>
#include <print>
#include <ranges>
#include <set>
#include <unordered_set>

namespace r = std::ranges;
namespace rv = std::ranges::views;

/*------------------------------------------------------------------------------------------------*/

namespace {
    using rules = std::array<bool, 32>;
    using cell_set = std::set<int64_t>;

    cell_set parse_initial_set_str(const std::string& str) {
        auto items = rv::enumerate(str | rv::drop(15)) |
            rv::filter(
                [](auto&& pair) {
                    auto [index, ch] = pair;
                    return ch == '#';
                }
            ) | rv::keys;
        cell_set set;
        for (auto cell : items) {
            set.insert(cell);
        }
        return set;
    }

    uint8_t neighborhood_str_to_key( std::string str) {
        r::reverse(str);
        int bit = 1;
        uint8_t key = 0;
        for (int i = 0; i < 5; ++i) {
            if (str[i] == '#') {
                key = key | (1 << i);
            }
        }
        return key;
    }

    cell_set key_to_neighborhood( uint8_t key ) {
        cell_set neighborhood;
        int cell = 2;
        for (int i = 0; i < 5; ++i) {
            if ((1 << i) & key) {
                neighborhood.insert(2 - i);
            }
        }
        return neighborhood;
    }

    rules parse_rules_strs(const std::vector<std::string>& inp) {
        rules rule_set;
        r::fill(rule_set, 0);

        for (const auto& line : inp) {
            auto parts = aoc::split(line, ' ');
            auto key = neighborhood_str_to_key(parts.front());
            bool val = parts.back() == "#";
            rule_set[key] = val;
        }

        return rule_set;
    }

    std::tuple<cell_set, rules> parse_input(const std::vector<std::string>& inp) {
        auto initial_set_str = inp.front();
        auto rules_strs = inp | rv::drop(2) | r::to<std::vector>();
        return {
            parse_initial_set_str(initial_set_str),
            parse_rules_strs(rules_strs)
        };
    }

    uint8_t rule_key_at_loc(int64_t loc, const cell_set& state) {
        uint8_t key = 0;
        uint8_t bit = 1 << 4;
        for (int64_t i = loc - 2; i <= loc + 2; i++) {
            if (state.contains(i)) {
                key = key | bit;
            }
            bit = bit >> 1;
        }
        return key;
    }

    cell_set perform_one_generation(const cell_set& state, const rules& rules) {
        cell_set next;
        std::unordered_set<int64_t> tested;
        for (auto cell : state) {
            for (auto i = cell - 2; i <= cell + 2; ++i) {
                if (tested.contains(i)) {
                    continue;
                }
                tested.insert(i);
                auto key = rule_key_at_loc(i, state);
                if (rules[key]) {
                    next.insert(i);
                }
            }
        }
        return next;
    }

    int64_t sum_after_n_generations(const cell_set& init_state, const rules& rules, int64_t n) {
        auto state = init_state;
        for (int64_t i = 0; i < n; ++i) {
            state = perform_one_generation(state, rules);
        }
        return r::fold_left(state, 0, std::plus<int64_t>());
    }

    int64_t do_part_2(const cell_set& init_state, const rules& rules) {
        auto state = init_state;
        
        int64_t sum = 0;
        int64_t prev_1 = 0;
        int64_t prev_2 = 0;
        int64_t when = 0;
        int64_t delta = 0;
        int64_t preamble = 0;

        for (int64_t i = 0; i < 10000; ++i) {
            state = perform_one_generation(state, rules);
            prev_2 = prev_1;
            prev_1 = sum;
            sum = r::fold_left(state, 0, std::plus<int64_t>());

            if (sum - prev_1 == prev_1 - prev_2) {
                when = i - 1;
                delta = sum - prev_1;
                preamble = prev_2;
                break;
            }
        }

        int64_t n = 50000000000;
        return  preamble + (n - when) * delta;
    }
}

void aoc::y2018::day_12(const std::string& title) {

    auto [init_state, rules] = parse_input(
        aoc::file_to_string_vector(
            aoc::input_path(2018, 12)
        )
    );

    std::println("--- Day 12: {} ---", title);
    std::println("  part 1: {}", sum_after_n_generations(init_state, rules, 20));
    std::println("  part 2: {}", do_part_2(init_state, rules));
    
}
