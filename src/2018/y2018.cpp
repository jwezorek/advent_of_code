
#include "y2018.h"
#include <vector>
#include <functional>
#include <print>

void aoc::y2018::do_advent_of_code(int day) {
    const static auto advent_of_code_2018 = std::vector<std::function<void()>>{
        []() { day_01( "Chronal Calibration" ); },
        []() { day_02( "Inventory Management System" ); },
        []() { day_03( "No Matter How You Slice It" ); },
        []() { day_04( "Repose Record" ); },
        []() { day_05( "Alchemical Reduction" ); },
        []() { day_06( "Chronal Coordinates" ); },
        []() { day_07( "The Sum of Its Parts" ); },
        []() { day_08( "Memory Maneuver" ); },
        []() { day_09( "Marble Mania" ); },
        []() { day_10( "The Stars Align" ); },
        []() { day_11( "Chronal Charge" ); },
        []() { day_12( "" ); },
        []() { day_13( "" ); },
        []() { day_14( "" ); },
        []() { day_15( "" ); },
        []() { day_16( "" ); },
        []() { day_17( "" ); },
        []() { day_18( "" ); },
        []() { day_19( "" ); },
        []() { day_20( "" ); },
        []() { day_21( "" ); },
        []() { day_22( "" ); },
        []() { day_23( "" ); },
        []() { day_24( "" ); },
        []() { day_25( "" ); }
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
