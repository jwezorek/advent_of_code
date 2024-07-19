#include "../util/util.h"
#include "y2020.h"
#include <filesystem>
#include <functional>
#include <print>
#include <ranges>
#include <unordered_map>
#include <deque>

namespace r = std::ranges;
namespace rv = std::ranges::views;

/*------------------------------------------------------------------------------------------------*/

namespace {

    class number_history {
        std::unordered_map<int, std::deque<int>> history_;
        int last_number_spoken_;

        void insert_into_history(int num, int when) {
            auto& times = history_[num];
            if (times.size() == 2) {
                times.pop_front();
            }
            times.push_back(when);
        }

    public:
        void speak_number(int num, int when) {
            last_number_spoken_ = num;
            insert_into_history(num, when);
        }

        int last_number_spoken() const {
            return last_number_spoken_;
        }

        bool was_spoken(int num) const {
            return history_.contains(num);
        }

        int time_diff(int num) const {
            return history_.at(num).back() - history_.at(num).front();
        }
    };

    int nth_number_spoken(const std::vector<int>& start, int n) {
        number_history history;
        for (int turn = 1; turn <= n; ++turn) {
            if (turn <= start.size()) {
                history.speak_number(start[turn - 1], turn);
                continue;
            }
            if (!history.was_spoken(history.last_number_spoken())) {
                history.speak_number(0, turn);
                continue;
            }
            history.speak_number(history.time_diff(history.last_number_spoken()), turn);
        } 
        return history.last_number_spoken();
    }
}

void aoc::y2020::day_15(const std::string& title) {
    auto starting_numbers = aoc::extract_numbers(
        aoc::file_to_string(aoc::input_path(2020, 15))
    );

    std::println("--- Day 15: {} ---", title);
    std::println("  part 1: {}", nth_number_spoken(starting_numbers, 2020));
    std::println("  part 2: {}", nth_number_spoken(starting_numbers, 30000000));
}