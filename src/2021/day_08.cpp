#include "../util.h"
#include "y2021.h"
#include <filesystem>
#include <functional>
#include <print>
#include <ranges>
#include <unordered_map>

namespace r = std::ranges;
namespace rv = std::ranges::views;

/*------------------------------------------------------------------------------------------------*/

namespace {

    struct display_data {
        std::array<std::string, 10> numerals;
        std::array<std::string, 4> display;
    };

    display_data parse_line_of_input(const std::string& line) {
        auto input_and_output = aoc::split(line, '|');
        auto input = aoc::split(aoc::trim(input_and_output[0]), ' ');
        auto output = aoc::split(aoc::trim(input_and_output[1]), ' ');

        if (input.size() != 10 || output.size() != 4)
            throw std::runtime_error("error parsing input");

        std::array<std::string, 10> numerals;
        r::copy(input, numerals.begin());
        std::array<std::string, 4> display;
        r::copy(output, display.begin());

        return { numerals, display };
    }

    std::vector<display_data> parse_input(const std::vector<std::string>& input) {
        return input | rv::transform(
                [](const std::string& str) {
                    return parse_line_of_input(str);
                }
            ) | r::to<std::vector<display_data>>();
    }

    int puzzle_part_1(const std::vector<display_data>& data) {
        int sum = 0;
        for (const auto& d : data) {
            for (const auto& str : d.display) {
                if ((str.size() == 2) || (str.size() == 4) || (str.size() == 3) || (str.size() == 7)) {
                    ++sum;
                }
            }
        }
        return sum;
    }

    std::map<char, int> code_to_mapping(const std::string& code) {
        std::map<char, int> mapping;
        for (char c = 'a'; c < 'h'; ++c) {
            int i = c - 'a';
            char numeral = code[i];
            mapping[c] = numeral - '0';
        }
        return mapping;
    }

    std::string decode_glyph_to_segments_string(const std::string& code, const std::string& coded_glyph) {
        auto mapping = code_to_mapping(code);
        std::string output = "OOOOOOO";
        for (char c : coded_glyph) {
            output[mapping[c]] = 'X';
        }
        return output;
    }

    std::optional<int> coded_glyph_to_digit(const std::string& code, const std::string& coded_glyph) {
        static std::unordered_map<std::string, int> segments_to_digit = {
            {"XXXOXXX", 0},
            {"OOXOOXO", 1},
            {"XOXXXOX", 2},
            {"XOXXOXX", 3},
            {"OXXXOXO", 4},
            {"XXOXOXX", 5},
            {"XXOXXXX", 6},
            {"XOXOOXO", 7},
            {"XXXXXXX", 8},
            {"XXXXOXX", 9}
        };

        auto segments_str = decode_glyph_to_segments_string(code, coded_glyph);
        auto iter = segments_to_digit.find(segments_str);
        if (iter == segments_to_digit.end()) {
            return std::nullopt;
        }
        return iter->second;
    }

    bool does_code_decode_display(const std::string& code, const std::array<std::string, 10>& digit_strings) {
        for (const auto& coded_glyph : digit_strings) {
            auto decoded_digit = coded_glyph_to_digit(code, coded_glyph);
            if (!decoded_digit) {
                return false;
            }
        }
        return true;
    }

    int decode_display(const std::string& code, const std::array<std::string, 4>& coded_display) {
        std::array<int, 4> digits;
        r::transform(coded_display, digits.begin(),
            [code](const std::string& coded_digit) {
                auto digit = coded_glyph_to_digit(code, coded_digit);
                if (!digit) {
                    throw std::runtime_error("bad code");
                }
                return *digit;
            }
        );
        return digits[3] + 10 * digits[2] + 100 * digits[1] + 1000 * digits[0];
    }

    std::vector<std::vector<int>> generate_all_permutations(const std::vector<int>& nums) {

        if (nums.size() == 1) {
            return std::vector<std::vector<int>>{nums};
        }

        static auto split = [](int n, const std::vector<int>& nums) {
            int nth_val = nums[n];
            std::vector<int> ary(nums.size() - 1);
            std::copy_if(nums.begin(), nums.end(), ary.begin(),
                [nth_val](int v) { return v != nth_val; }
            );
            return std::tuple<int, std::vector<int>>{ nth_val, std::move(ary) };
        };

        static auto join = [](int n, const std::vector<int>& vec) {
            auto v = vec;
            v.push_back(n);
            return v;
        };

        std::vector<std::vector<int>> permutations;
        for (int i = 0; i < nums.size(); ++i) {
            auto [n, remaining] = split(i, nums);
            auto permutations_n_minus_1 = generate_all_permutations(remaining);
            for (const auto& p : permutations_n_minus_1) {
                permutations.push_back(join(n, p));
            }
        }

        return permutations;
    }

    std::vector<std::string> generate_all_codes() {
        return generate_all_permutations({ 0,1,2,3,4,5,6 }) | rv::transform(
                    [](const auto& p) {
                        std::string code(' ', p.size());
                        for (int i = 0; i < p.size(); ++i) {
                            code[i] = p[i] + '0';
                        }
                        return code;
                    }
            ) | r::to<std::vector<std::string>>();
    }

    int crack_code(const display_data& data) {
        static auto codes = generate_all_codes();
        for (const auto& code : codes) {
            if (does_code_decode_display(code, data.numerals)) {
                return decode_display(code, data.display);
            }
        }
        return -1;
    }
}

void aoc::y2021::day_08(const std::string& title) {
    auto input = aoc::file_to_string_vector(aoc::input_path(2021, 8));
    auto display_data = parse_input(input);

    std::println("--- Day 8: {} ---", title);
    std::println("  part 1: {}", puzzle_part_1(display_data));

    // Just use brute force ... there is a cleverer way to come up
    // with but C++ is fast enough to just brute force this.
    int sum = 0;
    for (const auto& dd : display_data) {
        auto display_value = crack_code(dd);
        sum += display_value;
    }
    std::println("  part 2: {}", sum);
}