
#include "../util/util.h"
#include "y2017.h"
#include <filesystem>
#include <functional>
#include <print>
#include <ranges>
#include <unordered_map>
#include <boost/functional/hash.hpp>

namespace r = std::ranges;
namespace rv = std::ranges::views;

/*------------------------------------------------------------------------------------------------*/

namespace {

    using numbers = std::vector<int>;
    
    struct hash_nums {
        size_t operator()(const numbers& nums) const {
            size_t seed = 0;
            for (const auto n : nums) {
                boost::hash_combine(seed, n);
            }
            return seed;
        }
    };

    struct equate_nums {
        bool operator()(const numbers& lhs, const numbers& rhs) const {
            if (lhs.size() != rhs.size()) {
                return false;
            }
            for (const auto& [left, right] : rv::zip(lhs, rhs)) {
                if (left != right) {
                    return false;
                }
            }
            return true;
        }
    };

    using nums_map = std::unordered_map<numbers, int, hash_nums, equate_nums>;

    std::vector<std::tuple<int,int>> get_redistributions(int amnt, int n) {
        std::vector<std::tuple<int, int>> output;
        auto mod = amnt % n;
        if (mod != 0) {
            auto dist_amnt = amnt / n + 1;
            auto dist_count = mod;
            output.emplace_back(
                dist_amnt,
                dist_count
            );
            amnt -= dist_amnt * dist_count;
            n -= dist_count;
        } 
        if (amnt > 0) {
            output.emplace_back(
                amnt / n,
                n
            );
        }
        return output;
    }

    void redistribute(numbers& nums) {
        auto iter = r::max_element(nums);
        auto amnt = *iter;
        *(iter++) = 0;
        auto distributions = get_redistributions(amnt, static_cast<int>(nums.size()));
        for (const auto& [dist_amnt, dist_count] : distributions) {
            for (int i = 0; i < dist_count; ++i) {
                if (iter == nums.end()) {
                    iter = nums.begin();
                }
                *(iter++) += dist_amnt;
            }
        }
    }
    
    std::tuple<int,int> num_nonrepearing_redistributions( numbers nums) {
        nums_map nums_seen;

        int count = 0;
        while (!nums_seen.contains(nums)) {
            nums_seen[nums] = count;
            redistribute(nums);
            ++count;
        }

        return { count, count - nums_seen[nums] };
    }

}

void aoc::y2017::day_06(const std::string& title) {

    auto inp = aoc::extract_numbers(
        aoc::file_to_string(
            aoc::input_path(2017, 6)
        ),
        true
    );

    std::println("--- Day 6: {} ---", title);

    auto [first_repeat_count, size_of_loop] = num_nonrepearing_redistributions( inp );
    std::println("  part 1: {}", first_repeat_count);
    std::println("  part 2: {}", size_of_loop);
    
}
