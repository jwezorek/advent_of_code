#include "y2022.h"
#include <vector>
#include <functional>
#include <print>

void aoc::y2022::do_advent_of_code(int day) {
    const static auto advent_of_code_2022 = std::vector<std::function<void()>>{
        []() { day_01( "Calorie Counting" ); },
        []() { day_02( "Rock Paper Scissors" );  },
        []() { day_03( "Rucksack Reorganization" );  },
        []() { day_04( "Camp Cleanup" );  },
        []() { day_05( "Supply Stacks" );  },
        []() { day_06( "Tuning Trouble" ); },
        []() { day_07( "No Space Left On Device" );  },
        []() { day_08( "Treetop Tree House" );  },
        []() { day_09( "Rope Bridge" );  },
        []() { day_10( "Cathode-Ray Tube" );  },
        []() { day_11( "Monkey in the Middle"); },
        []() { day_12( "Hill Climbing Algorithm");  },
        []() { day_13( "Distress Signal");  },
        []() { day_14( "Regolith Reservoir");  },
        []() { day_15( "Beacon Exclusion Zone");  },
        []() { day_16( "Proboscidea Volcanium"); },
        []() { day_17( "Pyroclastic Flow");  },
        []() { day_18( "Boiling Boulders");  },
        []() { day_19( "Not Enough Minerals");  },
        []() { day_20( "Grove Positioning System");  },
        []() { day_21( "Monkey Math"); },
        []() { day_22( "Monkey Map");  },
        []() { day_23( "Unstable Diffusion");  },
        []() { day_24( "Blizzard Basin");  },
        []() { day_25( "Full of Hot Air");  }
    };

    auto index = day - 1;
    if (index < 0) {
        std::print("invalid day: {0}\n", day);
        return;
    }

    if (index < advent_of_code_2022.size()) {
        advent_of_code_2022[index]();
    } else {
        std::print("Day {0} is not complete.\n", day);
    }
}