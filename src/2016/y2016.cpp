
#include "y2016.h"
#include <vector>
#include <functional>
#include <print>

void aoc::y2016::do_advent_of_code(int day) {
    const static auto advent_of_code_2016 = std::vector<std::function<void()>>{
        []() { day_01( "No Time for a Taxicab" ); },
        []() { day_02( "Bathroom Security" ); },
        []() { day_03( "Squares With Three Sides" ); },
        []() { day_04( "Security Through Obscurity" ); },
        []() { day_05( "How About a Nice Game of Chess?" ); },
        []() { day_06( "Signals and Noise" ); },
        []() { day_07( "Internet Protocol Version 7" ); },
        []() { day_08( "Two-Factor Authentication" ); },
        []() { day_09( "Explosives in Cyberspace" ); },
        []() { day_10( "Balance Bots" ); },
        []() { day_11( "Radioisotope Thermoelectric Generators" ); },
        []() { day_12( "Leonardo's Monorail" ); },
        []() { day_13( "A Maze of Twisty Little Cubicles" ); },
        []() { day_14( "One-Time Pad" ); },
        []() { day_15( "Timing is Everything" ); },
        []() { day_16( "Dragon Checksum" ); },
        []() { day_17( "Two Steps Forward" ); },
        []() { day_18( "Like a Rogue" ); },
        []() { day_19( "An Elephant Named Joseph" ); },
        []() { day_20( "Firewall Rules" ); },
        []() { day_21( "Scrambled Letters and Hash" ); },
        []() { day_22( "Grid Computing" ); },
        []() { day_23( "Safe Cracking" ); },
        []() { day_24( "Air Duct Spelunking" ); },
        []() { day_25( "Clock Signal" ); }
    };
    
    auto index = day - 1;
    if (index < 0) {
        std::print("invalid day: {0}\n", day);
        return;
    }

    if (index < advent_of_code_2016.size()) {
        advent_of_code_2016[index]();
    }
    else {
        std::print("Day {0} is not complete.\n", day);
    }
}
