#include "days.h"
#include "../util.h"
#include <print>
#include <ranges>
#include <numeric>
#include <algorithm>

namespace r = std::ranges;
namespace rv = std::ranges::views;

namespace {

    struct record {
        std::string row;
        std::vector<int> groups;
    };

    std::vector<record> parse_input(const std::vector<std::string>& lines) {
        return lines | rv::transform(
                [](const std::string& line)->record {
                    auto parts = aoc::split(line, ' ');
                    return {
                        parts.front(),
                        aoc::extract_numbers(parts.back())
                    };
                }
            ) | r::to<std::vector<record>>();
    }

    bool check_row(const std::string& row, std::vector<int> groups) {
        if (row.find('?') != std::string::npos) {
            throw std::runtime_error("row not assigned");
        }
        auto new_row = row;
        r::replace(new_row, '.', ' ');
        new_row = aoc::collapse_whitespace(new_row);
        auto row_groups = aoc::split(new_row, ' ');
        if (row_groups.size() != groups.size()) {
            return false;
        }
        auto pairs = rv::zip(row_groups, groups);
        return r::find_if(pairs,
                [](const auto& pair)->bool {
                    auto [str, sz] = pair;
                    return str.size() != sz;
                }
            ) == pairs.end();
    }

    std::string assignment(int n, unsigned int index) {
        std::string output(n, '.');
        for (auto i = 0; i < n; ++i) {
            if (index & (1 << i)) {
                output[i] = '#';
            }
        }
        return output;
    }

    int max_index(int n) {
        return (1 << n);
    }

    std::string assign(const std::string& string_with_unknowns, const std::string& assignment) {
        auto output = string_with_unknowns;
        auto iter = assignment.begin();
        for (auto out_iter = output.begin(); out_iter != output.end(); ++out_iter) {
            if (*out_iter == '?') {
                *out_iter = *(iter++);
            }
        }
        return output;
    }

    int number_of_unknowns(const std::string& input) {
        return r::fold_left(
            input | rv::transform(
                [](char ch)->int {
                    return ch == '?' ? 1 : 0;
                }
            ),
            0,
            std::plus<>()
        );
    }

    int count_valid_assignments(const record& rec) {
        int num_unknowns = number_of_unknowns(rec.row);
        return r::fold_left(
            rv::iota(0, max_index(num_unknowns)) |
            rv::transform(
                [&](auto index)->int {
                    auto test = assignment(num_unknowns, index);
                    auto filled_in = assign(rec.row, test);
                    return check_row(filled_in, rec.groups) ? 1 : 0;
                }
            ),
            0,
            std::plus<>()
        );
    }
}

/*------------------------------------------------------------------------------------------------*/

void aoc::y2023::day_12(const std::string& title) {

    auto input = parse_input(aoc::file_to_string_vector(aoc::input_path(2023, 12)));

    std::println("--- Day 12: {0} ---\n", title);

    std::println("  part 1: {}",
        r::fold_left(
            input | rv::transform(count_valid_assignments),
            0,
            std::plus<>()
        )
    );

}