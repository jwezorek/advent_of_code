#include "y2023.h"
#include <vector>
#include <functional>
#include <print>

void aoc::y2023::do_advent_of_code(int day) {
    const static auto advent_of_code_2023 = std::vector<std::function<void()>>{
        []() {day_01("Trebuchet?!"); },
        []() {day_02("Cube Conundrum"); },
        []() {day_03("Gear Ratios"); },
        []() {day_04("Scratchcards"); },
        []() {day_05("If You Give A Seed A Fertilizer"); },
        []() {day_06("Wait For It"); },
        []() {day_07("Camel Cards"); },
        []() {day_08("Haunted Wasteland"); },
        []() {day_09("Mirage Maintenance"); },
        []() {day_10("Pipe Maze"); },
        []() {day_11("Cosmic Expansion"); },
        []() {day_12("Hot Springs"); },
        []() {day_13("Point of Incidence"); },
        []() {day_14("Parabolic Reflector Dish"); },
        []() {day_15("Lens Library"); },
        []() {day_16("The Floor Will Be Lava"); },
        []() {day_17("Clumsy Crucible"); },
        []() {day_18("Lavaduct Lagoon"); },
        []() {day_19("Aplenty"); },
        []() {day_20("Pulse Propagation"); },
        []() {day_21("Step Counter"); },
        []() {day_22("Sand Slabs"); },
        []() {day_23("A Long Walk"); },
        []() {day_24("Never Tell Me The Odds"); },
        []() {day_25("Snowverload"); }
    };
    auto index = day - 1;
    if (index < 0) {
        std::print("invalid day: {0}\n", day);
        return;
    }

    if (index < advent_of_code_2023.size()) {
        advent_of_code_2023[index]();
    } else {
        std::print("Day {0} is not complete.\n", day);
    }
}