
#include "../util/util.h"
#include "../util/vec2.h"
#include "y2024.h"
#include <filesystem>
#include <functional>
#include <print>
#include <ranges>
#include <unordered_set>
#include <optional>
#include <boost/functional/hash.hpp>
#include <queue>

namespace r = std::ranges;
namespace rv = std::ranges::views;

/*------------------------------------------------------------------------------------------------*/

namespace {
    using point = aoc::vec2<int>;
    using point_set = aoc::vec2_set<int>;

    template<typename T>
    using point_map = aoc::vec2_map<int, T>;

    enum keyboard_layout {
        numeric = 0,
        directional
    };

    std::optional<char> loc_to_key(const point& loc, keyboard_layout layout) {
        point_map<char> numeric_keypad_map = {
            { {0,0} , '7' } , { {1,0} , '8' } ,{ {2,0} , '9' },
            { {0,1} , '4' } , { {1,1} , '5' } ,{ {2,1} , '6' },
            { {0,2} , '1' } , { {1,2} , '2' } ,{ {2,2} , '3' },
            { {1,3} , '0' } , { {2,3} , 'A' }
        };
        point_map<char> directional_keypad_map = {
            {{1,0},'^'}, {{2,0},'A'},
            {{0,1},'<'}, {{1,1},'v'}, {{2,1},'>'}
        };
        const auto& keyboard = (layout == numeric) ? numeric_keypad_map : directional_keypad_map;
        if (!keyboard.contains(loc)) {
            return {};
        }
        return keyboard.at(loc);
    }

    const std::vector<char>& keys(keyboard_layout layout) {
        const static std::vector<std::vector<char>> layout_keys = {
            { '0','1','2','3','4','5','6','7','8','9','A' },
            { '^','<','v','>','A'}
        };
        return layout_keys.at(static_cast<size_t>(layout));
    }

    using keyboards = std::vector<keyboard_layout>;

    struct state {
        std::string typed;
        std::vector<point> kbd_locs;

        bool operator==(const state& s) const {
            if (typed != s.typed) {
                return false;
            }
            for (const auto& [lhs, rhs] : rv::zip(kbd_locs, s.kbd_locs)) {
                if (lhs != rhs) {
                    return false;
                }
            }
            return true;
        }
    };

    struct state_hash {
        size_t operator()(const state& s) const {
            size_t seed = 0;
            boost::hash_combine(seed, s.typed);
            for (const auto& pt : s.kbd_locs) {
                boost::hash_combine(seed, pt.x);
                boost::hash_combine(seed, pt.y);
            }
            return seed;
        }
    };

    using state_set = std::unordered_set<state, state_hash>;

    std::optional<point> key_to_delta(keyboard_layout layout, char key) {
        if (key == 'A') {
            return {};
        }

        static const std::unordered_map<char, point> delta_tbl = {
            {'^' , { 0,-1}},
            {'>' , { 1, 0}},
            { 'v', { 0, 1}},
            {'<' , {-1, 0}}
        };
        return delta_tbl.at(key);
    }
 
    std::optional<state> next_state(const keyboards& layouts, const state& s, char key, int level) {
        if (level == layouts.size() - 1) {
            auto new_state = s;
            new_state.typed = s.typed + std::string{ key };
            return new_state;
        }
        auto delta = key_to_delta(layouts.at(level), key);
        if (delta) {
            auto new_pos = s.kbd_locs[level] + *delta;
            if (loc_to_key(new_pos, layouts.at(level + 1))) {
                auto new_state = s;
                new_state.kbd_locs[level] = s.kbd_locs[level] + *delta;
                return new_state;
            } 
            return {};
        }
        return next_state(layouts, s, *loc_to_key(s.kbd_locs[level], layouts.at(level + 1)), level + 1);
    }

    bool is_prefix(const std::string& typed, const std::string& code) {
        if (typed.size() > code.size()) {
            return false;
        }
        return code.compare(0, typed.size(), typed) == 0;
    }

    std::vector<state> next_states(const keyboards& layouts, const state& s, const std::string& code) {
        std::vector<state> output;
        for (auto key : keys(layouts.front())) {
            auto new_state = next_state(layouts, s, key, 0);
            if (new_state && is_prefix(new_state->typed, code)) {
                output.push_back(*new_state);
            }
        }
        return output;
    }
    
    std::string to_string(const state& s) {
        std::stringstream ss;
        ss << "[ ";
        ss << "\"" << s.typed << "\" : { "; 
        for (auto pt : s.kbd_locs) {
            ss << "( " << pt.x << " , " << pt.y << " ) ";
        }
        ss << "} ]";

        return ss.str();
    }

    int64_t shortest_path(const state& initial_state, const std::vector<keyboard_layout>& keyboards, const std::string& code) {
        state_set visited;
        std::queue<std::tuple<state, int64_t>> queue;
        queue.emplace(initial_state, 0);

        while (!queue.empty()) {
            auto [curr_state, key_punches] = queue.front();
            queue.pop();

            if (visited.contains(curr_state)) {
                continue;
            }
            visited.insert(curr_state);

            if (curr_state.typed == code) {
                return key_punches;
            }

            for (const auto& next : next_states(keyboards, curr_state, code)) {
                queue.emplace(next, key_punches+1);
            }
        }

        return -1;
    }

    int64_t complexity(const state& initial_state, const std::vector<keyboard_layout>& keyboards, const std::string& code) {
        int64_t value = aoc::extract_numbers(code).front();
        return value * shortest_path(initial_state, keyboards, code);
    }

    int64_t sum_of_complexities(
            const state& initial_state, const std::vector<keyboard_layout>& keyboards,
            const std::vector<std::string>& codes) {

        return r::fold_left(
            codes | rv::transform(
                [&](auto&& code) {
                    return complexity(initial_state, keyboards, code);
                }
            ),
            0ll,
            std::plus<int64_t>()
        );
    }

    state make_initial_state(int num_keyboards) {
        auto locs = rv::repeat(
                point{ 2,0 }
            ) | rv::take(
                num_keyboards - 2
            ) | r::to<std::vector>();
        locs.push_back({ 2,3 });
        return {
            {},
            std::move(locs)
        };
    }

    std::vector<keyboard_layout> make_keyboards(int num_keyboards) {
        auto kbds = rv::repeat(
                directional
            ) | rv::take(
                num_keyboards - 1
            ) | r::to<std::vector>();
        kbds.push_back(numeric);
        return kbds;
    }
}

void aoc::y2024::day_21(const std::string& title) {

    auto inp = aoc::file_to_string_vector(
            aoc::input_path(2024, 21)
        );

    std::println("--- Day 21: {} ---", title);
    std::println("  part 1: {}", sum_of_complexities(make_initial_state(4), make_keyboards(4), inp));
    std::println("  part 2: {}", sum_of_complexities(make_initial_state(8), make_keyboards(8), inp));
    
}
