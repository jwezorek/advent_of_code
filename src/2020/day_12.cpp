#include "../util/util.h"
#include "y2020.h"
#include <filesystem>
#include <functional>
#include <print>
#include <ranges>
#include <unordered_map>

namespace r = std::ranges;
namespace rv = std::ranges::views;

/*------------------------------------------------------------------------------------------------*/

namespace {
    struct vec2 {
        int x;
        int y;
    };

    vec2 operator+(const vec2& lhs, const vec2& rhs) {
        return {
            lhs.x + rhs.x,
            lhs.y + rhs.y
        };
    }

    vec2 operator*(int lhs, const vec2& rhs) {
        return {
            lhs * rhs.x,
            lhs * rhs.y
        };
    }

    bool operator==(const vec2& lhs, const vec2 rhs) {
        return lhs.x == rhs.x && lhs.y == rhs.y;
    }

    enum direction {
        north = 0, east, south, west
    };

    enum class instr_dir {
        north = 0, east, south, west, left, right, forward
    };

    vec2 direction_delta(direction dir) {
        static const std::array<vec2, 4> deltas = {
            vec2{0,1},vec2{1,0},vec2{0,-1},vec2{-1,0}
        };
        return deltas.at(dir);
    }

    int wrap(int n, int max) {
        return ((n % max) + max) % max;
    }

    direction change_direction(direction dir, bool right, int degrees) {
        int index = degrees / 90;
        if (!right) {
            index *= -1;
        }
        return static_cast<direction>(wrap(dir + index, 4));
    }

    struct pos_dir {
        vec2 pos;
        direction dir;
    };

    struct nav_instruction {
        instr_dir dir;
        int amnt;
    };

    struct ship_and_wp {
        vec2 ship;
        vec2 waypoint;
    };

    nav_instruction parse_nav_instruction(const std::string& str) {
        static const std::unordered_map<char, instr_dir> char_to_dir = {
            {'N', instr_dir::north},
            {'E', instr_dir::east},
            {'S', instr_dir::south},
            {'W', instr_dir::west},
            {'L', instr_dir::left},
            {'R', instr_dir::right},
            {'F', instr_dir::forward}
        };
        return {
            char_to_dir.at(str.front()),
            std::stoi(str.substr(1, str.size() - 1))
        };
    }

    pos_dir do_nav_instruction(const pos_dir& pd, const nav_instruction& instr) {
        if (instr.dir >= instr_dir::north && instr.dir <= instr_dir::west) {
            return { pd.pos + instr.amnt * direction_delta(static_cast<direction>(instr.dir)), pd.dir };
        } 
        if (instr.dir == instr_dir::forward) {
            return { pd.pos + instr.amnt * direction_delta(pd.dir), pd.dir };
        }
        return { pd.pos, change_direction(pd.dir, instr.dir == instr_dir::right, instr.amnt) };
    }

    int follow_nav_instructions(const std::vector<nav_instruction>& instructions) {
        pos_dir ship = { {0,0}, east };
        for (const auto& instruction : instructions) {
            ship = do_nav_instruction(ship, instruction);
        }
        return std::abs(ship.pos.x) + std::abs(ship.pos.y);
    }

    vec2 rotate(vec2 pos, bool right, int amnt) {
        amnt /= 90;
        auto rotate_right = [](const vec2& p)->vec2 {
            return { p.y, -p.x };
        };
        auto rotate_left = [](const vec2& p)->vec2 {
            return { -p.y, p.x };
            };
        for (int i = 0; i < amnt; ++i) {
            pos = (right) ? rotate_right(pos) : rotate_left(pos);
        }
        return pos;
    }

    ship_and_wp do_nav_instruction(const ship_and_wp& sw, const nav_instruction& instr) {
        if (instr.dir >= instr_dir::north && instr.dir <= instr_dir::west) {
            return {
                sw.ship,
                sw.waypoint + instr.amnt * direction_delta(static_cast<direction>(instr.dir))
            };
        }
        if (instr.dir == instr_dir::forward) {
            return {
                sw.ship + instr.amnt * sw.waypoint,
                sw.waypoint
            };
        }
        return {
            sw.ship,
            rotate(sw.waypoint, instr.dir == instr_dir::right, instr.amnt)
        };
    }

    int follow_waypoint_nav_instructions(const std::vector<nav_instruction>& instructions) {
        ship_and_wp state{ {0,0},{10,1} };
        for (const auto& instruction : instructions) {
            state = do_nav_instruction(state, instruction);
        }
        return std::abs(state.ship.x) + std::abs(state.ship.y);
    }
}

void aoc::y2020::day_12(const std::string& title) {
    auto input = aoc::file_to_string_vector(aoc::input_path(2020, 12)) |
        rv::transform(parse_nav_instruction) | r::to<std::vector<nav_instruction>>();

    std::println("--- Day 12: {} ---", title);
    std::println("  part 1: {}", follow_nav_instructions(input));
    std::println("  part 2: {}", follow_waypoint_nav_instructions(input));
}