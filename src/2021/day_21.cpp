#include "../util/util.h"
#include "y2021.h"
#include <filesystem>
#include <functional>
#include <print>
#include <ranges>
#include <boost/multi_array.hpp>

namespace r = std::ranges;
namespace rv = std::ranges::views;

/*------------------------------------------------------------------------------------------------*/

namespace {
    using state_table_t = boost::multi_array<uint64_t, 4>;

    constexpr int board_spaces = 10;

    int one_based_wrap(int v, int b) {
        return ((v - 1) % b) + 1;
    }

    std::tuple<int, int> play_deterministic_dirac(int player_1_position, int player_2_position, int max_score, int die_sides) {
        int score[2] = { 0,0 };
        int position[2] = { player_1_position , player_2_position };
        int current_player = 0;
        int current_die_state = 1;
        int total_die_rolls = 0;
        while (score[0] < max_score && score[1] < max_score) {
            int roll = current_die_state + (one_based_wrap(current_die_state + 1, die_sides)) + (one_based_wrap(current_die_state + 2, die_sides));
            total_die_rolls += 3;
            current_die_state = one_based_wrap(current_die_state + 3, die_sides);
            position[current_player] = one_based_wrap(position[current_player] + roll, board_spaces);
            score[current_player] += position[current_player];
            current_player = (current_player + 1) % 2;
        }
        return { total_die_rolls , std::min(score[0],score[1]) };
    }

    std::vector<std::tuple<int, int>> counts_of_three_threeside_die_rolls() {

        auto base_3_digits = [](int n)->std::tuple<int, int, int> {
            int digits[3] = {};
            for (int threes_place = 9, i = 0; threes_place > 0; threes_place /= 3, ++i) {
                digits[i] = n / threes_place;
                n = n % threes_place;
            }
            return { digits[0],  digits[1],  digits[2] };
            };

        std::vector<int> counts(7, 0);
        for (int i = 0; i < 27; ++i) {
            auto [v1, v2, v3] = base_3_digits(i);
            auto sum = v1 + v2 + v3 + 3;
            counts[sum - 3]++;
        }

        std::vector<std::tuple<int, int>> pairs;
        for (int i = 0; i < counts.size(); ++i) {
            pairs.push_back({ i + 3, counts[i] });
        }

        return pairs;
    }

    struct player_state_t {
        int score;
        int pos;
    };

    class multiverse_state {
        state_table_t tbl_;

    public:
        multiverse_state(int player1_pos = -1, int player2_pos = -1) : tbl_(boost::extents[10][22][10][22]) {
            if (player1_pos > 0 && player2_pos > 0) {
                get({ 0, player1_pos }, { 0, player2_pos })++;
            }
        }

        uint64_t& get(player_state_t p1, player_state_t p2) {
            const multiverse_state& self = static_cast<const multiverse_state&>(*this);
            return  const_cast<uint64_t&>(self.get(p1, p2));
        }

        const uint64_t& get(player_state_t p1, player_state_t p2) const {
            int p1_pos_index = p1.pos - 1;
            int p1_score_index = (p1.score < 21) ? p1.score : 21;
            int p2_pos_index = p2.pos - 1;
            int p2_score_index = (p2.score < 21) ? p2.score : 21;
            return tbl_[p1_pos_index][p1_score_index][p2_pos_index][p2_score_index];
        }

        bool do_player_ones_turn() {
            static auto die_roll_counts = counts_of_three_threeside_die_rolls();
            auto next_state = blank_next_state();
            for (int p1_pos = 1; p1_pos <= 10; ++p1_pos) {
                for (int p1_score = 0; p1_score < 21; ++p1_score) {
                    for (int p2_pos = 1; p2_pos <= 10; ++p2_pos) {
                        for (int p2_score = 0; p2_score < 21; ++p2_score) {
                            for (auto [roll, roll_count] : die_roll_counts) {
                                auto num_universes = get({ p1_score, p1_pos }, { p2_score, p2_pos });
                                if (num_universes == 0) {
                                    continue;
                                }
                                int player1_new_pos = one_based_wrap(p1_pos + roll, board_spaces);
                                int player1_new_score = p1_score + player1_new_pos;
                                next_state.get({ player1_new_score, player1_new_pos }, { p2_score, p2_pos }) += roll_count * num_universes;
                            }
                        }
                    }
                }
            }
            *this = next_state;
            return done();
        }

        // We need to preserve wins...
        multiverse_state blank_next_state() const {
            multiverse_state next;
            for (int p1_pos = 1; p1_pos <= 10; ++p1_pos) {
                for (int p2_pos = 1; p2_pos <= 10; ++p2_pos) {
                    for (int score = 0; score < 21; ++score) {
                        next.get({ 21, p1_pos }, { score,p2_pos }) = get({ 21, p1_pos }, { score,p2_pos });
                        next.get({ score,p1_pos }, { 21,p2_pos }) = get({ score,p1_pos }, { 21,p2_pos });
                    }
                }
            }

            return next;
        }

        bool do_player_twos_turn() {
            static auto die_roll_counts = counts_of_three_threeside_die_rolls();
            auto next_state = blank_next_state();
            for (int p1_pos = 1; p1_pos <= 10; ++p1_pos) {
                for (int p1_score = 0; p1_score < 21; ++p1_score) {
                    for (int p2_pos = 1; p2_pos <= 10; ++p2_pos) {
                        for (int p2_score = 0; p2_score < 21; ++p2_score) {
                            for (auto [roll, roll_count] : die_roll_counts) {
                                auto num_universes = get({ p1_score, p1_pos }, { p2_score, p2_pos });
                                if (num_universes == 0) {
                                    continue;
                                }
                                int player2_new_pos = one_based_wrap(p2_pos + roll, board_spaces);
                                int player2_new_score = p2_score + player2_new_pos;
                                next_state.get({ p1_score, p1_pos }, { player2_new_score, player2_new_pos }) += roll_count * num_universes;
                            }
                        }
                    }
                }
            }
            *this = next_state;

            return done();
        }

        bool done() const {
            for (int p1_pos = 1; p1_pos <= 10; ++p1_pos) {
                for (int p1_score = 0; p1_score < 21; ++p1_score) {
                    for (int p2_pos = 1; p2_pos <= 10; ++p2_pos) {
                        for (int p2_score = 0; p2_score < 21; ++p2_score) {
                            if (get({ p1_score, p1_pos }, { p2_score, p2_pos }) > 0) {
                                return false;
                            }
                        }
                    }
                }
            }
            return true;
        }

        std::tuple<uint64_t, uint64_t> wins() const {
            uint64_t player_1_wins = 0;
            uint64_t player_2_wins = 0;
            for (int p1_pos = 1; p1_pos <= 10; ++p1_pos) {
                for (int p2_pos = 1; p2_pos <= 10; ++p2_pos) {
                    for (int score = 0; score < 21; ++score) {
                        player_1_wins += get({ 21, p1_pos }, { score, p2_pos });
                        player_2_wins += get({ score, p1_pos }, { 21, p2_pos });
                    }
                }
            }
            return { player_1_wins , player_2_wins };
        }

    };

    std::tuple<uint64_t, uint64_t> play_dirac(int player1_pos, int player2_pos) {

        multiverse_state state(player1_pos, player2_pos);
        bool done = false;
        while (!done) {
            done = state.do_player_ones_turn();
            if (!done) {
                done = state.do_player_twos_turn();
            }
        }
        return state.wins();
    }

    std::tuple<int, int> parse_input(const std::string& inp) {
        auto player_pos = aoc::extract_numbers(inp);
        return {
            player_pos[1],
            player_pos[3]
        };
    }
}

void aoc::y2021::day_21(const std::string& title) {
    auto [player_1, player_2] = parse_input(
        aoc::file_to_string(aoc::input_path(2021, 21))
    );

    std::println("--- Day 21: {} ---", title);

    auto [rolls, score] = play_deterministic_dirac(player_1, player_2, 1000, 100);
    std::println("  part 1: {}", rolls * score);

    auto [p1_wins, p2_wins] = play_dirac(player_1, player_2);
    std::println("  part 2: {}", std::max(p1_wins, p2_wins));
}