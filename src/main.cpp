#include "2025/y2025.h"
#include "2024/y2024.h"
#include "2023/y2023.h"
#include "2022/y2022.h"
#include "2021/y2021.h"
#include "2020/y2020.h"
#include "2019/y2019.h"
#include "2018/y2018.h"
#include "2017/y2017.h"
#include "2016/y2016.h"
#include "2015/y2015.h"
#include "util/util.h"
#include "util/make_year.h"
#include <functional>
#include <string>
#include <print>
#include <ranges>
#include <optional>

namespace r = std::ranges;
namespace rv = std::ranges::views;

/*------------------------------------------------------------------------------------------------*/

namespace {

    void do_advent_of_code(int year, int day) {
        static const std::unordered_map<int, std::function<void(int)>> years = { 
            {2015, aoc::y2015::do_advent_of_code},
            {2016, aoc::y2016::do_advent_of_code},
            {2017, aoc::y2017::do_advent_of_code},
            {2018, aoc::y2018::do_advent_of_code},
            {2019, aoc::y2019::do_advent_of_code},
            {2020, aoc::y2020::do_advent_of_code},
            {2021, aoc::y2021::do_advent_of_code},
            {2022, aoc::y2022::do_advent_of_code},
            {2023, aoc::y2023::do_advent_of_code},
            {2024, aoc::y2024::do_advent_of_code},
            {2025, aoc::y2025::do_advent_of_code}
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

    std::optional<std::tuple<int, int>> parse_create_args(int argc, char* argv[]) {
        if (argc < 3 || argc > 4) {
            return {};
        }
        auto args = rv::iota(1, argc) | rv::transform(
                [&](auto i) -> std::string {
                    return argv[i];
                }
            ) | rv::filter(
                is_integer
            ) | rv::transform(
                [](const std::string& str) {
                    return std::stoi(str);
                }
            ) | r::to<std::vector>();

        if (args.empty()) {
            return {};
        }

        if (args.size() == 1) {
            args.push_back(25);
        }

        return { { args[0], args[1] } };
        
    }

}

int main(int argc, char* argv[]) {

    if (argc < 3 || argc > 4) {
        std::print(
            "expects either a year and a day\n",
            "or 'create' and a year with an optional number of days\n"
            "to generate day template .cpp files...\n"
        );
        return -1;
    }

    if (std::string{ argv[1] } == "create") {

        auto args = parse_create_args(argc, argv);
        if (!args) {

            std::print(
                "expects 'create [year] (num days)'..."
            );
            return -1;
        }

        auto [year, num_days] = *args;

        auto success = aoc::make_year_stubs(year, num_days);
        if (success) {
            std::print("stubbed in year {}... \n", year);
            return 0;
        } 
        std::print("failed to stub in year {}... \n", year);
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