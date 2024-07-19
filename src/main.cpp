#include "2023/y2023.h"
#include "2022/y2022.h"
#include "2021/y2021.h"
#include "2020/y2020.h"
#include "2019/y2019.h"
#include "2015/y2015.h"
#include "util/util.h"
#include <iostream>
#include <vector>
#include <functional>
#include <string>
#include <print>

/*------------------------------------------------------------------------------------------------*/
namespace {
    void do_advent_of_code(int year, int day) {
        static const std::unordered_map<int, std::function<void(int)>> years = { 
            {2015, aoc::y2015::do_advent_of_code},
            {2019, aoc::y2019::do_advent_of_code},
            {2020, aoc::y2020::do_advent_of_code},
            {2021, aoc::y2021::do_advent_of_code},
            {2022, aoc::y2022::do_advent_of_code},
            {2023, aoc::y2023::do_advent_of_code}
        };
        if (!years.contains(year)) {
            std::println("'Advent of Code {}' not found.", year);
            return;
        }
        years.at(year)(day);
    }
}

int main(int argc, char* argv[]) {

    if (argc < 3) {
        std::print( "specify day and year\n" );
        return -1;
    }

    std::string year_str = argv[1];
    if (!aoc::is_number(argv[1])) {
        std::print("invalid year: {0}\n", year_str);
        return -1;
    }

    std::string day_str = argv[2];
    if (!aoc::is_number(argv[1])) {
        std::print( "invalid day: {0}\n", day_str);
        return -1;
    }

    auto day = std::stoi(day_str);
    if (day < 1 || day > 25) {
        std::print( "invalid day: {0}\n", day );
        return -1;
    }

    do_advent_of_code(std::stoi(year_str), day);
    return 0;
}