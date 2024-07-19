#include "../util/util.h"
#include "y2020.h"
#include <filesystem>
#include <functional>
#include <print>
#include <ranges>
#include <unordered_set>

namespace r = std::ranges;
namespace rv = std::ranges::views;

/*------------------------------------------------------------------------------------------------*/

namespace {

    int specifier_to_index(std::string::const_iterator iter, char lower_ch, int i, int j) {
        if (i == j) {
            return i;
        }
        bool forward = *iter == lower_ch;
        auto next_iter = std::next(iter);
        return (forward) ?
            specifier_to_index(next_iter, lower_ch, i, (i + j) / 2) :
            specifier_to_index(next_iter, lower_ch, (i + j) / 2 + 1, j);
    }

    int row_specifier_to_row(const std::string& row_spec) {
        return specifier_to_index(row_spec.begin(), 'F', 0, 127);
    }

    int seat_specifier_to_seat(const std::string& seat_spec) {
        return specifier_to_index(seat_spec.begin(), 'L', 0, 7);
    }

    std::tuple<int, int> specifier_to_row_and_col(const std::string& spec) {
        return {
            row_specifier_to_row(spec.substr(0,7)),
            seat_specifier_to_seat(spec.substr(7,3))
        };
    }

    int seat_id(const std::string& spec) {
        auto [row, col] = specifier_to_row_and_col(spec);
        return row * 8 + col;
    }

    int find_seat(const std::vector<std::string>& specs) {
        auto seat_ids = specs | rv::transform(seat_id) | r::to<std::unordered_set<int>>();
        auto highest = r::max(seat_ids);
        for (int id = 0; id < highest; ++id) {
            if (seat_ids.contains(id)) {
                continue;
            }
            if (seat_ids.contains(id - 1) && seat_ids.contains(id + 1)) {
                return id;
            }
        }
        return -1;
    }
}

void aoc::y2020::day_05(const std::string& title) {
    auto input = aoc::file_to_string_vector(aoc::input_path(2020, 5));

    std::println("--- Day 5: {} ---", title);
    std::println("  part 1: {}", r::max(input | rv::transform( seat_id)) );
    std::println("  part 2: {}", find_seat(input));
}