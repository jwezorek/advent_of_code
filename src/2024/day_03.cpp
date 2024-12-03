
#include "../util/util.h"
#include "y2024.h"
#include <filesystem>
#include <functional>
#include <print>
#include <ranges>
#include <regex>
#include <optional>

namespace r = std::ranges;
namespace rv = std::ranges::views;

/*------------------------------------------------------------------------------------------------*/

namespace {

    struct instruction {
        std::optional<std::pair<int, int>> mult;
        bool enable_mult;

        instruction(int x, int y) : mult{ {x,y} }, enable_mult{ false }
        {}

        instruction(bool enable) : mult{}, enable_mult { enable }
        {}
    };

    std::vector<instruction> just_multiply_instructions(const std::string& input) {
        std::vector<instruction> output;

        std::regex pattern(R"(mul\((-?\d+),(-?\d+)\))");
        std::smatch match;

        std::sregex_iterator it(input.begin(), input.end(), pattern);
        std::sregex_iterator end;

        while (it != end) {
            int x = std::stoi((*it)[1].str());
            int y = std::stoi((*it)[2].str());
            output.emplace_back(x, y);
            ++it;
        }

        return output;
    }

    std::vector<instruction> all_instructions(const std::string& input) {
        std::vector<instruction> output;

        std::regex pattern(R"(mul\((-?\d+),(-?\d+)\)|\bdo\(\)|\bdon't\(\))");
        std::smatch match;

        std::sregex_iterator it(input.begin(), input.end(), pattern);
        std::sregex_iterator end;

        while (it != end) {
            if ((*it)[0] == "do()") {
                output.emplace_back(true);
            } else if ((*it)[0] == "don't()") {
                output.emplace_back(false);
            } else if ((*it)[1].matched && (*it)[2].matched) {
                int x = std::stoi((*it)[1].str());
                int y = std::stoi((*it)[2].str());
                output.emplace_back(x, y);
            }
            ++it;
        }

        return output;
    }

    int perform_instructions( const std::vector<instruction>& instructions) {

        bool mult_enabled = true;
        return r::fold_left(
            instructions | rv::transform(
                [&mult_enabled](auto&& instr) {
                    if (instr.mult) {
                        if (!mult_enabled) {
                            return 0;
                        }
                        auto [x, y] = instr.mult.value();
                        return x * y;
                    }
                    mult_enabled = instr.enable_mult;
                    return 0;
                }
            ),
            0,
            std::plus<>()
        );
    }
}

void aoc::y2024::day_03(const std::string& title) {

    auto inp = aoc::file_to_string(
            aoc::input_path(2024, 3)
        ); 

    std::println("--- Day 3: {} ---", title);
    std::println("  part 1: {}", 
        perform_instructions( just_multiply_instructions(inp)) );
    std::println("  part 2: {}", 
        perform_instructions( all_instructions(inp) )
    );
    
}
