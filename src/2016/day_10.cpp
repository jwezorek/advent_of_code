
#include "../util/util.h"
#include "y2016.h"
#include <filesystem>
#include <functional>
#include <print>
#include <ranges>
#include <unordered_map>

namespace r = std::ranges;
namespace rv = std::ranges::views;

/*------------------------------------------------------------------------------------------------*/

namespace {
    struct inp_instruction {
        int value;
        int to_bot;
    };

    struct instruction {
        int from_bot;
        bool low_to_bot;
        int low;
        bool high_to_bot;
        int high;
    };

    inp_instruction parse_inp_instruction(const std::string& str) {
        auto vals = aoc::extract_numbers(str);
        return { vals[0],vals[1] };
    }

    instruction parse_instruction(const std::string& str) {
        auto words = aoc::extract_alphanumeric(str);
        return {
            std::stoi(words[1]),
            words[5] == "bot",
            std::stoi(words[6]),
            words[10] == "bot",
            std::stoi(words[11])
        };
    }

    std::tuple<std::vector< inp_instruction>, std::vector<instruction>> parse(
            const std::vector<std::string>& inp) {
        std::vector<inp_instruction> inp_instrs;
        std::vector<instruction> instrs;

        for (const auto& str : inp) {
            if (str.contains("value")) {
                inp_instrs.push_back(parse_inp_instruction(str));
            } else {
                instrs.push_back( parse_instruction(str) );
            }
        }

        r::sort(instrs,
            [](auto&& lhs, auto&& rhs) {
                return lhs.from_bot < rhs.from_bot;
            }
        );

        return {
            std::move(inp_instrs),
            std::move(instrs)
        };
    }

    int do_part_1(const std::vector<inp_instruction>& init, const std::vector<instruction>& instrs) {
        std::vector<std::vector<int>> bots(instrs.size());
        std::unordered_map<int, int> output;

        for (const auto& inp : init) {
            bots[inp.to_bot].push_back(inp.value);
        }

        int answer = -1;
        while (answer < 0) {
            for (size_t i = 0; i < bots.size(); ++i) {
                auto& bot = bots[i];
                if (bot.size() == 2) {

                    r::sort(bot);
                    auto low = bot.front();
                    auto high = bot.back();
                    bot.clear();
                    
                    if (low == 17 && high == 61) {
                        answer = i;
                        break;
                    }
                    const auto& instr = instrs[i];
                    if (instr.low_to_bot) {
                        bots[instr.low].push_back(low);
                    } else {
                        output[instr.low] = low;
                    }

                    if (instr.high_to_bot) {
                        bots[instr.high].push_back(high);
                    }
                    else {
                        output[instr.high] = high;
                    }
                }
            }

        } 

        return answer;
    }

    int do_part_2(const std::vector<inp_instruction>& init, const std::vector<instruction>& instrs) {
        std::vector<std::vector<int>> bots(instrs.size());
        std::unordered_map<int, int> output;

        for (const auto& inp : init) {
            bots[inp.to_bot].push_back(inp.value);
        }

        bool done = true;
        do {
            for (size_t i = 0; i < bots.size(); ++i) {
                auto& bot = bots[i];
                if (bot.size() == 2) {
                    done = false;

                    r::sort(bot);
                    auto low = bot.front();
                    auto high = bot.back();
                    bot.clear();

                    const auto& instr = instrs[i];
                    if (instr.low_to_bot) {
                        bots[instr.low].push_back(low);
                    }
                    else {
                        output[instr.low] = low;
                    }

                    if (instr.high_to_bot) {
                        bots[instr.high].push_back(high);
                    }
                    else {
                        output[instr.high] = high;
                    }
                }
            }

        } while (!done);

        return output[0] * output[1] * output[2];
    }
}

void aoc::y2016::day_10(const std::string& title) {

    auto [init, instrs] = parse(
        aoc::file_to_string_vector(
            aoc::input_path(2016, 10)
        )
    );

    std::println("--- Day 10: {} ---", title);
    std::println("  part 1: {}", do_part_1(init, instrs) );
    //std::println("  part 2: {}", do_part_2(init, instrs) );
    
}
