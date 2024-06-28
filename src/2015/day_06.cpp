#include "../util.h"
#include "y2015.h"
#include <filesystem>
#include <functional>
#include <print>
#include <ranges>
#include <unordered_set>

namespace r = std::ranges;
namespace rv = std::ranges::views;

/*------------------------------------------------------------------------------------------------*/

namespace {
    enum command {
        turn_on = 0,
        toggle = 1,
        turn_off = 2
    };

    struct instruction {
        command cmd;
        int x1;
        int y1;
        int x2;
        int y2;
    };

    command extract_command(const std::string& str) {
        static const std::array<std::tuple<command, std::string>, 3> cmds = {{
            {turn_on, "on"}, {turn_off, "off"}, {toggle, "toggle"}
        }};
        for (auto [cmd, cmd_str] : cmds) {
            if (str.contains(cmd_str)) {
                return cmd;
            }
        }
        throw std::runtime_error("???");
    }

    instruction str_to_instr(const std::string& str) {
        auto nums = aoc::extract_numbers(str, false);
        return {
            extract_command(str),
            nums[0],nums[1],nums[2],nums[3]
        };
    }
    using grid = std::vector<std::vector<int>>;
    grid make_grid() {
        return std::vector<std::vector<int>>( 1000, std::vector<int>(1000, 0));
    }

    void do_turn_on(grid& g, int x1, int y1, int x2, int y2, bool is_part_1) {
        if (is_part_1) {
            for (int y = y1; y <= y2; ++y) {
                for (int x = x1; x <= x2; ++x) {
                    g[y][x] = 1;
                }
            }
        } else {
            for (int y = y1; y <= y2; ++y) {
                for (int x = x1; x <= x2; ++x) {
                    g[y][x]++;
                }
            }
        }
    }

    void do_turn_off(grid& g, int x1, int y1, int x2, int y2, bool is_part_1) {
        if (is_part_1) {
            for (int y = y1; y <= y2; ++y) {
                for (int x = x1; x <= x2; ++x) {
                    g[y][x] = 0;
                }
            }
        } else {
            for (int y = y1; y <= y2; ++y) {
                for (int x = x1; x <= x2; ++x) {
                    g[y][x]--;

                    if (g[y][x] < 0) {
                        g[y][x] = 0;
                    }
                }
            }
        }
    }

    void do_toggle(grid& g, int x1, int y1, int x2, int y2, bool is_part_1) {
        if (is_part_1) {
            for (int y = y1; y <= y2; ++y) {
                for (int x = x1; x <= x2; ++x) {
                    g[y][x] = (!g[y][x]) ? 1 : 0;
                }
            }
        } else {
            for (int y = y1; y <= y2; ++y) {
                for (int x = x1; x <= x2; ++x) {
                    g[y][x] += 2;
                }
            }
        }
    }

    void do_instruction(grid& g, const instruction& instr, bool is_part_1) {
        static const std::array<std::function<void(grid&, int, int, int, int, bool)>, 3> funcs = { {
                do_turn_on, do_toggle, do_turn_off
        } };
        funcs[static_cast<int>(instr.cmd)](g, instr.x1, instr.y1, instr.x2, instr.y2, is_part_1);
    }

    int do_instructions(const std::vector<instruction>& instructions, bool part_1) {
        auto grid = make_grid();
        for (const auto& instr : instructions) {
            do_instruction(grid, instr, part_1);
        }
        return r::fold_left( grid | rv::join, 0, std::plus<>() );
    }
}

void aoc::y2015::day_06(const std::string& title) {

    auto inp = aoc::file_to_string_vector(
            aoc::input_path(2015, 6)
        ) | rv::transform(
            str_to_instr
        ) | r::to<std::vector>();

    std::println("--- Day 6: {} ---", title);
    std::println("  part 1: {}",
        do_instructions(inp, true)
    );
    std::println("  part 2: {}",
        do_instructions(inp, false)
    );
}