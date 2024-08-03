
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
        []() { day_12( "Subterranean Sustainability" ); },
        []() { day_13( "Mine Cart Madness" ); },
        []() { day_14( "Chocolate Charts" ); },
        []() { day_15( "Beverage Bandits" ); },
        []() { day_16( "Chronal Classification" ); },
        []() { day_17( "Reservoir Research" ); },
        []() { day_18( "Settlers of The North Pole" ); },
        []() { day_19( "Go With The Flow" ); },
        []() { day_20( "A Regular Map" ); },
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
