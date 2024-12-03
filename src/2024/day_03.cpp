
#include "../util/util.h"
#include "y2024.h"
#include <filesystem>
#include <functional>
#include <print>
#include <ranges>
#include <regex>
#include <variant>

namespace r = std::ranges;
namespace rv = std::ranges::views;

/*------------------------------------------------------------------------------------------------*/

namespace {

    using instruction = std::variant<std::pair<int, int>, bool>;

    std::vector<instruction> parse_instructions(const std::string& input) {
        std::vector<instruction> output;

        std::regex pattern(R"(mul\((\d+),(\d+)\)|do\(\)|don't\(\))");
        std::sregex_iterator it(input.begin(), input.end(), pattern);

        while (it != std::sregex_iterator{}) {
            const auto& match = *it;
            if (match[0] == "do()") {
                output.emplace_back(true);
            } else if (match[0] == "don't()") {
                output.emplace_back(false);
            } else {
                output.emplace_back(
                    std::pair<int, int>{
                        std::stoi(match[1].str()),
                        std::stoi(match[2].str())
                    }
                );
            }
            ++it;
        }

        return output;
    }

    int perform_instructions( const std::vector<instruction>& instrs, bool just_mults) {

        bool mult_enabled = true;
        return r::fold_left(
            instrs | rv::transform(
                [&](auto&& instr) {
                    return std::visit(
                        aoc::overload{
                            [&](const std::pair<int,int>& mult) {
                                if (!mult_enabled) {
                                    return 0;
                                }
                                return mult.first * mult.second;
                            },
                            [&](bool enable_mult) {
                                if (!just_mults) {
                                    mult_enabled = enable_mult;
                                }
                                return 0;
                            }
                        },
                        instr
                    );
                }
            ),
            0,
            std::plus<>()
        );
    }
}

void aoc::y2024::day_03(const std::string& title) {

    auto inp = parse_instructions(
        aoc::file_to_string(
            aoc::input_path(2024, 3)
        )
    );

    std::println("--- Day 3: {} ---", title);
    std::println("  part 1: {}", 
        perform_instructions( inp, true) );
    std::println("  part 2: {}",
        perform_instructions( inp, false )
    );
    
}
