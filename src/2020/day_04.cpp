#include "../util/util.h"
#include "y2020.h"
#include <filesystem>
#include <functional>
#include <print>
#include <ranges>
#include <map>
#include <set>

namespace r = std::ranges;
namespace rv = std::ranges::views;

/*------------------------------------------------------------------------------------------------*/

namespace {

    using passport = std::map<std::string, std::string>;

    std::vector<std::string> flatten(const std::vector<std::string>& input) {
        return input | rv::chunk_by(
            [](const std::string& str1, const std::string& str2)->bool {
                return str1.size() != 0 && str2.size() != 0;
            }
        ) | rv::transform(
            [](auto rng)->std::string {
                return rng | rv::join_with(' ') | r::to<std::string>();
            }
        ) | rv::filter(
            [](auto&& str) {
                return !str.empty();
            }
        ) | r::to<std::vector<std::string>>();
    }

    passport parse_passport(const std::string& str) {
        auto fields = aoc::split(str, ' ');
        return fields | rv::transform(
            [](auto&& field)->passport::value_type {
                auto field_val = aoc::split(field, ':');
                return { field_val.front(), field_val.back() };
            }
        ) | r::to<passport>();
    }

    bool has_necessary_fields(const passport& psp) {
        static const std::array<std::string, 7> fields = {
            "byr", "iyr", "eyr", "hgt",
            "hcl", "ecl", "pid"
        };
        for (auto field : fields) {
            if (!psp.contains(field)) {
                return false;
            }
        }
        return true;
    }

    bool valid_year(const std::string& str, int min, int max) {
        auto year = std::stoi(str);
        return year >= min && year <= max;
    }

    bool valid_height(const std::string& str) {
        bool is_cm = str.find("cm") != std::string::npos;
        bool is_inches = str.find("in") != std::string::npos;
        if (!is_cm && !is_inches) {
            return false;
        }
        auto hgt = std::stoi(aoc::remove_nonnumeric(str));
        if (is_cm) {
            return hgt >= 150 && hgt <= 193;
        } else {
            return hgt >= 59 && hgt <= 76;
        }
    }

    bool valid_hair_color(const std::string& str) {
        if (str.front() != '#' || str.size() != 7) {
            return false;
        }
        for (char ch : str | rv::drop(1)) {
            bool valid = (ch >= '0' && ch <= '9') || (ch >= 'a' && ch <= 'f');
            if (!valid) {
                return false;
            }
        }
        return true;
    }

    bool valid_eye_color(const std::string& str) {
        static const std::set<std::string> colors = {
            "amb", "blu", "brn", "gry", "grn", "hzl", "oth"
        };
        return colors.contains(str);
    }

    bool valid_pid(const std::string& str) {
        if (str.size() != 9) {
            return false;
        }
        for (char ch : str) {
            if (!std::isdigit(ch)) {
                return false;
            }
        }
        return true;
    }

    using field_validator = std::function<bool(const std::string&)>;
    bool has_valid_fields(const passport& psp) {
        if (!has_necessary_fields(psp)) {
            return false;
        }
        static const std::map<std::string, field_validator> validators = {
            {"byr", [](const std::string& str) { return valid_year(str, 1920, 2002); }},
            {"iyr", [](const std::string& str) { return valid_year(str, 2010, 2020); }},
            {"eyr", [](const std::string& str) { return valid_year(str, 2020, 2030); }},
            {"hgt", valid_height},
            {"hcl", valid_hair_color},
            {"ecl", valid_eye_color},
            {"pid", valid_pid},
            {"cid", [](const std::string&) {return true; }}
        };

        for (const auto& [key, val] : psp) {
            if (!validators.at(key)(val)) {
                return false;
            }
        }

        return true;
    }
}

void aoc::y2020::day_04(const std::string& title) {
    auto input = flatten(
            aoc::file_to_string_vector(aoc::input_path(2020, 4))
        ) | rv::transform(
            parse_passport
        ) | r::to<std::vector<passport>>();

    std::println("--- Day 4: {} ---", title);
    std::println("  part 1: {}", 
        r::fold_left(
            input | rv::transform([](auto&& passport) {
                return has_necessary_fields(passport) ? 1 : 0; }
            ),
            0,
            std::plus<>()
        )
    );
    std::println("  part 2: {}",
        r::fold_left(
            input | rv::transform([](auto&& passport) {
                return has_valid_fields(passport) ? 1 : 0; }
            ),
            0,
            std::plus<>()
        )
    );
}
