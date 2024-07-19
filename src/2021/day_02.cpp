#include "../util/util.h"
#include "y2021.h"
#include <filesystem>
#include <functional>
#include <print>
#include <ranges>
#include <unordered_map>

namespace r = std::ranges;
namespace rv = std::ranges::views;

/*------------------------------------------------------------------------------------------------*/

namespace {
    struct point {
        int x;
        int y;
    };

    point operator+(const point& lhs, const point& rhs) {
        return { lhs.x + rhs.x, lhs.y + rhs.y };
    }

    point operator*(int lhs, const point& rhs) {
        return { lhs * rhs.x, lhs * rhs.y };
    }

    point to_offset(const std::string& inp) {
        static const std::unordered_map<std::string, point> tbl = {
            {"forward", {1,0}}, {"down", {0,1}}, {"up", {0,-1}}
        };
        const auto& [dir, amnt] = aoc::split_to_tuple<2>(inp, ' ');
        return std::stoi(amnt) * tbl.at(dir);
    }

    int do_part_1(const std::vector<std::string>& input) {
        auto dest = r::fold_left(
            input | rv::transform(to_offset),
            point{ 0,0 },
            std::plus<point>()
        );
        return dest.x * dest.y;
    }

    struct sub_state {
        point loc;
        int aim;
    };

    sub_state operator+(const sub_state& lhs, const sub_state& rhs) {
        return { lhs.loc + rhs.loc , lhs.aim + rhs.aim };
    }
    
    sub_state update_sub_state(const sub_state& ss, const std::string& inp) {
        const auto& [dir, amnt_str] = aoc::split_to_tuple<2>(inp, ' ');
        int amnt = std::stoi(amnt_str);
        std::unordered_map<std::string, sub_state> tbl = {
            {"forward", {{amnt, ss.aim * amnt}, 0}}, 
            {"down", {{0, 0}, amnt}}, 
            {"up", {{0, 0}, -amnt}}
        };
        return ss + tbl.at(dir);
    }

    int do_part_2(const std::vector<std::string>& input) {
        auto dest = r::fold_left(
            input,
            sub_state{ { 0,0 },0 },
            update_sub_state
        );
        return dest.loc.x * dest.loc.y;
    }
}

void aoc::y2021::day_02(const std::string& title) {
    auto input = aoc::file_to_string_vector(aoc::input_path(2021, 2));

    std::println("--- Day 2: {} ---", title);
    std::println("  part 1: {}", do_part_1(input) );
    std::println("  part 2: {}", do_part_2(input));
}