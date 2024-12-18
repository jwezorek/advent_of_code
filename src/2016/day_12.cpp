
#include "../util/util.h"
#include "y2016.h"
#include <filesystem>
#include <functional>
#include <print>
#include <ranges>
#include <unordered_set>
#include <variant>
#include <unordered_map>

namespace r = std::ranges;
namespace rv = std::ranges::views;

/*------------------------------------------------------------------------------------------------*/

namespace {
    enum op {
        cpy,
        inc,
        dec,
        jnz
    };

    using operand = std::variant<int64_t, char>;

    struct statement {
        op op;
        std::vector<operand> args;
    };

    struct state {
        int instr_ptr;
        std::unordered_map<char, int64_t> registers;
    };

    operand str_to_operand(const std::string& str) {
        if (std::isalpha(str.front())) {
            return { str.front() };
        }
        return aoc::string_to_int64(str);
    }

    statement parse_statement(const std::string& str) {
        static const std::unordered_map<std::string, op> op_tbl = {
            {"cpy", cpy},
            {"inc", inc},
            {"dec", dec},
            {"jnz", jnz}
        };

        auto parts = str | rv::split(' ') | rv::transform(
            [](auto rng) {
                return rng | r::to<std::string>();
            }
        ) | r::to<std::vector>();

        return {
            op_tbl.at(parts.front()),
            parts | rv::drop(1) | rv::transform(
                str_to_operand
            ) | r::to<std::vector>()
        };
    }
}

void aoc::y2016::day_12(const std::string& title) {

    auto inp = aoc::file_to_string_vector(
            aoc::input_path(2016, 12)
        ) | rv::transform(
            parse_statement
        ) | r::to<std::vector>();

    std::println("--- Day 12: {} ---", title);
    std::println("  part 1: {}", 0);
    std::println("  part 2: {}", 0);
    
}
