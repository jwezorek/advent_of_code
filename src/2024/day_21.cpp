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

    point key_to_loc(char key, keyboard_layout layout) {
        static std::unordered_map<char, point> key_to_loc_numeric = {
            { '7' , {0,0} } , { '8' , {1,0} } ,{ '9' , {2,0} },
            { '4' , {0,1} } , { '5' , {1,1} } ,{ '6' , {2,1} },
            { '1' , {0,2} } , { '2' , {1,2} } ,{ '3' , {2,2} },
            { '0' , {1,3} } , { 'A' , {2,3} }
        };
        static std::unordered_map<char, point> key_to_loc_directional = {
            {'^',{1,0}}, {'A',{2,0}},
            {'<',{0,1}}, {'v',{1,1}}, {'>', {2,1}}
        };
        const auto& keyboard = (layout == numeric) ? key_to_loc_numeric : key_to_loc_directional;
        return keyboard.at(key);
    }

    template <typename T> int sgn(T val) {
        return (T(0) < val) - (val < T(0));
    }

    auto inclusive_range(int from, int to) {
        auto delta = sgn(to - from);
        return rv::iota(
            0, std::abs(to - from) + 1
        ) | rv::transform(
            [delta, from](int i) {
                return i * delta + from;
            }
        );
    }

    point directional_key_to_delta(char key) {
        static const std::unordered_map<char, point> delta_tbl = {
            {'^' , { 0,-1}},
            {'>' , { 1, 0}},
            { 'v', { 0, 1}},
            {'<' , {-1, 0}}
        };
        return delta_tbl.at(key);
    }

    bool can_move(const point& loc, const std::string& seq, keyboard_layout kbd) {
        auto pt = loc;
        for (char key : seq) {
            pt = pt + directional_key_to_delta(key);
            if (!loc_to_key(pt, kbd)) {
                return false;
            }
        }
        return true;
    }

    std::string shortest_key_seq(char src_key, char dst_key, keyboard_layout kbd) {

        static const auto num_index = [](char c) {
            if (c == 'A') {
                return 10;
            } else {
                return c - '0';
            }
        };

        static const auto dir_index = [] (char c) {
            switch (c) {
                case '^': return 0;
                case 'A': return 1;
                case '<': return 2;
                case 'v': return 3;
                case '>': return 4;
            };
        };

        if (kbd == numeric) {
            static const std::string numeric_tbl[11][11] = {
                {"A", "^<A", "^A", ">^A", "^^<A", "^^A", ">^^A", "^^^<A", "^^^A", ">^^^A", ">A"},
                {">vA", "A", ">A", ">>A", "^A", "^>A", "^>>A", "^^A", "^^>A", "^^>>A", ">>vA"},
                {"<vA", "<A", "A", ">A", "<^A", "^A", "^>A", "<^^A", "^^A", "^^>A", "v>A"},
                {"<vA", "<<A", "<A", "A", "<<^A", "<^A", "^A", "<<^^A", "<^^A", "^^A", "vA"},
                {">vvA", "vA", "v>A", "v>>A", "A", ">A", ">>A", "^A", "^>A", "^>>A", ">>vvA"},
                {"vvA", "<vA", "vA", "v>A", "<A", "A", ">A", "<^A", "^A", "^>A", "vv>A"},
                {"<vvA", "<<vA", "<vA", "vA", "<<A", "<A", "A", "<<^A", "<A", "^A", "vvA"},
                {">vvvA", "vvA", "vv>A", "vv>>A", "vA", "v>A", "v>>A", "A", ">A", ">>A", ">>vvvA"},
                {"vvvA", "<vvA", "vvA", "vv>A", "<vA", "vA", "v>A", "<A", "A", ">A", "vvv>A"},
                {"<vvvA", "<<vvA", "<vvA", "vvA", "<<vA", "<vA", "vA", "<<A", "<A", "A", "vvvA"},
                {"<A", "^<<A", "<^A", "^A", "^^<<A", "<^^A", "^^A", "^^^<<A", "<^^^A", "^^^A", "A"},
            };
            return numeric_tbl[num_index(src_key)][num_index(dst_key)];
        } else {
            static const std::string directional_tbl[5][5] = {
                {"A", ">A", "v<A", "vA", "v>A"},
                {"<A", "A", "v<<A", "<vA", "vA"},
                {">^A", ">>^A", "A", ">A", ">>A"},
                {"^A", "^>A", "<A", "A", ">A"},
                {"<^A", "^A", "<<A", "<A", "A"},
            };
            return directional_tbl[dir_index(src_key)][dir_index(dst_key)];
        }
    }

    using atom_counts = std::unordered_map<std::string, int64_t>;

    atom_counts directional_kbd_seq(const std::string& seq) {
        atom_counts counts;
        char key = 'A';
        for (auto next_key : seq) {
            ++counts[shortest_key_seq(key, next_key, directional)];
            key = next_key;
        }
        return counts;
    }

    atom_counts numeric_kbd_seq(int num_directional_kbds, const std::string& code) {
        if (num_directional_kbds == 1) {
            atom_counts counts;
            char key = 'A';
            for (auto next_key : code) {
                ++counts[shortest_key_seq(key, next_key, numeric)];
                key = next_key;
            }
            return counts;
        }
        atom_counts output;
        auto n_minus_1_counts = numeric_kbd_seq(num_directional_kbds - 1, code);
        for (const auto& [n_minus_1_atom, n_minus_1_count] : n_minus_1_counts) {
            auto next_level_counts = directional_kbd_seq(n_minus_1_atom);
            for (const auto [next_level_atom, next_level_count] : next_level_counts) {
                output[next_level_atom] += next_level_count * n_minus_1_count;
            }
        }
        return output;
    }

    int64_t fewest_keypresses(int num_directional_kbds, const std::string& code) {
        auto counts = numeric_kbd_seq(num_directional_kbds, code);
        return r::fold_left(
            counts | rv::transform(
                [](auto&& pair)->int64_t {
                    auto [atom, count] = pair;
                    return atom.size() * count;
                }
            ),
            0ll,
            std::plus<int64_t>()
        );
    }

    int64_t complexity(int num_directional_kbds, const std::string& code) {
        int64_t value = aoc::extract_numbers(code).front();
        return value * fewest_keypresses(num_directional_kbds, code);
    }

    int64_t sum_of_complexities(int num_directional_kbds, const std::vector<std::string>& codes) {

        return r::fold_left(
            codes | rv::transform(
                [&](auto&& code) {
                    return complexity(num_directional_kbds, code);
                }
            ),
            0ll,
            std::plus<int64_t>()
        );
    }
}

void aoc::y2024::day_21(const std::string& title) {

    auto inp = aoc::file_to_string_vector(
        aoc::input_path(2024, 21)
    );

    std::println("--- Day 21: {} ---", title);
    std::println("  part 1: {}", sum_of_complexities(3, inp));
    std::println("  part 2: {}", sum_of_complexities(26, inp));

}