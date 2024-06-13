#include "y2019.h"
#include <vector>
#include <functional>
#include <print>

void aoc::y2019::do_advent_of_code(int day) {
    const static auto advent_of_code_2019 = std::vector<std::function<void()>>{
        []() { day_01( "The Tyranny of the Rocket Equation" ); },
        []() { day_02( "1202 Program Alarm" ); }
    };

    auto index = day - 1;
    if (index < 0) {
        std::print("invalid day: {0}\n", day);
        return;
    }

    if (index < advent_of_code_2019.size()) {
        advent_of_code_2019[index]();
    }
    else {
        std::print("Day {0} is not complete.\n", day);
    }
}