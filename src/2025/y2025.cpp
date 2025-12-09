
#include "y2025.h"
#include <vector>
#include <functional>
#include <print>

void aoc::y2025::do_advent_of_code(int day) {
    const static auto advent_of_code_2025 = std::vector<std::function<void()>>{
        []() { day_01( "Secret Entrance" ); },
        []() { day_02( "Gift Shop" ); },
        []() { day_03( "Lobby" ); },
        []() { day_04( "Printing Department" ); },
        []() { day_05( "Cafeteria" ); },
        []() { day_06( "Trash Compactor" ); },
        []() { day_07( "Laboratories" ); },
        []() { day_08( "Playground" ); },
        []() { day_09( "Movie Theater" ); },
        []() { day_10( "<unknown>" ); },
        []() { day_11( "<unknown>" ); },
        []() { day_12( "<unknown>" ); },
    };
    
    auto index = day - 1;
    if (index < 0) {
        std::print("invalid day: {0}\n", day);
        return;
    }

    if (index < advent_of_code_2025.size()) {
        advent_of_code_2025[index]();
    }
    else {
        std::print("Day {0} is not complete.\n", day);
    }
}
