#include "2023/days.h"
#include "util.h"
#include <iostream>
#include <vector>
#include <functional>
#include <string>
#include <print>

/*------------------------------------------------------------------------------------------------*/

namespace {
    int do_advent_of_code_2023(int day) {
        const static auto advent_of_code_2023 = std::vector<std::function<void()>>{
            []() {aoc::y2023::day_01( "Trebuchet?!" ); },
            []() {aoc::y2023::day_02( "Cube Conundrum" ); },
            []() {aoc::y2023::day_03( "Gear Ratios" ); },
            []() {aoc::y2023::day_04( "Scratchcards" ); },
            []() {aoc::y2023::day_05( "If You Give A Seed A Fertilizer" ); },
            []() {aoc::y2023::day_06( "Wait For It" ); },
            []() {aoc::y2023::day_07( "Camel Cards"); },
            []() {aoc::y2023::day_08( "Haunted Wasteland"); },
            []() {aoc::y2023::day_09( "Mirage Maintenance"); },
            []() {aoc::y2023::day_10( "Pipe Maze" ); },
            []() {aoc::y2023::day_11( "Cosmic Expansion"); },
            []() {aoc::y2023::day_12( "Hot Springs" ); },
            []() {aoc::y2023::day_13( "Point of Incidence" ); },
            []() {aoc::y2023::day_14( "Parabolic Reflector Dish" ); },
            []() {aoc::y2023::day_15( "Lens Library" ); },
            []() {aoc::y2023::day_16( "The Floor Will Be Lava" ); },
            []() {aoc::y2023::day_17( "Clumsy Crucible" ); },
            []() {aoc::y2023::day_18( "Lavaduct Lagoon" ); },
            []() {aoc::y2023::day_19( "Aplenty"); },
            []() {aoc::y2023::day_20( "???" ); },
            []() {aoc::y2023::day_21( "???" ); },
            []() {aoc::y2023::day_22( "???" ); },
            []() {aoc::y2023::day_23( "???" ); },
            []() {aoc::y2023::day_24( "???" ); },
            []() {aoc::y2023::day_25( "???" ); }
        }; 
        auto index = day - 1;
        if (index < 0) {
            std::print("invalid day: {0}\n", day);
            return -1;
        }

        if (index < advent_of_code_2023.size()) {
            advent_of_code_2023[index]();
        } else {
            std::print("Day {0} is not complete.\n", day);
        }

        return 0;
    }
}

int main(int argc, char* argv[]) {

    if (argc < 2) {
        std::print( "no day specified\n" );
        return -1;
    }

    std::string day = argv[1];
    if (!aoc::is_number(argv[1])) {
        std::print( "invalid day: {0}\n", day );
        return -1;
    }

    auto day_number = std::stoi(day);
    if (day_number < 1 || day_number > 25) {
        std::print( "invalid day: {0}\n", day_number );
        return -1;
    }

    return do_advent_of_code_2023(day_number);
}