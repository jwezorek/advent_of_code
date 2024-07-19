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

    using char_set = std::unordered_set<char>;

    int count_anyone_answered_yes(const std::vector<std::string>& group) {
        auto set = group | rv::join | r::to<char_set>();
        return static_cast<int>(set.size());
    }

    
    std::unordered_set<char> intersect(const char_set& lhs, const char_set& rhs) {
        return lhs |
            rv::filter(
                [&rhs](char lhs_ch) {
                    return rhs.contains(lhs_ch);
                }
        ) | r::to<char_set>();
    }

    int count_everyone_answered_yes(const std::vector<std::string>& group) {
        auto intersection = r::fold_left_first(
            group | rv::transform(
                [](auto&& str)->char_set {
                    return str | r::to<char_set>();
                }
            ),
            intersect
        );
        if (!intersection) {
            return 0;
        }
        return static_cast<int>(intersection->size());
    }

}

void aoc::y2020::day_06(const std::string& title) {
    auto input = aoc::group_strings_separated_by_blank_lines(
        aoc::file_to_string_vector( aoc::input_path(2020, 6) )
    );

    std::println("--- Day 6: {} ---", title);
    std::println("  part 1: {}", 
        r::fold_left(
            input | rv::transform(count_anyone_answered_yes),
            0,
            std::plus<>()
        )
    );
    std::println("  part 2: {}",
        r::fold_left(
            input | rv::transform(count_everyone_answered_yes),
            0,
            std::plus<>()
        )
    );
}