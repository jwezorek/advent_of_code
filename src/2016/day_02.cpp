
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

    int find_one_digit(const std::string& instructions, int start) {
        const static std::array<std::array<int, 3>, 3> loc_to_btn = { {
            {1, 2, 3},
            {4, 5, 6},
            {7, 8, 9}
        } };
        const static std::unordered_map<int, point> btn_to_loc = {
            {1, {0,0}},{2,{1,0}},{3,{2,0}},
            {4, {0,1}},{5,{1,1}},{6,{2,1}},
            {7, {0,2}},{8,{1,2}},{9,{2,2}}
        };
        const static std::unordered_map<char, point> delta_tbl = {
            {'U', {0,-1}},
            {'R', {1, 0}},
            {'D', {0, 1}},
            {'L', {-1,0}}
        };
        auto loc = btn_to_loc.at(start);
        for (auto instr : instructions) {
            auto next_loc = loc + delta_tbl.at(instr);
            if (next_loc.x >= 0 && next_loc.y >= 0 && next_loc.x < 3 && next_loc.y < 3) {
                loc = next_loc;
            }
        }
        return loc_to_btn[loc.y][loc.x];
    }

    std::string find_code(const std::vector<std::string>& instructions) {
        std::stringstream ss;
        int btn = 5;
        for (const auto& digit_instrs : instructions) {
            btn = find_one_digit(digit_instrs, btn);
            ss << btn;
        }
        return ss.str();
    }
}

void aoc::y2016::day_02(const std::string& title) {

    auto inp = aoc::file_to_string_vector(
            aoc::input_path(2016, 2)
        ); 

    std::println("--- Day 2: {} ---", title);
    std::println("  part 1: {}", find_code(inp) );
    std::println("  part 2: {}", 0);
    
}
