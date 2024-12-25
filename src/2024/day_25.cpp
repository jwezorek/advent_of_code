
#include "../util/util.h"
#include "y2024.h"
#include <filesystem>
#include <functional>
#include <print>
#include <ranges>
#include <unordered_set>

namespace r = std::ranges;
namespace rv = std::ranges::views;

/*------------------------------------------------------------------------------------------------*/

namespace {
    using pins = std::vector<int>;

    pins to_pins(const std::vector<std::string>& group) {
        return rv::iota(0, 5) | rv::transform(
            [&group](int col)->int {
                return static_cast<int>(
                    r::count_if(
                        rv::iota(0ull, group.size()) | rv::transform(
                            [&group, col](size_t row)->char {
                                return group[row][col];
                            }
                        ),
                        [](char tile) {
                            return tile == '#';
                        }
                    )
                );
            }
        ) | r::to<std::vector>();
    }

    std::tuple<std::vector<pins>, std::vector<pins>> parse_input(
            const std::vector<std::string>& inp) {
        auto groups = aoc::group_strings_separated_by_blank_lines(inp);
        std::vector<pins> locks;
        std::vector<pins> keys;
        for (const auto& group : groups) {
            auto is_lock = group.front().front() == '#';
            if (is_lock) {
                locks.push_back(to_pins(group));
            } else {
                keys.push_back(to_pins(group));
            }
        }
        return { std::move(locks), std::move(keys) };
    }

    bool lock_fits_key(const pins& lock, const pins& key) {
        auto zipped = rv::zip(lock, key);
        return r::find_if(
            zipped,
            [](auto&& pair)->bool {
                auto [lock_col, key_col] = pair;
                return lock_col + key_col > 7;
            }
        ) == zipped.end();
    }
}

void aoc::y2024::day_25(const std::string& title) {

    auto [locks, keys] =  parse_input(
            aoc::file_to_string_vector(
                aoc::input_path(2024, 25)
            )
        );

    std::println("--- Day 25: {} ---", title);
    std::println("  part 1: {}",
        static_cast<int64_t>(
            r::count_if(
                rv::cartesian_product(locks, keys),
                [](const auto& lock_and_key)->bool {
                    const auto& [lock, key] = lock_and_key;
                    return lock_fits_key(lock, key);
                }
            )
        )
    );

    std::println("  part 2: {}", "<xmas freebie>" );
    
}
