
#include "../util/util.h"
#include "y2018.h"
#include <filesystem>
#include <functional>
#include <print>
#include <ranges>
#include <unordered_set>

namespace r = std::ranges;
namespace rv = std::ranges::views;

/*------------------------------------------------------------------------------------------------*/

namespace {

    struct state {
        std::vector<uint8_t> recipes;
        int elf1;
        int elf2;

        state() {
            recipes = { 3,7 };
            elf1 = 0;
            elf2 = 1;
        }
    };

    void done_one_round(state& s) {
        auto sum = s.recipes[s.elf1] + s.recipes[s.elf2];
        auto tens = sum / 10;
        auto ones = sum % 10;
        if (tens > 0) {
            s.recipes.push_back(tens);
        }
        s.recipes.push_back(ones);
        s.elf1 += (s.recipes[s.elf1] + 1);
        s.elf1 %= s.recipes.size();

        s.elf2 += s.recipes[s.elf2] + 1;
        s.elf2 %= s.recipes.size();
    }

    std::string do_part_1(int num) {
        
        state s;
        while (s.recipes.size() <= num + 10) {
            done_one_round(s);
        }

        std::stringstream ss;
        for (int i = num; i < num + 10; ++i) {
            ss << std::to_string(s.recipes[i]);
        }

        return ss.str();
    }

    std::vector<uint8_t> to_digits(int num) {
        auto str = std::to_string(num);
        return str | rv::transform(
                [](char ch)->uint8_t {
                    return ch - '0';
                }
            ) | r::to<std::vector>();
    }

    std::optional<int> test_for_digits(
            const std::vector<uint8_t>& nums,
            const std::vector<uint8_t>& target,
            int offset) {
        int start = static_cast<int>(nums.size()) - 
            static_cast<int>(target.size()) - offset;
        if (start < 0) {
            return {};
        }
        for (int i = 0; i < target.size(); ++i) {
            if (target.at(i) != nums.at(start + i)) {
                return {};
            }
        }
        return start;
    }

    int do_part_2(int num) {

        auto digits = to_digits(num);

        std::optional<int> loc;
        state s;
        while (!loc) {
            done_one_round(s);
            loc = test_for_digits(s.recipes, digits, 0);
            if (!loc) {
                loc = test_for_digits(s.recipes, digits, 1);
            }
        }

        return *loc;
    }
}

void aoc::y2018::day_14(const std::string& title) {

    auto inp = std::stoi(
        aoc::file_to_string(
            aoc::input_path(2018, 14)
        )
    );

    std::println("--- Day 14: {} ---", title);
    std::println("  part 1: {}", do_part_1( inp ) );
    std::println("  part 2: {}", do_part_2( inp ) );
    
}
