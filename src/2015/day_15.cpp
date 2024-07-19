#include "../util/util.h"
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

    constexpr int k_calories = 4;

    bool get_first_composition(int n, std::vector<int>& composition) {
        auto k = static_cast<int>(composition.size());
        if (n < k) {
            return false;
        }
        for (int i = 0; i < k - 1; i++) {
            composition[i] = 1;
        }
        composition[k - 1] = n - k + 1;
        return true;
    }

    bool get_next_composition(int n, std::vector<int>& composition) {
        auto k = static_cast<int>(composition.size());
        if (composition[0] == n - k + 1) {
            return false;
        }
        int last = k - 1;
        while (composition[last] == 1) {
            last--;
        }

        int z = composition[last];
        composition[last - 1] += 1;
        composition[last] = 1;
        composition[k - 1] = z - 1;
        return true;
    }

    using ingredient = std::vector<int>;

    ingredient str_to_ingredient(const std::string& str) {
        return aoc::extract_numbers(str, true);
    }

    int score_recipe(const std::vector<ingredient>& ingredients, const std::vector<int>& amounts) {
        return r::fold_left(
            rv::iota(0, 4) |
            rv::transform(
                [&](int prop)->int {
                    auto value = r::fold_left(
                        rv::zip(ingredients, amounts) | rv::transform(
                            [&](auto&& tup)->int {
                                const auto& [ingr, amt] = tup;
                                return ingr[prop] * amt;
                            }
                        ),
                        0,
                        std::plus<>()
                    );
                    return std::max(value, 0);
                }
            ),
            1,
            std::multiplies<>()
        );
    }

    int calory_total(const std::vector<ingredient>& ingredients, const std::vector<int>& amounts) {
        return r::fold_left(
            rv::zip(ingredients, amounts) | rv::transform(
                [](auto&& tup)->int {
                    const auto& [ingr, amt] = tup;
                    return ingr.at(k_calories) * amt;
                }
            ),
            0,
            std::plus<>()
        );
    }

    int highest_scoring_cookie(const std::vector<ingredient>& ingredients) {
        std::vector<int> recipe(4, 0);
        int max = 0;
        for (bool exists = get_first_composition(100, recipe);
                exists;
                exists = get_next_composition(100, recipe)) {
            max = std::max(max, score_recipe(ingredients, recipe));
        }
        return max;
    }

    int highest_scoring_500_calory_cookie(const std::vector<ingredient>& ingredients) {
        std::vector<int> recipe(4, 0);
        int max = 0;
        for (bool exists = get_first_composition(100, recipe);
                exists;
                exists = get_next_composition(100, recipe)) {
            if (calory_total(ingredients, recipe) != 500) {
                continue;
            }
            max = std::max(max, score_recipe(ingredients, recipe));
        }
        return max;
    }
}

void aoc::y2015::day_15(const std::string& title) {

    auto inp = aoc::file_to_string_vector(
            aoc::input_path(2015, 15)
        ) | rv::transform(
            str_to_ingredient
        ) | r::to<std::vector>();

    std::println("--- Day 15: {} ---", title);
    std::println("  part 1: {}",
        highest_scoring_cookie(inp)
    );
    std::println("  part 2: {}",
        highest_scoring_500_calory_cookie(inp)
    );
}