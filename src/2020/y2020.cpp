#include "y2020.h"
#include <vector>
#include <functional>
#include <print>

void aoc::y2020::do_advent_of_code(int day) {
    const static auto advent_of_code_2020 = std::vector<std::function<void()>>{
        []() { day_01( "Report Repair" ); },
        []() { day_02( "Password Philosophy" );  },
        []() { day_03( "Toboggan Trajectory" );  },
        []() { day_04( "Passport Processing" ); },
        []() { day_05( "Binary Boarding" );  },
        []() { day_06( "Custom Customs" );  },
        []() { day_07( "Handy Haversacks" ); },
        []() { day_08( "Handheld Halting" ); },
        []() { day_09( "Encoding Error" ); },
        []() { day_10( "Adapter Array" ); },
        []() { day_11( "Seating System" ); },
        []() { day_12( "Rain Risk" ); },
        []() { day_13( "Shuttle Search" ); },
        []() { day_14( "Docking Data" ); },
        []() { day_15( "Rambunctious Recitation" ); },
        []() { day_16( "Ticket Translation" ); },
        []() { day_17( "Conway Cubes" ); },
        []() { day_18( "Operation Order" ); },
        []() { day_19( "Monster Messages" ); },
        []() { day_20( "Jurassic Jigsaw" ); },
        []() { day_21( "Allergen Assessment" ); },
        []() { day_22( "Crab Combat" ); },
        []() { day_23( "Crab Cups" ); },
        []() { day_24( "Lobby Layout" ); },
        []() { day_25( "Combo Breaker" ); }
    };

    auto index = day - 1;
    if (index < 0) {
        std::print("invalid day: {0}\n", day);
        return;
    }

    if (index < advent_of_code_2020.size()) {
        advent_of_code_2020[index]();
    } else {
        std::print("Day {0} is not complete.\n", day);
    }
}