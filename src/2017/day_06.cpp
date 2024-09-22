
#include "../util/util.h"
#include "y2017.h"
#include <filesystem>
#include <functional>
#include <print>
#include <ranges>
#include <unordered_set>
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

    using nums_set = std::unordered_set<numbers, hash_nums, equate_nums>;

    struct redistribution {
        int amnt;
        int count;
    };

    std::vector<redistribution> get_redistributions(int amnt, int n) {
        std::vector<redistribution> output;
        auto mod = amnt % n;
        if (mod != 0) {
            output.emplace_back(
                amnt / n + 1,
                mod
            );
            amnt -= output.back().amnt * output.back().count;
            n -= output.back().count;
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
        for (const auto& dist : distributions) {
            for (int i = 0; i < dist.count; ++i) {
                if (iter == nums.end()) {
                    iter = nums.begin();
                }
                *(iter++) += dist.amnt;
            }
        }
    }
    
    int num_nonrepearing_redistributions( numbers nums) {
        nums_set nums_seen;

        int count = 0;
        while (!nums_seen.contains(nums)) {
            nums_seen.insert(nums);
            redistribute(nums);
            ++count;
        }

        return count;
    }

}

void test_redist(int amnt, int n) {
    auto redist = get_redistributions(amnt, n);
    std::print("({} , {}) => ", amnt, n);
    for (const auto red : redist) {
        std::print("[ amnt: {}, count: {}] ", red.amnt, red.count);
    }
    std::println("");
}

void aoc::y2017::day_06(const std::string& title) {

    auto inp = aoc::extract_numbers(
        aoc::file_to_string(
            aoc::input_path(2017, 6)
        ),
        true
    );

    std::println("--- Day 6: {} ---", title);
    std::println("  part 1: {}", num_nonrepearing_redistributions( inp ));
    std::println("  part 2: {}", 0);
    
}
