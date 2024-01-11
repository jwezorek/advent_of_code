#include "../util.h"
#include "y2020.h"
#include <filesystem>
#include <functional>
#include <print>
#include <ranges>
#include <unordered_set>
#include <unordered_map>

namespace r = std::ranges;
namespace rv = std::ranges::views;

/*------------------------------------------------------------------------------------------------*/

namespace {
    using set = std::unordered_set<std::string>;
    using map = std::unordered_map<std::string, std::string>;

    struct food {
        set ingredients;
        set allergens;
    };

    food parse_food(const std::string& str) {
        auto [lhs, rhs] = aoc::split_to_tuple<2>(str, '(');
        return {
            aoc::extract_alphabetic(lhs) | r::to<set>(),
            aoc::extract_alphabetic(rhs) | rv::drop(1) | r::to<set>()
        };
    }

    set intersect(const set& lhs, const set& rhs) {
        return lhs | rv::filter(
            [&rhs](auto&& lhs_ele) {return rhs.contains(lhs_ele); }
        ) | r::to<set>();
    }

    set union_(const set& lhs, const set& rhs) {
        std::array<decltype(rv::all(lhs)), 2> ary = {
            rv::all(lhs), rv::all(rhs)
        };
        return ary | rv::join | r::to<set>();
    }

    set allergen_set(const std::vector<food>& foods) {
        return foods | rv::transform(
            [](auto&& food) {
                return rv::all(food.allergens);
            } 
        ) | rv::join | r::to<set>();
    }

    set ingredient_set(const std::vector<food>& foods) {
        return foods | rv::transform(
            [](auto&& food) {
                return rv::all(food.ingredients);
            }
        ) | rv::join | r::to<set>();
    }

    struct pair {
        std::string ingredient;
        std::string allergen;
    };

    std::optional<pair> find_ingred_allergen_pair(
            const std::vector<food>& all_foods) {
        auto allergens = allergen_set(all_foods);
        auto ingredients = ingredient_set(all_foods);
        for (auto allergen : allergens) {
            auto foods = all_foods | rv::filter(
                    [allergen](auto&& food) {
                        return food.allergens.contains(allergen);
                    }
                ) | r::to<std::vector<food>>();
            auto intersection = r::fold_left( 
                foods | rv::transform(
                    [](auto&& f) {return f.ingredients; }
                ),
                ingredients,
                intersect 
            ); 
            if (intersection.size() == 1) {
                return pair{ 
                    *intersection.begin(), 
                    allergen 
                };
            }
        }
        return {};
    }

    food remove_ingred_and_allergen(const food& f,
            const std::string& ingred, const std::string& allergen) {
        auto food = f;
        food.allergens.erase(allergen);
        food.ingredients.erase(ingred);
        return food;
    }

    std::vector<food> remove_ingred_and_allergen(
            const std::vector<food>& foods, const std::string& ingred, const std::string& allergen) {
        return foods |
            rv::transform(
                [&](auto&& f) {
                    return remove_ingred_and_allergen(f, ingred, allergen);
                }
            ) | rv::filter(
                [](auto&& f) {
                    return !f.allergens.empty() || !f.ingredients.empty();
                }
            ) | r::to<std::vector<food>>();
    }

    set find_ingreds_without_allergens(const std::vector<food>& all_foods, map& allergen_to_ingred) {

        auto foods = all_foods;
        bool done = false;
        std::optional<pair> new_mapping;

        do {
            new_mapping = find_ingred_allergen_pair(foods);
            if (new_mapping) {
                allergen_to_ingred[new_mapping->allergen] = new_mapping->ingredient;
                foods = remove_ingred_and_allergen(foods, new_mapping->ingredient, new_mapping->allergen);
            }
        } while (new_mapping);

        return r::fold_left(
            foods | rv::transform([](auto&& f) {return f.ingredients; }),
            set{},
            union_
        );
    }

    int count_ingred_usage(const std::vector<food>& foods, const set& ingredients) {
        return r::fold_left(
            foods | rv::transform(
                [&](auto&& food) {
                    return r::count_if(
                        food.ingredients,
                        [&](auto&& i) {
                            return ingredients.contains(i);
                        }
                    );
                }
            ),
            0,
            std::plus<>()
        );
    }

    std::string dangerous_ingredient_list(const map& allergen_to_ingred) {
        auto allergens = allergen_to_ingred | rv::keys | r::to<std::vector<std::string>>();
        r::sort(allergens);
        auto ingreds = allergens | rv::transform(
                [&](auto&& allergen) { return allergen_to_ingred.at(allergen); }
            ) | r::to<std::vector<std::string>>();
        return ingreds | rv::join_with(',') | r::to<std::string>();
    }
}

void aoc::y2020::day_21(const std::string& title) {
    auto input = aoc::file_to_string_vector(aoc::input_path(2020, 21)) |
        rv::transform(parse_food) | r::to<std::vector<food>>();

    map allergen_to_ingred;
    auto ingreds = find_ingreds_without_allergens(input, allergen_to_ingred);

    std::println("--- Day 21: {} ---", title);
    std::println("  part 1: {}", count_ingred_usage(input, ingreds));
    std::println("  part 2: {}", dangerous_ingredient_list(allergen_to_ingred));
}