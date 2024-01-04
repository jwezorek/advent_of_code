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
        []() { day_11( "" ); },
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

    if (index < advent_of_code_2020.size()) {
        advent_of_code_2020[index]();
    } else {
        std::print("Day {0} is not complete.\n", day);
    }
}