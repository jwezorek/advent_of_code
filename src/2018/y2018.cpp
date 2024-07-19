
#include "y2018.h"
#include <vector>
#include <functional>
#include <print>

void aoc::y2018::do_advent_of_code(int day) {
    const static auto advent_of_code_2018 = std::vector<std::function<void()>>{
        []() { day_01( "Chronal Calibration" ); },
        []() { day_02( "Inventory Management System" ); },
        []() { day_03( "<unknown>" ); },
        []() { day_04( "<unknown>" ); },
        []() { day_05( "<unknown>" ); },
        []() { day_06( "<unknown>" ); },
        []() { day_07( "<unknown>" ); },
        []() { day_08( "<unknown>" ); },
        []() { day_09( "<unknown>" ); },
        []() { day_10( "<unknown>" ); },
        []() { day_11( "<unknown>" ); },
        []() { day_12( "<unknown>" ); },
        []() { day_13( "<unknown>" ); },
        []() { day_14( "<unknown>" ); },
        []() { day_15( "<unknown>" ); },
        []() { day_16( "<unknown>" ); },
        []() { day_17( "<unknown>" ); },
        []() { day_18( "<unknown>" ); },
        []() { day_19( "<unknown>" ); },
        []() { day_20( "<unknown>" ); },
        []() { day_21( "<unknown>" ); },
        []() { day_22( "<unknown>" ); },
        []() { day_23( "<unknown>" ); },
        []() { day_24( "<unknown>" ); },
        []() { day_25( "<unknown>" ); }
    };
    
    auto index = day - 1;
    if (index < 0) {
        std::print("invalid day: {0}\n", day);
        return;
    }

    if (index < advent_of_code_2018.size()) {
        advent_of_code_2018[index]();
    }
    else {
        std::print("Day {0} is not complete.\n", day);
    }
}
