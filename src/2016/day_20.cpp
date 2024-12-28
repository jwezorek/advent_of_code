
#include "../util/util.h"
#include "y2016.h"
#include <filesystem>
#include <functional>
#include <print>
#include <ranges>
#include <unordered_set>
#include <boost/icl/interval_set.hpp>

namespace r = std::ranges;
namespace rv = std::ranges::views;

/*------------------------------------------------------------------------------------------------*/

namespace {
    using interval = boost::icl::discrete_interval<uint32_t>;
    using interval_set = boost::icl::interval_set<uint32_t>;

    interval parse_interval(const std::string& str) {
        auto values = aoc::extract_numbers_int64(str);
        return interval::closed(
            static_cast<uint32_t>(values[0]), 
            static_cast<uint32_t>(values[1])
        );
    }

    interval make_closed(const interval& input_interval) {
        auto bounds = input_interval.bounds();

        uint32_t lower = input_interval.lower();
        uint32_t upper = input_interval.upper();

        if (bounds.left() == boost::icl::interval_bounds::open()) {
            lower += 1; 
        }
        if (bounds.right() == boost::icl::interval_bounds::open()) {
            upper -= 1; 
        }

        return interval::closed(lower, upper);
    }

    interval_set complement_of_intervals(const std::vector<interval>& ints) {
        constexpr auto max = std::numeric_limits<uint32_t>::max();
        interval_set set;
        for (auto&& interval : ints) {
            set.add(interval);
        }
        interval full_range = interval::closed(0, max);
        auto complement = interval_set(full_range) - set;

        return complement | rv::transform(make_closed) | r::to<interval_set>();
    }

    uint32_t lowest_number_not_in_intervals(const std::vector<interval>& ints) {
        auto complement = complement_of_intervals(ints);
        return lower( *complement.begin() );
    }

    uint32_t interval_size(const interval& input_interval) {
        return input_interval.upper() - input_interval.lower() + 1;
    }

    uint32_t size_of_complement(const std::vector<interval>& ints) {
        auto complement = complement_of_intervals(ints);
        return r::fold_left(
            complement | rv::transform(interval_size),
            static_cast<uint32_t>(0),
            std::plus<>()
        );
    }
}

void aoc::y2016::day_20(const std::string& title) {

    auto inp = aoc::file_to_string_vector(
            aoc::input_path(2016, 20)
        ) | rv::transform(
            [](auto&& str) {
                return parse_interval(str);
            }
        ) | r::to<std::vector>();

    std::println("--- Day 20: {} ---", title);
    std::println("  part 1: {}", lowest_number_not_in_intervals(inp) );
    std::println("  part 2: {}", size_of_complement(inp) );
}
