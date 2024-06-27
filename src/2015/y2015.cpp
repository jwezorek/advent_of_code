#include "y2015.h"
#include <vector>
#include <functional>
#include <print>

void aoc::y2015::do_advent_of_code(int day) {
    const static auto advent_of_code_2015 = std::vector<std::function<void()>>{
        []() { day_01( "Not Quite Lisp" ); },
        []() { day_02( "I Was Told There Would Be No Math" ); },
        []() { day_03( "Perfectly Spherical Houses in a Vacuum" ); }
    };
    
    auto index = day - 1;
    if (index < 0) {
        std::print("invalid day: {0}\n", day);
        return;
    }

    if (index < advent_of_code_2015.size()) {
        advent_of_code_2015[index]();
    }
    else {
        std::print("Day {0} is not complete.\n", day);
    }
}