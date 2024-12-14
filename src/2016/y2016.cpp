
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

    if (index < advent_of_code_2016.size()) {
        advent_of_code_2016[index]();
    }
    else {
        std::print("Day {0} is not complete.\n", day);
    }
}
