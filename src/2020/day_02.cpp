#include "../util/util.h"
#include "y2020.h"
#include <filesystem>
#include <functional>
#include <print>
#include <ranges>
#include <unordered_map>

namespace r = std::ranges;
namespace rv = std::ranges::views;

/*------------------------------------------------------------------------------------------------*/

namespace {

    struct password_policy {
        int min;
        int max;
        char ch;
        std::string pwd;
    };

    password_policy parse_pwd_policy(const std::string& str) {
        auto fields = aoc::split(str, ' ');
        auto range = aoc::extract_numbers(fields[0]);
        return {
            range.front(),
            range.back(),
            fields[1].front(),
            fields[2]
        };
    }

    bool is_valid_part1(const password_policy& policy) {
        auto count = r::count(policy.pwd, policy.ch);
        return count >= policy.min && count <= policy.max;
    }

    bool is_valid_part2(const password_policy& policy) {
        auto a = policy.pwd[policy.min - 1];
        auto b = policy.pwd[policy.max - 1];
        auto c = policy.ch;
        return (a == c && b != c) || (b == c && a != c);
    }

    using validator = std::function<bool(const password_policy&)>;
    int count_valid(const std::vector<password_policy>& pwds, validator validate) {
        return r::fold_left(
            pwds | rv::transform(
                [validate](auto&& pwd_pol)->int {
                    return validate(pwd_pol) ? 1 : 0;
                }
            ),
            0,
            std::plus<>()
        );
    }
}

void aoc::y2020::day_02(const std::string& title) {
    auto input = aoc::file_to_string_vector(aoc::input_path(2020, 2)) | rv::transform(
        parse_pwd_policy
    ) | r::to<std::vector<password_policy>>();

    std::println("--- Day 2: {} ---", title);
    std::println("  part 1: {}", count_valid( input, is_valid_part1 ));
    std::println("  part 2: {}", count_valid( input, is_valid_part2 ));
}