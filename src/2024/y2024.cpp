
#include "y2024.h"
#include <vector>
#include <functional>
#include <print>

void aoc::y2024::do_advent_of_code(int day) {
    const static auto advent_of_code_2024 = std::vector<std::function<void()>>{
        []() { day_01( "Historian Hysteria" ); },
        []() { day_02( "Red-Nosed Reports" ); },
        []() { day_03( "Mull It Over" ); },
        []() { day_04( "Ceres Search" ); },
        []() { day_05( "Print Queue" ); },
        []() { day_06( "Guard Gallivant" ); },
        []() { day_07( "Bridge Repair" ); },
        []() { day_08( "Resonant Collinearity" ); },
        []() { day_09( "Disk Fragmenter" ); },
        []() { day_10( "Hoof It" ); },
        []() { day_11( "Plutonian Pebbles" ); },
        []() { day_12( "Garden Groups" ); },
        []() { day_13( "Claw Contraption" ); },
        []() { day_14( "Restroom Redoubt" ); },
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

    if (index < advent_of_code_2024.size()) {
        advent_of_code_2024[index]();
    }
    else {
        std::print("Day {0} is not complete.\n", day);
    }
}
