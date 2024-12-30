
#include "../util/util.h"
#include "y2016.h"
#include <filesystem>
#include <functional>
#include <print>
#include <ranges>
#include <unordered_set>
#include <variant>

namespace r = std::ranges;
namespace rv = std::ranges::views;

/*------------------------------------------------------------------------------------------------*/

namespace {

    bool test(int64_t a) {

        int out_index = 0;

        int64_t b = 0;
        int64_t c = 0;
        int64_t d = 0;

        d = a;
        c = 9;
    line_2: b = 282;
    line_1: ++d;
        --b;
        if (b != 0) goto line_1;
        --c;
        if (c != 0) goto line_2;
    line_11: a = d;
    line_10: std::print("");
        b = a;
        a = 0;
    line_6: c = 2;
    line_5: if (b != 0) goto line_3;
        goto line_4;
    line_3: --b;
        --c;
        if (c != 0) goto line_5;
        ++a;
        goto line_6;
    line_4: b = 2;
    line_9: if (c != 0) goto line_7;
        goto line_8;
    line_7: --b;
        --c;
        goto line_9;
    line_8: std::print("");
        if (out_index % 2 == b) {
            out_index++;
            if (out_index > 100) {
                return true;
            }
        } else {
            return false;
        }
        if (a != 0) goto line_10;
        goto line_11;
    }

    int do_part_1() {
        return *r::find_if(rv::iota(0), test);
    }
}


void aoc::y2016::day_25(const std::string& title) {

    std::println("--- Day 25: {} ---", title);
    std::println("  part 1: {}", do_part_1());
    std::println("  part 2: {}", "<xmas freebie>");
    
}
