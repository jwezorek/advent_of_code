#include "../util.h"
#include "y2015.h"
#include <filesystem>
#include <functional>
#include <print>
#include <ranges>
#include <unordered_set>

namespace r = std::ranges;
namespace rv = std::ranges::views;

/*------------------------------------------------------------------------------------------------*/

namespace {
    using set_token = uint32_t;

    int hamming_weight(uint32_t x) {
        int count = 0;
        while (x) {
            count += x & 1;
            x >>= 1;
        }
        return count;
    }

    std::vector<set_token> subsets_with_correct_sum_and_size(
        const std::vector<int> nums, int sum) {
        auto n = static_cast<set_token>(nums.size());
        std::vector<set_token> subsets;
        int min_set_sz = std::numeric_limits<int>::max();
        for (set_token tok = 0; tok < (1 << n); ++tok) {
            set_token subset_sum = 0;
            int set_sz = 0;
            for (set_token j = 0; j < n; ++j) {
                subset_sum += ((1 << j) & tok) ? nums[j] : 0;
                set_sz += ((1 << j) & tok) ? 1 : 0;
                if (subset_sum > sum) {
                    continue;
                }
            }
            if (sum == subset_sum) {
                subsets.push_back(tok);
                min_set_sz = std::min(set_sz, min_set_sz);
            }
        }
        int max_subset_sz = n - 2 * min_set_sz;
        return subsets |
            rv::filter(
                [max_subset_sz](auto subset) {
                    return hamming_weight(subset) <= max_subset_sz;
                }
            ) | r::to<std::vector>();
    }

    std::vector<std::array<set_token, 3>> subset_triples(const std::vector<set_token>& sets, int n) {
        std::vector<std::array<set_token, 3>> output;
        output.reserve(500000);
        for (auto [i, j] : aoc::two_combos_indices(sets.size())) {
            if ((sets[i] & sets[j]) == 0) {
                auto set_union = sets[i] | sets[j];
                auto complement = ~set_union;
                auto mask = (static_cast<set_token>(1) << n) - 1;
                output.push_back( {{sets[i], sets[j], complement & mask}} );
            }
        }
        output.shrink_to_fit();
        return output;
    }

    std::vector<int> expand_set_token(const std::vector<int>& nums, set_token tok) {
        std::vector<int> set;
        for (int i = 0; i < nums.size(); ++i) {
            if ((1 << i) & tok) {
                set.push_back(nums[i]);
            }
        }
        return set;
    }

    std::string set_token_to_str(const std::vector<int>& nums, set_token tok) {
        std::vector<int> set = expand_set_token(nums, tok);
        std::stringstream ss;
        ss << "{ ";
        int n = static_cast<int>(set.size()) - 1;
        for (int i = 0; i <= n; ++i) {
            ss << set[i];
            if (i != n) {
                ss << ",";
            }
            ss << " ";
        }
        ss << "}";
        return ss.str();
    }

    int smallest_set_size(const std::vector<std::array<set_token, 3>>& triples) {
        return r::min(
            triples | rv::transform(
                 [](const auto& triple)->int {
                    return r::min(
                        triple | rv::transform(
                            [](auto&& set)->int {
                                return hamming_weight(set);
                            }
                        )
                    );
                 }
             )
        );
    }

    std::vector<set_token> smallest_sets(const std::vector<std::array<set_token, 3>>& triples) {
        auto small_sz = smallest_set_size(triples);
        std::unordered_set<set_token> small_sets;
        for (const auto& triple : triples) {
            for (auto set : triple) {
                if (hamming_weight(set) == small_sz) {
                    small_sets.insert(set);
                }
            }
        }
        return small_sets | r::to<std::vector>();
    }

    int64_t smallest_quantum_entanglement(
            const std::vector<int>& nums, const std::vector<set_token>& sets) {
        return r::min(
            sets | rv::transform(
                [&](auto tok) {
                    auto set = expand_set_token(nums, tok);
                    return r::fold_left(
                        set,
                        static_cast<int64_t>(1),
                        std::multiplies<int64_t>()
                    );
                }
            )
        );
    }
}

void aoc::y2015::day_24(const std::string& title) {

    auto nums = aoc::file_to_string_vector(
            aoc::input_path(2015, 24)
        ) | rv::transform(
            [](auto&& str) {
                return std::stoi(str);
            }
        ) | r::to<std::vector>();

    auto subset_sum = r::fold_left(nums, 0, std::plus<>()) / 3;
    auto subsets = subsets_with_correct_sum_and_size(nums, subset_sum);
    auto triples = subset_triples(subsets, nums.size());
    auto small_sets = smallest_sets(triples);

    std::println("--- Day 24: {} ---", title);
    std::println("  part 1: {}",
        smallest_quantum_entanglement(nums, small_sets)
    );
    std::println("  part 2: {}",
        0
    );
}