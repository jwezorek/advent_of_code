#include "2023/days.h"
#include "util.h"
#include <iostream>
#include <vector>
#include <functional>
#include <string>
#include <print>

/*------------------------------------------------------------------------------------------------*/

namespace {
    int do_advent_of_code_2023(int day) {
        const static auto advent_of_code_2022 = std::vector<std::function<void()>>{
            []() {aoc::y2023::day_01( "Trebuchet?!" ); },
            []() {aoc::y2023::day_02( "Cube Conundrum" ); },
            []() {aoc::y2023::day_03( "Gear Ratios" ); },
            []() {aoc::y2023::day_04( "Scratchcards" ); }
        };
        auto index = day - 1;
        if (index < 0) {
            std::print("invalid day: {0}\n", day);
            return -1;
        }

        if (index < advent_of_code_2022.size()) {
            advent_of_code_2022[index]();
        } else {
            std::print("Day {0} is not complete.\n", day);
        }

        return 0;
    }
}

int main(int argc, char* argv[]) {

    if (argc < 2) {
        std::print( "no day specified\n" );
        return -1;
    }

    std::string day = argv[1];
    if (!aoc::is_number(argv[1])) {
        std::print( "invalid day: {0}\n", day );
        return -1;
    }

    auto day_number = std::stoi(day);
    if (day_number < 1 || day_number > 25) {
        std::print( "invalid day: {0}\n", day_number );
        return -1;
    }

    return do_advent_of_code_2023(day_number);
}