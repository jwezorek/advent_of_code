
#include "../util/util.h"
#include "y2017.h"
#include <filesystem>
#include <functional>
#include <print>
#include <ranges>
#include <unordered_set>

namespace r = std::ranges;
namespace rv = std::ranges::views;

/*------------------------------------------------------------------------------------------------*/

namespace {

    std::string initial_state() {
        return rv::iota(0, 16) | rv::transform(
            [](auto i)->char {
                return i + 'a';
            }
        ) | r::to<std::string>();
    }

    enum move_type {
        spin,
        exchange,
        partner
    };

    struct dance_move {
        move_type move;
        int arg1;
        int arg2;
    };

    dance_move parse_dance_move(const std::string& str) {
        auto op = str.front();
        auto arg_str = str | rv::drop(1) | r::to<std::string>();
        if (op == 's') {
            return { spin, std::stoi(arg_str), 0 };
        } else if (op == 'x') {
            auto args = aoc::extract_numbers(arg_str);
            return { exchange, args[0], args[1] };
        }
        auto args = aoc::split(arg_str, '/');
        return {
            partner,
            static_cast<int>(args[0].front()),
            static_cast<int>(args[1].front())
        };
    }

    void perform_spin(std::string& state, int amt) {
        std::stringstream ss;
        ss << (state | rv::drop(16 - amt) | r::to<std::string>());
        for (auto ch : state | rv::take(16 - amt)) {
            ss << ch;
        }
        state = ss.str();
    }

    void perform_exchange(std::string& state, int arg1, int arg2) {
        std::swap(state[arg1], state[arg2]);
    }

    void perform_partner(std::string& state, char arg1, char arg2) {
        auto first = r::find(state, arg1);
        auto second = r::find(state, arg2);
        std::swap(*first, *second);
    }

    void perform_dance_move(const dance_move& move, std::string& state) {
        switch (move.move) {
            case spin:
                perform_spin(state, move.arg1);
                break;

            case exchange:
                perform_exchange(state, move.arg1, move.arg2);
                break;

            case partner:
                perform_partner(
                    state, 
                    static_cast<char>(move.arg1),
                    static_cast<char>(move.arg2)
                );
                break;
        }
    }

    std::string perform_dance(
            const std::vector<dance_move>& dance, const std::string& start, int iterations) {
        auto state = start;
        for (int i = 0; i < iterations; ++i) {
            for (const auto& move : dance) {
                perform_dance_move(move, state);
            }
        }
        return state;
    }

    std::string dance_a_billion_times(const std::vector<dance_move>& dance) {
        std::unordered_set<std::string> set;
        auto line = initial_state();

        int cycle_sz = 0;
        while (!set.contains(line)) {
            set.insert(line);
            line = perform_dance(dance, line, 1);
            ++cycle_sz;
        }

        return perform_dance(dance, line, 1000000000 % cycle_sz);
    }
}

void aoc::y2017::day_16(const std::string& title) {

    auto inp = aoc::split(
            aoc::file_to_string(aoc::input_path(2017, 16)), ','
        ) | rv::transform(
            parse_dance_move
        ) | r::to<std::vector>();

    std::println("--- Day 16: {} ---", title);
    std::println("  part 1: {}", perform_dance(inp, initial_state(), 1) );
    std::println("  part 2: {}", dance_a_billion_times(inp) );
    
}
