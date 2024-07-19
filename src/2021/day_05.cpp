#include "../util/util.h"
#include "y2021.h"
#include <filesystem>
#include <functional>
#include <print>
#include <ranges>
#include <unordered_map>
#include <regex>

namespace r = std::ranges;
namespace rv = std::ranges::views;

/*------------------------------------------------------------------------------------------------*/

namespace {

    std::vector<std::tuple<int, int, int, int>> parse_input(const std::vector<std::string>& line_strings) {
        return line_strings | rv::transform(
            [](const std::string& line_str)->std::tuple<int, int, int, int> {
                auto str = std::regex_replace(line_str, std::regex(" -> "), " ");
                str = std::regex_replace(str, std::regex(","), " ");
                auto coords = aoc::split(str, ' ');
                return {
                    std::stoi(coords[0]),
                    std::stoi(coords[1]),
                    std::stoi(coords[2]),
                    std::stoi(coords[3])
                };
            }
        ) | r::to<std::vector<std::tuple<int, int, int, int>>>();
    }

    std::tuple<int, int> get_dimensions(std::vector<std::tuple<int, int, int, int>> lines) {
        int wd = -1;
        int hgt = -1;
        for (const auto& [x1, y1, x2, y2] : lines) {
            wd = (x1 > wd) ? x1 : wd;
            wd = (x2 > wd) ? x2 : wd;
            hgt = (y1 > hgt) ? y1 : hgt;
            hgt = (y2 > hgt) ? y2 : hgt;
        }
        return { wd + 1, hgt + 1 };
    }

    std::vector<std::vector<int>> init_board(int wd, int hgt) {
        std::vector<std::vector<int>> rows(hgt, std::vector<int>(wd, 0));
        return rows;
    }

    void draw_line(std::vector<std::vector<int>>& g, int x1, int y1, int x2, int y2, bool skip_diagonal = false) {
        if (skip_diagonal) {
            if (x1 != x2 && y1 != y2) {
                return;
            }
        }

        int x_delta = (x2 != x1) ? (x2 - x1) / std::abs(x2 - x1) : 0;
        int y_delta = (y2 != y1) ? (y2 - y1) / std::abs(y2 - y1) : 0;
        int length = std::max(std::abs(x2 - x1), std::abs(y2 - y1));

        int x = x1;
        int y = y1;
        for (int i = 0; i <= length; ++i) {
            g[y][x]++;
            x += x_delta;
            y += y_delta;
        }
    }

    int count_intersections(const std::vector<std::vector<int>>& g) {
        int n = 0;
        for (const auto& row : g) {
            for (int count : row) {
                n += (count > 1) ? 1 : 0;
            }
        }
        return n;
    }

    int draw_lines(std::vector<std::vector<int>>& board, const std::vector<std::tuple<int, int, int, int>>& lines, bool skip_diagonals) {
        for (const auto& [x1, y1, x2, y2] : lines) {
            draw_line(board, x1, y1, x2, y2, skip_diagonals);
        }
        return count_intersections(board);
    }
}

void aoc::y2021::day_05(const std::string& title) {
    auto input = aoc::file_to_string_vector(aoc::input_path(2021, 5));
    auto lines = parse_input(input);
    auto [wd, hgt] = get_dimensions(lines);

    auto board_part_1 = init_board(wd, hgt);
    auto board_part_2 = board_part_1;

    std::println("--- Day 5: {} ---", title);
    std::println("  part 1: {}", draw_lines(board_part_1, lines, true));
    std::println("  part 2: {}", draw_lines(board_part_2, lines, false));
}