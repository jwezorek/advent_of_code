#include "2023/y2023.h"
#include "2022/y2022.h"
#include "2021/y2021.h"
#include "2020/y2020.h"
#include "2019/y2019.h"
#include "2018/y2018.h"
#include "2017/y2017.h"
#include "2015/y2015.h"
#include "util/util.h"
#include "util/make_year.h"
#include <iostream>
#include <vector>
#include <functional>
#include <string>
#include <print>
#include <ranges>

namespace r = std::ranges;
namespace rv = std::ranges::views;

/*------------------------------------------------------------------------------------------------*/

namespace {

    void do_advent_of_code(int year, int day) {
        static const std::unordered_map<int, std::function<void(int)>> years = { 
            {2015, aoc::y2015::do_advent_of_code},
            {2017, aoc::y2017::do_advent_of_code},
            {2018, aoc::y2018::do_advent_of_code},
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

    bool is_integer(const std::string& str) {
        return r::find_if(str, [](auto ch) {return !std::isdigit(ch); }) == str.end();
    }
}

int main(int argc, char* argv[]) {

    if (argc < 3) {
        std::string arg = argv[1];
        if (is_integer(arg)) {
            auto success = aoc::make_year_stubs(std::stoi(argv[1]));
            if (success) {
                std::print("stubbed in year {}... \n", arg);
                return 0;
            } else {
                std::print("failed to stub in year {}... \n", arg);
                return -1;
            }
        }
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