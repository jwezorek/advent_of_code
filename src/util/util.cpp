#include "util.h"
#include <ranges>
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <functional>
#include <stdexcept>
#include <filesystem>

namespace r = std::ranges;
namespace rv = std::ranges::views;
namespace fs = std::filesystem;

/*------------------------------------------------------------------------------------------------*/

namespace {

    void ltrim(std::string& s) {
        s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
            return !std::isspace(ch);
            }));
    }

    // trim from end (in place)
    void rtrim(std::string& s) {
        s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
            return !std::isspace(ch);
            }).base(), s.end());
    }

    // We assume the executable is being run from the build the directory and that the
    // build directory is somewhere in the project directory. 

    fs::path aoc_directory() {
        auto dir = fs::current_path();
        while (!dir.empty() && !dir.filename().string().contains("advent_of_code")) {
            dir = dir.parent_path();
        }
        if (dir.empty()) {
            throw std::runtime_error("exe is not in a child of an 'advent_of_code' directory");
        }
        return dir;
    }
}

// Input files are placed in <aoc directory>/src/<year>/input

std::string aoc::input_path(int year, int day, const std::string& tag) {

    std::stringstream filename;
    filename << "day_";

    if (day < 10) {
        filename << '0';
    }
    filename << day;

    if (!tag.empty()) {
        filename << "_" << tag;
    }

    filename << ".txt";

    auto inp_path = aoc_directory() / "src" / 
        std::to_string(year) / "input" / filename.str();
    return inp_path.string();
}

std::vector<std::string> aoc::file_to_string_vector(const std::string& filename) {
    std::vector<std::string> v;

    std::ifstream fs(filename);
    if (!fs) {
        throw std::runtime_error("bad file");
    }

    std::string line;
    while (std::getline(fs, line)) {
        v.push_back(line);
    }
    return v;
}

std::string aoc::trim(const std::string& str) {
    auto trimmed = str;
    ltrim(trimmed);
    rtrim(trimmed);
    return trimmed;
}

std::string aoc::collapse_whitespace(const std::string& str) {
    std::stringstream ss;
    for (auto i = str.begin(); i != str.end(); ++i) {
        ss << *i;
        if (std::isspace(*i)) {
            while (i + 1 != str.end() && std::isspace(*(i + 1))) {
                ++i;
            }
        }
    }
    return trim(ss.str());
}

bool aoc::is_number(const std::string& s) {
    return !s.empty() && std::find_if(s.begin(),
        s.end(), [](unsigned char c) { return !std::isdigit(c); }) == s.end();
}

std::vector<std::string> aoc::split(const std::string& s, char delim) {
    std::stringstream ss(s);
    std::string item;
    std::vector<std::string> elems;
    while (std::getline(ss, item, delim)) {
        elems.push_back(item);
    }
    return elems;
}

std::string aoc::file_to_string(const std::string& filename) {
    std::ifstream file(filename);
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

std::vector<std::vector<int>> aoc::strings_to_2D_array_of_digits(const std::vector<std::string>& lines) {
    std::vector<std::vector<int>> grid(lines.size());
    std::transform(lines.begin(), lines.end(), grid.begin(),
        [](const std::string& line)->std::vector<int> {
            std::vector<int> row(line.size());
            std::transform(line.begin(), line.end(), row.begin(),
                [](char c)->int {
                    return c - '0';
                }
            );
            return row;
        }
    );
    return grid;
}

std::string aoc::remove_nonalphabetic(const std::string& str) {
    auto s = str;
    s.erase(std::remove_if(s.begin(), s.end(),
        [](auto const& c) -> bool { return !std::isalpha(c); }), s.end());
    return s;
}

std::string aoc::remove_nonnumeric(const std::string& str) {
    auto s = str;
    s.erase(std::remove_if(s.begin(), s.end(),
        [](auto const& c) -> bool { return !std::isdigit(c); }), s.end());
    return s;
}

std::vector<std::string> aoc::extract_alphabetic(const std::string& str) {
    auto just_letters = aoc::collapse_whitespace(
        str | rv::transform(
            [](char ch)->char {
                return (std::isalpha(ch)) ? ch : ' ';
            }
        ) | r::to<std::string>()
    );
    return split(just_letters, ' ');
}

std::vector<std::string> aoc::extract_alphanumeric(const std::string& str) {
    auto just_alnum = aoc::collapse_whitespace(
        str | rv::transform(
            [](char ch)->char {
                return (std::isalnum(ch)) ? ch : ' ';
            }
        ) | r::to<std::string>()
    );
    return split(just_alnum, ' ');
}

std::vector<int> aoc::extract_numbers(const std::string& str, bool allow_negatives) {
    std::function<bool(char)> is_digit = (allow_negatives) ?
        [](char ch)->bool {return std::isdigit(ch) || ch == '-'; } :
        [](char ch)->bool {return std::isdigit(ch); };
    auto just_numbers = aoc::collapse_whitespace( str |
        rv::transform(
            [is_digit](char ch)->char {
                return (is_digit(ch)) ? ch : ' ';
            }
        ) | r::to<std::string>()
    );
    auto pieces = split(just_numbers, ' ');
    pieces = pieces |
        rv::filter([](auto&& str) {return !str.empty(); }) |
        r::to<std::vector<std::string>>();
    return pieces |
        rv::transform(
            [](const auto& p)->int {
                return std::stoi(p);
            }
    ) | r::to<std::vector<int>>();
}


int64_t aoc::string_to_int64(const std::string& str) {
    int64_t value;
    std::istringstream iss(str);
    iss >> value;
    return value;
}

std::vector<std::vector<std::string>> aoc::group_strings_separated_by_blank_lines(
        const std::vector<std::string>& input)  {
    return input | rv::chunk_by(
            [](const std::string& str1, const std::string& str2)->bool {
                return str1.size() != 0 && str2.size() != 0;
            }
        ) | rv::transform(
            [](auto rng)->std::vector<std::string> {
                return rng | r::to<std::vector<std::string>>();
            }
        ) | rv::filter(
            [](auto&& strs) {
                if (strs.empty()) {
                    return false;
                }
                if (strs.size() == 1 && strs.front().empty()) {
                    return false;
                }
                return true;
            }
        ) | r::to<std::vector<std::vector<std::string>>>();
}

std::vector<int64_t> aoc::extract_numbers_int64(const std::string& str, bool allow_negatives) {
    std::function<bool(char)> is_digit = (allow_negatives) ?
        [](char ch)->bool {return std::isdigit(ch) || ch == '-'; } :
        [](char ch)->bool {return std::isdigit(ch); };
    auto just_numbers = aoc::collapse_whitespace(str |
        rv::transform(
            [is_digit](char ch)->char {
                return (is_digit(ch)) ? ch : ' ';
            }
        ) | r::to<std::string>()
                );
    auto pieces = split(just_numbers, ' ');
    pieces = pieces |
        rv::filter([](auto&& str) {return !str.empty(); }) |
        r::to<std::vector<std::string>>();
    return pieces |
        rv::transform(
            string_to_int64
        ) | r::to<std::vector<int64_t>>();
}