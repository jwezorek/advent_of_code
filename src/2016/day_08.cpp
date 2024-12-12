
#include "../util/util.h"
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
    
    enum op {
        rect,
        rot_row,
        rot_column
    };

    struct command {
        op op;
        int arg1;
        int arg2;
    };

    op op_from_str(const std::string& str) {
        static const std::array<std::pair<op, std::string>, 3> ary = {{
            {rect, "rect"},
            {rot_row, "row"},
            {rot_column, "column"},
        }};
        for (auto [op, op_str] : ary) {
            if (str.contains(op_str)) {
                return op;
            }
        }
        throw std::runtime_error("bad command");
    }
    
    command parse_command(const std::string& str) {
        auto op = op_from_str(str);
        auto args = aoc::extract_numbers(str);
        return { op, args[0], args[1] };
    }

    using screen = std::vector<std::string>;

    auto column_ptrs(auto& scr, int col) {
        return scr | rv::transform(
            [=](auto&& row) {
                return &(row[col]);
            }
        );
    }

    std::string column(const screen& scr, int col) {
        return column_ptrs(scr, col) | rv::transform(
            [](auto ptr) {
                return *ptr;
            }
        ) | r::to<std::string>();
    }

    void set_column(screen& scr, int col, const std::string& str) {
        for (auto [ptr, ch] : rv::zip(column_ptrs(scr, col), str)) {
            *ptr = ch;
        }
    }

    screen make_screen(int cols, int rows) {
        return std::vector<std::string>(
            rows,
            std::string(cols, '.')
        );
    }

    void draw_rect(screen& scr, int wd, int hgt) {
        for (auto [x, y] : rv::cartesian_product(rv::iota(0, wd), rv::iota(0, hgt))) {
            scr[y][x] = '#';
        }
    }

    std::string rotate(const std::string& str, int n) {
        auto front = str.substr(0, str.size() - n);
        auto back = str.substr(str.size() - n, n);
        return back + front;
    }

    void rotate_row(screen& scr, int row, int amnt) {
        scr[row] = rotate(scr[row], amnt);
    }

    void rotate_col(screen& scr, int col, int amnt) {
        auto col_str = column(scr, col);
        col_str = rotate(col_str, amnt);
        set_column(scr, col, col_str);
    }

    void do_command(screen& scr, const command& cmd) {
        using command_fn = std::function<void(screen&, int, int)>;
        static const std::unordered_map<op, command_fn> tbl = {
            {rect, draw_rect},
            {rot_row, rotate_row},
            {rot_column, rotate_col}
        };
        tbl.at(cmd.op)(scr, cmd.arg1, cmd.arg2);
    }

    int do_part_1(const std::vector<command>& cmds, int wd, int hgt) {
        auto screen = make_screen(wd, hgt);
        for (const auto& cmd : cmds) {
            do_command(screen, cmd);
        }

        return r::fold_left(
            screen | rv::transform(
                [](auto&& row) {
                    return r::count_if(row, [](char ch) {return ch == '#'; });
                }
            ),
            0,
            std::plus<>()
        );
    }

    void do_part_2(const std::vector<command>& cmds, int wd, int hgt) {
        auto screen = make_screen(wd, hgt);
        for (const auto& cmd : cmds) {
            do_command(screen, cmd);
        }
        for (const auto& row : screen) {
            std::println("{}", row);
        }
    }
}

void aoc::y2016::day_08(const std::string& title) {

    auto inp = aoc::file_to_string_vector(
            aoc::input_path(2016, 8)
        ) | rv::transform(
            parse_command
        ) | r::to<std::vector>();

    std::println("--- Day 8: {} ---", title);
    std::println("  part 1: {}", do_part_1(inp, 50, 6));
    std::println("  part 2:");
    do_part_2(inp, 50, 6);
    
}
