
#include "../util/util.h"
#include "y2025.h"
#include <filesystem>
#include <functional>
#include <print>
#include <ranges>
#include <tuple>
#include <boost/icl/interval_set.hpp>
#include <boost/icl/interval.hpp>

namespace r = std::ranges;
namespace rv = std::ranges::views;
namespace icl = boost::icl;


/*------------------------------------------------------------------------------------------------*/

namespace {

    struct id_range {
        int64_t from;
        int64_t to;
    };

    std::tuple<std::vector< id_range>, std::vector<int64_t>> parse_input(
            const std::vector<std::string>& inp) {
        auto parts = aoc::group_strings_separated_by_blank_lines(inp);
        return {
            parts.front() | rv::transform(
                    [](auto&& line)->id_range {
                        auto vals = aoc::extract_numbers_int64(line);
                        return { vals[0], vals[1] };
                    }
                ) | r::to<std::vector>(),
            parts.back() |
                rv::transform(
                    aoc::string_to_int64
                ) | r::to<std::vector>()
        };
    }

    bool in_some_range(const std::vector<id_range>& ranges, int64_t id) {
        for (const auto& range : ranges) {
            if (id >= range.from && id <= range.to) {
                return true;
            }
        }
        return false;
    }

    int brute_force_part_1(const std::vector<id_range>& ranges, const std::vector<int64_t>& ids) {
        return r::count_if(
            ids,
            [&](auto id) {
                return in_some_range(ranges, id);
            }
        );
    }

    int64_t total_covered_ids(const std::vector<id_range>& ranges) {

        icl::interval_set<int64_t> flattened;

        for (const auto& r : ranges) {
            flattened.insert(icl::interval<int64_t>::closed(r.from, r.to));
        }

        int64_t total = 0;
        for (const auto& inter : flattened) {
            total += (inter.upper() - inter.lower() + 1);
        }

        return total;
    }

}

void aoc::y2025::day_05(const std::string& title) {

    auto [id_ranges, ids] = parse_input(
        aoc::file_to_string_vector(
            aoc::input_path(2025, 5)
        )
    );

    std::println("--- Day 5: {} ---", title);
    std::println("  part 1: {}", brute_force_part_1(id_ranges, ids) );
    std::println("  part 2: {}", total_covered_ids(id_ranges) );
    
}
