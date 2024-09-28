
#include "../util/util.h"
#include "y2017.h"
#include <filesystem>
#include <functional>
#include <print>
#include <ranges>

namespace r = std::ranges;
namespace rv = std::ranges::views;

/*------------------------------------------------------------------------------------------------*/

namespace {

    int do_part_1(int step_sz) {
        std::vector<int> buffer = { 0 };
        int pos = 0;
        for (int i = 1; i <= 2017; ++i) {
            int insert_pos = (pos + step_sz) % buffer.size();
            buffer.insert(buffer.begin() + insert_pos + 1, i);
            pos = insert_pos + 1;
        }
        return buffer[(pos+1) % buffer.size()];
    }

    struct spinlock_state {
        int position;
        int buffer_sz;
        int target;
    };

    void update_state(spinlock_state& state, int val, int step_sz) {
        int insert_position = (state.position + step_sz) % state.buffer_sz;
        if (insert_position == 0) {
            state.target = val;
        }
        ++state.buffer_sz;
        state.position = (insert_position + 1) % state.buffer_sz;
    }

    int do_part_2(int step_sz) {
        spinlock_state state{ 0, 1, 0 };
        for (int i = 1; i <= 50000000; ++i) {
            update_state(state, i, step_sz);
        }
        return state.target;
    }
}

void aoc::y2017::day_17(const std::string& title) {

    auto inp = std::stoi(
        aoc::file_to_string(
            aoc::input_path(2017, 17)
        )
    );

    std::println("--- Day 17: {} ---", title);
    std::println("  part 1: {}", do_part_1(inp));
    std::println("  part 2: {}", do_part_2(inp));
    
}
