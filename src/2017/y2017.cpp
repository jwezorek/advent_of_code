
#include "y2017.h"
#include <vector>
#include <functional>
#include <print>

void aoc::y2017::do_advent_of_code(int day) {
    const static auto advent_of_code_2017 = std::vector<std::function<void()>>{
        []() { day_01( "Inverse Captcha" ); },
        []() { day_02( "Corruption Checksum" ); },
        []() { day_03( "Spiral Memory" ); },
        []() { day_04( "High-Entropy Passphrases" ); },
        []() { day_05( "A Maze of Twisty Trampolines, All Alike" ); },
        []() { day_06( "Memory Reallocation" ); },
        []() { day_07( "Recursive Circus" ); },
        []() { day_08( "I Heard You Like Registers" ); },
        []() { day_09( "Stream Processing" ); },
        []() { day_10( "Knot Hash" ); },
        []() { day_11( "Hex Ed" ); },
        []() { day_12( "Digital Plumber" ); },
        []() { day_13( "Packet Scanners" ); },
        []() { day_14( "Disk Defragmentation" ); },
        []() { day_15( "Dueling Generators" ); },
        []() { day_16( "Permutation Promenade" ); },
        []() { day_17( "Spinlock" ); },
        []() { day_18( "Duet" ); },
        []() { day_19( "A Series of Tubes" ); },
        []() { day_20( "Particle Swarm" ); },
        []() { day_21( "Fractal Art" ); },
        []() { day_22( "Sporifica Virus" ); },
        []() { day_23( "Coprocessor Conflagration" ); },
        []() { day_24( "Electromagnetic Moat" ); },
        []() { day_25( "<unknown>" ); }
    };
    
    auto index = day - 1;
    if (index < 0) {
        std::print("invalid day: {0}\n", day);
        return;
    }

    if (index < advent_of_code_2017.size()) {
        advent_of_code_2017[index]();
    }
    else {
        std::print("Day {0} is not complete.\n", day);
    }
}
