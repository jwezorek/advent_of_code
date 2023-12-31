#include "../util.h"
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

    class school_o_fish {
    private:
        std::array<uint64_t, 9> counts_;
    public:
        school_o_fish(const std::vector<int>& initial_pop) {
            counts_ = { 0,0,0,0,0,0,0,0,0 };
            for (int days : initial_pop) {
                counts_[days]++;
            }
        }

        void simulate_one_day() {
            auto  reborn = counts_[0];
            for (int i = 1; i < 9; i++) {
                counts_[i - 1] = counts_[i];
            }
            counts_[8] = reborn;
            counts_[6] += reborn;
        }

        uint64_t total() {
            return r::fold_left(counts_, static_cast<uint64_t>(0), std::plus<uint64_t>());
        }

        uint64_t  simulate_n_days(int n) {
            for (int i = 0; i < n; ++i) {
                simulate_one_day();
            }
            return total();
        }
    };

}

void aoc::y2021::day_06(const std::string& title) {
    school_o_fish s(
        aoc::extract_numbers(aoc::file_to_string(aoc::input_path(2021, 6)))
    );

    std::println("--- Day 6: {} ---", title);
    std::println("  part 1: {}", s.simulate_n_days(80));
    std::println("  part 2: {}", s.simulate_n_days(256 - 80));
}