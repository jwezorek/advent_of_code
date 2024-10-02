
#include "../util/util.h"
#include "y2017.h"
#include <filesystem>
#include <functional>
#include <print>
#include <ranges>
#include <unordered_set>

namespace r = std::ranges;
namespace rv = std::ranges::views;

/*------------------------------------------------------------------------------------------------*/

namespace {
    struct tm_table_item {
        int write_val;
        bool go_left;
        std::string next_state;
    };

    using tm_table = std::unordered_map<std::string, std::array<tm_table_item, 2>>;

    struct turing_machine {
        std::string start_state;
        tm_table tbl;
    };

    std::string last_word(const std::string& str) {
        auto words = aoc::extract_alphabetic(str);
        return words.back();
    }

    tm_table::value_type parse_row(const std::vector<std::string>& lines) {
        auto state_lbl = last_word(lines[0]);
        tm_table_item item_0 = {
            .write_val = aoc::extract_numbers(lines[2]).front(),
            .go_left = lines[3].contains("left"),
            .next_state = last_word(lines[4])
        };

        tm_table_item item_1 = {
            .write_val = aoc::extract_numbers(lines[6]).front(),
            .go_left = lines[7].contains("left"),
            .next_state = last_word(lines[8])
        };

        return { state_lbl, {item_0, item_1} };
    }

    std::tuple<int, turing_machine> parse(const std::vector<std::string>& inp) {
        auto groups = aoc::group_strings_separated_by_blank_lines(inp);
        auto start_state = last_word(groups[0].front());
        auto num_steps = aoc::extract_numbers(groups[0].back()).front();

        auto tbl = groups | rv::drop(1) | rv::transform(
            parse_row
        ) | r::to<tm_table>();

        return { num_steps, {start_state, tbl} };
    }

    int run_turing_machine(const turing_machine& tm, int num_iterations) {
        std::unordered_set<int> tape;
        std::string state = tm.start_state;
        int pos = 0;
        for (int i = 0; i < num_iterations; ++i) {
            int tape_cell = tape.contains(pos) ? 1 : 0;
            const auto& tbl_cell = tm.tbl.at(state)[tape_cell];
            if (tbl_cell.write_val == 1) {
                tape.insert(pos);
            } else {
                tape.erase(pos);
            }
            pos += (tbl_cell.go_left) ? -1 : 1;
            state = tbl_cell.next_state;
        }
        return tape.size();
    }
}

void aoc::y2017::day_25(const std::string& title) {

    auto [num_steps, tm] = parse(
        aoc::file_to_string_vector(
            aoc::input_path(2017, 25)
        )
    );

    std::println("--- Day 25: {} ---", title);
    std::println("  part 1: {}", run_turing_machine(tm, num_steps));
    std::println("  part 2: {}", "<xmas freebie>");
    
}
