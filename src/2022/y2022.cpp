#include "y2022.h"
#include <vector>
#include <functional>
#include <print>

void aoc::y2022::do_advent_of_code(int day) {
    const static auto advent_of_code_2022 = std::vector<std::function<void()>>{
        []() { day_01( "Calorie Counting" ); },
        []() { day_02( "Rock Paper Scissors" );  },
        []() { day_03( "Rucksack Reorganization" );  },
        []() { day_04( "Camp Cleanup" );  },
        []() { day_05( "Supply Stacks" );  }
    };
    auto index = day - 1;
    if (index < 0) {
        std::print("invalid day: {0}\n", day);
        return;
    }

    if (index < advent_of_code_2022.size()) {
        advent_of_code_2022[index]();
    } else {
        std::print("Day {0} is not complete.\n", day);
    }
}