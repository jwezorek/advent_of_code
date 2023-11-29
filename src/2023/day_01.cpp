#include "days.h"
#include "../util.h"
#include <print>

/*------------------------------------------------------------------------------------------------*/

void aoc::y2023::day_01(const std::string& title) {
    std::println("*** day 1 : {0} ***\n", title);

    auto priority = aoc::priority_queue<std::string>();
    priority.insert("foo", 5);
    priority.insert("bar", 15);
    priority.insert("quux", 1);
    priority.insert("mumble", 1);

    auto foo = priority.extract_min();
    auto bar = priority.extract_min();

    int aaa;
}