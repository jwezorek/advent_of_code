#include "../util/util.h"
#include "y2021.h"
#include <filesystem>
#include <functional>
#include <print>
#include <ranges>
#include <regex>

namespace r = std::ranges;
namespace rv = std::ranges::views;

/*------------------------------------------------------------------------------------------------*/

namespace {
    class bingo_board {
        struct cell {
            int val;
            bool marked;

            cell(int v) :
                val(v), marked(false)
            {}
        };

        std::vector<std::vector<cell>> impl_;
        int dim_;
        bool won_;

        bool check_cell(int row, int col) const {
            return impl_.at(row).at(col).marked;
        }

        bool check_row(int row) const {
            for (int col = 0; col < dim_; ++col) {
                if (!check_cell(row, col)) {
                    return false;
                }
            }
            return true;
        }

        bool check_col(int col) const {
            for (int row = 0; row < dim_; ++row) {
                if (!check_cell(row, col)) {
                    return false;
                }
            }
            return true;
        }

        bool check_win() const {
            for (int i = 0; i < dim_; ++i) {
                if (check_row(i) || check_col(i)) {
                    return true;
                }
            }
            return false;
        }

    public:

        bingo_board(const std::vector<std::vector<int>>& rows) : won_(false) {
            dim_ = rows.size();
            impl_ = rows | rv::transform(
                [](const std::vector<int>& row_vals) -> std::vector<cell> {
                    return row_vals | rv::transform(
                        [](const auto& val) {
                            return cell(val);
                        }
                    ) | r::to<std::vector<cell>>();
                }
            ) | r::to<decltype(impl_)>();
        }

        int get_cell_val(int row, int col) const {
            return impl_.at(row).at(col).val;
        }

        void mark_cell(int row, int col) {
            impl_.at(row).at(col).marked = true;
        }

        bool is_dead() const {
            return won_;
        }

        bool play_bingo_number(int n) {
            for (int row = 0; row < dim_; ++row) {
                for (int col = 0; col < dim_; ++col) {
                    if (get_cell_val(row, col) == n) {
                        mark_cell(row, col);
                    }
                }
            }
            won_ = check_win();
            return won_;
        }

        int score(int n) const {
            int sum = 0;
            for (int row = 0; row < dim_; ++row) {
                for (int col = 0; col < dim_; ++col) {
                    sum += (!check_cell(row, col)) ? get_cell_val(row, col) : 0;
                }
            }
            return sum * n;
        }

        std::string to_string() const {
            std::stringstream ss;
            for (const auto& row : impl_) {
                for (const auto& c : row) {
                    ss << c.val << " ";
                }
                ss << "\n";
            }
            return ss.str();
        }
    };

    std::string ltrim(const std::string& s)
    {
        size_t start = s.find_first_not_of(" ");
        return (start == std::string::npos) ? "" : s.substr(start);
    }

    std::tuple<std::string, std::vector<std::string>> get_first_line_strip_blanks(const std::vector<std::string>& lines) {
        std::vector<std::string> stripped = lines | rv::drop(1) | rv::filter(
            [](const auto& str) {return !str.empty(); }
        ) | r::to<std::vector<std::string>>();

        std::vector<std::string> normalized = stripped | rv::transform(
                [](const auto& str) {
                    return ltrim(std::regex_replace(str, std::regex("  "), " "));
                }
            ) | r::to< std::vector<std::string>>();
        return { lines[0], normalized };
    }

    std::vector<int> parse_numbers(const std::string& str, char delimiter) {
        return  aoc::split(str, delimiter) | rv::transform(
                [](const std::string& str)->int {
                    return std::stoi(str);
                }
            ) | r::to<std::vector<int>>();
    }

    std::vector<bingo_board> parse_boards(std::vector<std::string> strs) {
        std::vector<bingo_board> boards;
        auto iter = strs.begin();
        while (iter != strs.end()) {
            std::vector<std::vector<int>> rows;
            for (int i = 0; i < 5; i++) {
                auto str = *(iter++);
                rows.push_back(parse_numbers(str, ' '));
            }
            boards.emplace_back(rows);
        }
        return boards;
    }

    std::tuple<std::vector<int>, std::vector< bingo_board>> parse_game_data(const std::vector<std::string>& game_data) {
        auto [numbers_string, board_data] = get_first_line_strip_blanks(game_data);
        auto numbers = parse_numbers(numbers_string, ',');
        auto boards = parse_boards(board_data);
        return { numbers, boards };
    }

    std::tuple<bingo_board, int> find_first_winner(const std::vector<int>& numbers, std::vector< bingo_board> boards) {
        for (int number : numbers) {
            for (auto& board : boards) {
                if (board.play_bingo_number(number)) {
                    return { board, number };
                }
            }
        }
        throw std::runtime_error("no winner!");
    }

    std::tuple<bingo_board, int> find_last_winner(const std::vector<int>& numbers, std::vector< bingo_board> boards) {
        int last_winning_number;
        bingo_board* last_winning_board = nullptr;

        for (int number : numbers) {
            for (auto& board : boards) {
                if (board.is_dead()) {
                    continue;
                }

                if (board.play_bingo_number(number)) {
                    last_winning_board = &board;
                    last_winning_number = number;
                }
            }
        }
        if (!last_winning_board) {
            throw std::runtime_error("no winner");
        }

        return { *last_winning_board, last_winning_number };
    }
}

void aoc::y2021::day_04(const std::string& title) {
    auto input = aoc::file_to_string_vector(aoc::input_path(2021, 4));
    auto [bingo_numbers, boards] = parse_game_data(input);

    std::println("--- Day 4: {} ---", title);

    auto [first_winning_board, first_winning_number] = find_first_winner(bingo_numbers, boards);
    std::println("  part 1: {}", first_winning_board.score(first_winning_number));

    auto [winning_board, final_number] = find_last_winner(bingo_numbers, boards);
    std::println("  part 2: {}", winning_board.score(final_number));
}