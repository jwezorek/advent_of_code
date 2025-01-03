
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
        []() { day_15( "Warehouse Woes" ); },
        []() { day_16( "Reindeer Maze" ); },
        []() { day_17( "Chronospatial Computer" ); },
        []() { day_18( "RAM Run" ); },
        []() { day_19( "Linen Layout" ); },
        []() { day_20( "Race Condition" ); },
        []() { day_21( "Keypad Conundrum" ); },
        []() { day_22( "Monkey Market" ); },
        []() { day_23( "LAN Party" ); },
        []() { day_24( "Crossed Wires" ); },
        []() { day_25( "Code Chronicle" ); }
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
