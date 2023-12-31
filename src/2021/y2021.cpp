#include "y2021.h"
#include <vector>
#include <functional>
#include <print>

void aoc::y2021::do_advent_of_code(int day) {
    const static auto advent_of_code_2021 = std::vector<std::function<void()>>{
        []() { day_01( "Sonar Sweep" ); },
        []() { day_02( "Dive!" );  },
        []() { day_03( "Binary Diagnostic" );  },
        []() { day_04( "Giant Squid" ); },
        []() { day_05( "Hydrothermal Venture" );  },
        []() { day_06( "Lanternfish" );  },
        []() { day_07( "The Treachery of Whales" ); },
        []() { day_08( "Seven Segment Search");  },
        []() { day_09( "Smoke Basin");  },
        []() { day_10( "Syntax Scoring"); },
        []() { day_11( "Dumbo Octopus");  },
        []() { day_12( "Passage Pathing");  },
        []() { day_13( "Transparent Origami");  },
        []() { day_14( "Extended Polymerization");  },
        []() { day_15( "Chiton");  },
        []() { day_16( "Packet Decoder"); },
        []() { day_17( "Trick Shot"); },
        []() { day_18( "Snailfish"); },
        []() { day_19( "Beacon Scanner"); },
        []() { day_20( "Trench Map"); },
        []() { day_21( "Dirac Dice"); },
        []() { day_22( "Reactor Reboot"); },
        []() { day_23( "Amphipod"); },
        []() { day_24( "Arithmetic Logic Unit"); },
        []() { day_25( "Sea Cucumber"); }
    };

    auto index = day - 1;
    if (index < 0) {
        std::print("invalid day: {0}\n", day);
        return;
    }

    if (index < advent_of_code_2021.size()) {
        advent_of_code_2021[index]();
    }
    else {
        std::print("Day {0} is not complete.\n", day);
    }
}