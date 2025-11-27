#include "make_year.h"
#include <filesystem>
#include <ranges>
#include <print>
#include <fstream>
#include <regex>
#include <format>

namespace fs = std::filesystem;
namespace r = std::ranges;
namespace rv = std::ranges::views;

/*------------------------------------------------------------------------------------------------*/

namespace {

    constexpr auto k_day_cpp = R"(
#include "../util/util.h"
#include "yYEAR.h"
#include <filesystem>
#include <functional>
#include <print>
#include <ranges>
#include <unordered_set>

namespace r = std::ranges;
namespace rv = std::ranges::views;

/*------------------------------------------------------------------------------------------------*/

namespace {

}

void aoc::yYEAR::day_PADDEDDAY(const std::string& title) {

    auto inp = aoc::file_to_string_vector(
            aoc::input_path(YEAR, DAY)
        ); 

    std::println("--- Day DAY: {} ---", title);
    std::println("  part 1: {}", 0);
    std::println("  part 2: {}", 0);
    
}
)";

    constexpr auto k_year_cpp = R"(
#include "yYEAR.h"
#include <vector>
#include <functional>
#include <print>

void aoc::yYEAR::do_advent_of_code(int day) {
    const static auto advent_of_code_YEAR = std::vector<std::function<void()>>{
        []() { day_01( "<unknown>" ); },
        []() { day_02( "<unknown>" ); },
        []() { day_03( "<unknown>" ); },
        []() { day_04( "<unknown>" ); },
        []() { day_05( "<unknown>" ); },
        []() { day_06( "<unknown>" ); },
        []() { day_07( "<unknown>" ); },
        []() { day_08( "<unknown>" ); },
        []() { day_09( "<unknown>" ); },
        []() { day_10( "<unknown>" ); },
        []() { day_11( "<unknown>" ); },
        []() { day_12( "<unknown>" ); },
        []() { day_13( "<unknown>" ); },
        []() { day_14( "<unknown>" ); },
        []() { day_15( "<unknown>" ); },
        []() { day_16( "<unknown>" ); },
        []() { day_17( "<unknown>" ); },
        []() { day_18( "<unknown>" ); },
        []() { day_19( "<unknown>" ); },
        []() { day_20( "<unknown>" ); },
        []() { day_21( "<unknown>" ); },
        []() { day_22( "<unknown>" ); },
        []() { day_23( "<unknown>" ); },
        []() { day_24( "<unknown>" ); },
        []() { day_25( "<unknown>" ); }
    };
    
    auto index = day - 1;
    if (index < 0) {
        std::print("invalid day: {0}\n", day);
        return;
    }

    if (index < advent_of_code_YEAR.size()) {
        advent_of_code_YEAR[index]();
    }
    else {
        std::print("Day {0} is not complete.\n", day);
    }
}
)";

    constexpr auto k_year_header = R"(
#pragma once

#include <string>

namespace aoc {
    namespace yYEAR {
        void day_01(const std::string& title);
        void day_02(const std::string& title);
        void day_03(const std::string& title);
        void day_04(const std::string& title);
        void day_05(const std::string& title);
        void day_06(const std::string& title);
        void day_07(const std::string& title);
        void day_08(const std::string& title);
        void day_09(const std::string& title);
        void day_10(const std::string& title);
        void day_11(const std::string& title);
        void day_12(const std::string& title);
        void day_13(const std::string& title);
        void day_14(const std::string& title);
        void day_15(const std::string& title);
        void day_16(const std::string& title);
        void day_17(const std::string& title);
        void day_18(const std::string& title);
        void day_19(const std::string& title);
        void day_20(const std::string& title);
        void day_21(const std::string& title);
        void day_22(const std::string& title);
        void day_23(const std::string& title);
        void day_24(const std::string& title);
        void day_25(const std::string& title);

        void do_advent_of_code(int day);
    }
}
)";

    fs::path src_directory() {
        auto path = fs::current_path();
        while (path.filename() != "advent_of_code") {
            path = path.parent_path();
        }
        path = path / "src";
        return path;
    }

    void write_to_text_file(const fs::path& fname, const std::string& str) {
        std::ofstream file(fname);
        file << str;
    }

    std::string fill_in_template(const std::string& str, int year, int day = -1) {
        auto temp = str;
        temp = std::regex_replace(temp, std::regex("YEAR"), std::to_string(year));
        if (day > 0) {
            auto day_str = std::to_string(day);
            auto padded_day = (day < 10) ? std::string("0") + day_str : day_str;
            temp = std::regex_replace(temp, std::regex("PADDEDDAY"), padded_day);
            temp = std::regex_replace(temp, std::regex("DAY"), day_str);
        }
        return temp;
    }

    bool stub_in_year(const fs::path path, int year, int num_days) {

        if (!fs::is_empty(path)) {
            return false;
        }

        auto year_cpp_file = std::format("y{}.cpp", year);
        write_to_text_file(path / year_cpp_file, fill_in_template(k_year_cpp, year));

        auto year_header_file = std::format("y{}.h", year);
        write_to_text_file(path / year_header_file, fill_in_template(k_year_header, year));

        for (int day = 1; day <= num_days; ++day) {
            auto day_fname = (day < 10) ? 
                std::format("day_0{}.cpp", day) :
                std::format("day_{}.cpp", day);

            write_to_text_file(
                path / day_fname, fill_in_template(k_day_cpp, year, day)
            );
        }

        return true;
    }
}

bool aoc::make_year_stubs(int year, int num_days) {

    auto year_dir = src_directory() / std::to_string(year);
    if (fs::exists(year_dir)) {
        return false;
    }
    fs::create_directory(year_dir);
    return stub_in_year(fs::path(year_dir), year, num_days);
}