#include "y2021.h"
#include <vector>
#include <functional>
#include <print>

void aoc::y2021::do_advent_of_code(int day) {
    const static auto advent_of_code_2021 = std::vector<std::function<void()>>{
        []() { day_01( "Sonar Sweep" ); },
        []() { day_02( "Dive!" );  },
        []() { day_03( "Binary Diagnostic" );  },
    };

    auto index = day - 1;
    if (index < 0) {
        std::print("invalid day: {0}\n", day);
        return;
    }

    if (index < advent_of_code_2021.size()) {
        advent_of_code_2021[index]();
    }
    else {
        std::print("Day {0} is not complete.\n", day);
    }
}