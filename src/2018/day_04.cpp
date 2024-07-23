
#include "../util/util.h"
#include "y2018.h"
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
    struct event_record {
        int minute;
        std::variant<int, bool> event;
    };

    event_record parse_inp_line(const std::string& str) {
        auto vals = aoc::extract_numbers(str);
        return {
            vals[4],
            (vals.size() == 6) ?
                std::variant<int, bool>{vals[5]} :
                std::variant<int, bool>{str.contains("wakes")}
        };
    }

    using guard_table = std::unordered_map<int, std::vector<int>>;
    guard_table make_guard_table(const std::vector<event_record>& events) {
        guard_table tbl;
        int curr_guard = -1;
        int falls_asleep_minute = -1;
        for (const auto& event : events) {
            if (std::holds_alternative<int>(event.event)) {
                curr_guard = std::get<int>(event.event);
                falls_asleep_minute = -1;
                continue;
            }
            auto wakes_up = std::get<bool>(event.event);
            if (!wakes_up) {
                falls_asleep_minute = event.minute;
                continue;
            }
            if (!tbl.contains(curr_guard)) {
                tbl[curr_guard] = std::vector<int>(60, 0);
            }
            for (int i = falls_asleep_minute; i < event.minute; ++i) {
                tbl[curr_guard][i]++;
            }
        }
        return tbl;
    }

    int do_part_1(const guard_table& tbl) {
        auto guard_to_sleep_amnt = tbl |
            rv::transform(
                [](auto&& pair)->std::unordered_map<int, int>::value_type {
                    auto [id, chart] = pair;
                    return { id, r::fold_left(chart, 0, std::plus<>()) };
                }
            ) | r::to<std::unordered_map<int, int>>();

        auto most_alseep_guard = r::max_element(
                guard_to_sleep_amnt,
                [](const auto& lhs, const auto& rhs)->bool {
                    const auto& [lhs_guard, lhs_amnt] = lhs;
                    const auto& [rhs_guard, rhs_amnt] = rhs;
                    return lhs_amnt < rhs_amnt;
                }
            )->first;

        auto most_asleep_minute = std::get<0>(
            *r::max_element(
                rv::enumerate(tbl.at(most_alseep_guard)),
                [](const auto& lhs, const auto& rhs)->bool {
                    const auto& [lhs_minute, lhs_sleep_count] = lhs;
                    const auto& [rhs_minute, rhs_sleep_count] = rhs;
                    return lhs_sleep_count < rhs_sleep_count;
                }
            )
        );

        return most_alseep_guard * most_asleep_minute;
    }

    int do_part_2(const guard_table& tbl) {
        auto guard_to_max_sleep_minute = tbl |
            rv::transform(
                [](auto&& pair)->std::unordered_map<int, int>::value_type {
                    auto [id, chart] = pair;
                    int minute = std::get<0>(
                        *r::max_element(
                            rv::enumerate(chart),
                            [](const auto& lhs, const auto& rhs)->bool {
                                const auto& [lhs_minute, lhs_sleep_count] = lhs;
                                const auto& [rhs_minute, rhs_sleep_count] = rhs;
                                return lhs_sleep_count < rhs_sleep_count;
                            }
                        )
                    );
                    return { id, minute };
                }
            ) | r::to<std::unordered_map<int, int>>();

        auto max_sleep_minute_guard = r::max_element(
            guard_to_max_sleep_minute,
            [&](const auto& lhs, const auto& rhs)->bool {
                const auto& [lhs_guard, lhs_minute] = lhs;
                const auto& [rhs_guard, rhs_minute] = rhs;
                return tbl.at(lhs_guard).at(lhs_minute) < tbl.at(rhs_guard).at(rhs_minute);
            }
        )->first;

        return max_sleep_minute_guard * guard_to_max_sleep_minute[max_sleep_minute_guard];
    }
}

void aoc::y2018::day_04(const std::string& title) {

    auto inp = aoc::file_to_string_vector(
        aoc::input_path(2018, 4)
    );
    
    r::sort(inp);
    auto events = inp | rv::transform(
            parse_inp_line
        ) | r::to<std::vector>();
    auto guard_table = make_guard_table(events);
    
    std::println("--- Day 4: {} ---", title);
    std::println("  part 1: {}", do_part_1(guard_table));
    std::println("  part 2: {}", do_part_2(guard_table));
    
}
