
#include "../util/util.h"
#include "../util/vec2.h"
#include "y2016.h"
#include <filesystem>
#include <functional>
#include <print>
#include <ranges>
#include <unordered_map>

namespace r = std::ranges;
namespace rv = std::ranges::views;

/*------------------------------------------------------------------------------------------------*/

namespace {

    using point = aoc::vec2<int>;
    using loc_to_btn_tbl = aoc::vec2_map<int, char>;
    using btn_to_loc_tbl = std::unordered_map<char, point>;

    std::tuple<btn_to_loc_tbl, loc_to_btn_tbl> parse_keypad_layout(
            const std::vector<std::string>& layout) {

        int wd = static_cast<int>(layout.front().size());
        int hgt = static_cast<int>(layout.size());
        btn_to_loc_tbl btn_to_loc;
        loc_to_btn_tbl loc_to_btn;

        for (auto [x,y] : rv::cartesian_product(rv::iota(0,wd),rv::iota(0,hgt))) {
            auto btn = layout[y][x];
            if (btn != '.') {
                point pt = { x,y };
                loc_to_btn[pt] = btn;
                btn_to_loc[btn] = pt;
            }
        }

        return {
            std::move(btn_to_loc),
            std::move(loc_to_btn)
        };
    }

    char find_one_digit(const btn_to_loc_tbl& btn_to_loc, const loc_to_btn_tbl& loc_to_btn,
            const std::string& instructions, int start) {

        const static std::unordered_map<char, point> delta_tbl = {
            {'U', {0,-1}},
            {'R', {1, 0}},
            {'D', {0, 1}},
            {'L', {-1,0}}
        };

        auto loc = btn_to_loc.at(start);
        for (auto instr : instructions) {
            auto next_loc = loc + delta_tbl.at(instr);
            if (loc_to_btn.contains(next_loc)) {
                loc = next_loc;
            }
        }

        return loc_to_btn.at(loc);
    }

    std::string find_code(
            const std::vector<std::string>& keypad_layout, 
            const std::vector<std::string>& instructions) {

        auto [btn_to_loc, loc_to_btn] = parse_keypad_layout(keypad_layout);
        std::stringstream ss;
        char btn = '5';

        for (const auto& digit_instrs : instructions) {
            btn = find_one_digit(btn_to_loc, loc_to_btn, digit_instrs, btn);
            ss << btn;
        }

        return ss.str();
    }
}

void aoc::y2016::day_02(const std::string& title) {

    auto inp = aoc::file_to_string_vector(
            aoc::input_path(2016, 2)
        ); 

    std::vector<std::string> part1_keypad = {
        "123",
        "456",
        "789"
    };

    std::vector<std::string> part2_keypad = {
        "..1..",
        ".234.",
        "56789",
        ".ABC.",
        "..D.."
    };

    std::println("--- Day 2: {} ---", title);
    std::println("  part 1: {}", find_code(part1_keypad, inp) );
    std::println("  part 2: {}", find_code(part2_keypad, inp) );
    
}
