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
    bool get_first_composition(int n, int k, std::vector<int>& composition)
    {
        if (n < k) {
            return false;
        }
        for (int i = 0; i < k - 1; i++) {
            composition[i] = 1;
        }
        composition[k - 1] = n - k + 1;
        return true;
    }

    bool get_next_composition(int n, int k, std::vector<int>& composition)
    {
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

    void display_composition(const std::vector<int>& composition)
    {
        std::print("[ ");
        for (auto i : composition | rv::take(composition.size() - 1)) {
            std::print("{} , ", i);
        }
        std::println("{} ]", composition.back());
    }


    void display_all_compositions(int n, int k)
    {
        std::vector<int> composition(k, 0);  
        for (bool exists = get_first_composition(n, k, composition);
                exists;
                exists = get_next_composition(n, k, composition)) {
            display_composition(composition);
        }
    }

    struct ingredient {
        int capacity;
        int durability;
        int flavor;
        int texture;
        int calories;
    };

    ingredient str_to_ingredient(const std::string& str) {
        auto amounts = aoc::extract_numbers(str, true);
        return {
            amounts[0],
            amounts[1],
            amounts[2],
            amounts[3],
            amounts[4],
        };
    }

    int score_recipe(const std::vector<ingredient>& ingredients, const std::vector<int>& amounts) {
        auto capacity = r::fold_left(
            rv::zip(ingredients, amounts) | rv::transform(
                [](auto&& tup)->int {
                    const auto& [ingr, amt] = tup;
                    return ingr.capacity * amt;
                }
            ),
            0,
            std::plus<>()
        );  
        auto durability = r::fold_left(
            rv::zip(ingredients, amounts) | rv::transform(
                [](auto&& tup)->int {
                    const auto& [ingr, amt] = tup;
                    return ingr.durability * amt;
                }
            ),
            0,
            std::plus<>()
        );
        auto flavor = r::fold_left(
            rv::zip(ingredients, amounts) | rv::transform(
                [](auto&& tup)->int {
                    const auto& [ingr, amt] = tup;
                    return ingr.flavor * amt;
                }
            ),
            0,
            std::plus<>()
        );
        auto texture = r::fold_left(
            rv::zip(ingredients, amounts) | rv::transform(
                [](auto&& tup)->int {
                    const auto& [ingr, amt] = tup;
                    return ingr.texture * amt;
                }
            ),
            0,
            std::plus<>()
        );

        if (capacity < 0) {
            capacity = 0;
        }

        if (durability < 0) {
            durability = 0;
        }

        if (flavor < 0) {
            flavor = 0;
        }

        if (texture < 0) {
            texture = 0;
        }

        return capacity * durability * flavor * texture;
    }

    int calory_total(const std::vector<ingredient>& ingredients, const std::vector<int>& amounts) {
        return r::fold_left(
            rv::zip(ingredients, amounts) | rv::transform(
                [](auto&& tup)->int {
                    const auto& [ingr, amt] = tup;
                    return ingr.calories * amt;
                }
            ),
            0,
            std::plus<>()
        );
    }

    int highest_scoring_cookie(const std::vector<ingredient>& ingredients) {
        std::vector<int> composition(4, 0);
        int max = 0;
        for (bool exists = get_first_composition(100, 4, composition);
                exists;
                exists = get_next_composition(100, 4, composition)) {
            int score = score_recipe(ingredients, composition);
            max = std::max(max, score);
        }
        return max;
    }

    int highest_scoring_500_calory_cookie(const std::vector<ingredient>& ingredients) {
        std::vector<int> composition(4, 0);
        int max = 0;
        for (bool exists = get_first_composition(100, 4, composition);
                exists;
                exists = get_next_composition(100, 4, composition)) {
            if (calory_total(ingredients, composition) != 500) {
                continue;
            }
            int score = score_recipe(ingredients, composition);
            max = std::max(max, score);
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