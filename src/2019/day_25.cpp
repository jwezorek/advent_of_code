#include "../util/util.h"
#include "y2019.h"
#include "intcode.h"
#include <filesystem>
#include <functional>
#include <print>
#include <ranges>
#include <unordered_set>
#include <iostream>

namespace r = std::ranges;
namespace rv = std::ranges::views;

/*------------------------------------------------------------------------------------------------*/

namespace {
    using computer = aoc::intcode_computer;

    void send_input(computer& droid, const std::string& input) {
        for (char ch : input) {
            droid.run_until_event(ch);
        }
        droid.run_until_event('\n');
    }

    void run_droid(computer& droid, 
            std::function<std::string()> input_fn,
            std::function<void(std::string)> output_fn) {

        std::stringstream output_buffer;
        bool done = false;
        while (!done) {
            auto result = droid.run_until_event();
            if (result == aoc::terminated) {
                output_fn(output_buffer.str());
                done = true;
                continue;
            } else if (result == aoc::awaiting_input) {
                output_fn(output_buffer.str());
                output_buffer = {};
                std::string inp = input_fn();
                if (inp == "quit\n") {
                    done = true;
                } else {
                    send_input(droid, inp);
                }
                continue;
            }
            output_buffer << static_cast<char>(droid.output());
        }
    }

    void run_droid_with_user_input(computer& droid) {
        run_droid(droid,
            []()->std::string {
                std::print("> ");
                std::string inp;
                std::getline(std::cin, inp);
                return inp;
            },
            [](std::string str) {
                std::println("{}", str);
            }
        );
    }

    std::vector<std::vector<int>> get_all_subsets() {
        std::vector<std::vector<int>> sets;
        for (int v = 1; v <= 255; ++v) {
            std::vector<int> set;
            for (int i = 0; i < 8; ++i) {
                if (v & (1 << i)) {
                    set.push_back(i);
                }
            }
            sets.push_back(set);
        }
        return sets;
    }

    class try_item_set {
        std::array<std::string, 8> items_;
        std::vector<std::string> commands_;
        std::vector<std::string>::iterator iter_;
        std::string str_;
    public:
        try_item_set(const std::vector<int>& set_indices) : 
                items_({
                    "dark matter","jam","manifold","candy cane",
                    "antenna","hypercube","dehydrated water","bowl of rice"
                }) {
            std::stringstream ss;
            for (const auto& index : set_indices) {
                ss << items_.at(index) << ", ";
                commands_.push_back(std::format("take {}", items_.at(index)));
            }
            commands_.push_back("west");
            for (const auto& index : set_indices) {
                commands_.push_back(std::format("drop {}", items_.at(index)));
            }
            iter_ = commands_.begin();
            str_ = ss.str();
        }

        std::string to_string() const {
            return str_;
        }

        bool is_done() const {
            return iter_ == commands_.end();
        }
        
        std::string next_command() {
            auto cmd = *iter_;
            iter_++;
            return cmd;
        }
    };

    int64_t run_droid_autopilot(computer& droid) {
        std::vector<std::string> commands{
            "north", "take dark matter", "north", "north", "west", "take jam", "east", 
            "take manifold" , "east" , "take candy cane" , "west" , "south" , "east","south",
            "take antenna", "west","take hypercube","east","north","west","south","east","south",
            "take dehydrated water","north","east","take bowl of rice","west","west","south","west",
            "south","west","drop dark matter","drop jam","drop manifold","drop candy cane",
            "drop antenna","drop hypercube","drop dehydrated water","drop bowl of rice"
        };
        auto cmd_iter = commands.begin();
        std::vector<std::vector<int>> sets = get_all_subsets();
        auto sets_iter = sets.begin();
        std::optional<try_item_set> curr_set;
        int64_t result = -1;

        run_droid(droid,
            [&]()->std::string {
                if (cmd_iter != commands.end()) {
                    auto cmd = *cmd_iter++;
                    return cmd + '\n';
                } else {
                    if (!curr_set || curr_set->is_done()) {
                        curr_set = try_item_set(*sets_iter);
                        sets_iter++;
                    }
                    auto cmd = curr_set->next_command();
                    return cmd + '\n';
                }
            },
            [&](std::string str) {
                auto nums = aoc::extract_numbers_int64(str, false);
                if (!nums.empty() && nums.front() > 10000) {
                    result = nums.front();
                }
            }
        );

        return result;
    }
}

void aoc::y2019::day_25(const std::string& title) {

    auto program = split(
            aoc::file_to_string(aoc::input_path(2019, 25)), ','
        ) | rv::transform(
            [](auto&& str)->int64_t {
                return aoc::string_to_int64(str);
            }
        ) | r::to<std::vector>();

    computer droid(program);

    std::println("--- Day 25: {} ---", title);
    std::println("  part 1: {}",
        run_droid_autopilot(droid)
    );
    std::println("  part 2: {}",
        "<xmas freebie>"
    );
}