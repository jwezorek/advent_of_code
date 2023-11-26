#include "days.h"
#include "../util.h"
#include <print>

/*------------------------------------------------------------------------------------------------*/

void aoc::y2023::day_01(const std::string& title) {
    std::println("*** day 1 : {0} ***\n", title);

    auto strings = aoc::file_to_string_vector(aoc::input_path(2023, 1));
    for (auto str : strings) {
        std::print("{0}\n", str);
    }
}