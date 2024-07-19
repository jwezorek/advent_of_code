#include "../util/util.h"
#include "y2019.h"
#include <filesystem>
#include <functional>
#include <print>
#include <ranges>
#include <unordered_set>

namespace r = std::ranges;
namespace rv = std::ranges::views;

/*------------------------------------------------------------------------------------------------*/

namespace {
    struct chemical {
        int64_t amount;
        std::string symbol;
    };

    chemical operator*(int64_t k, const chemical& ing) {
        return { k * ing.amount, ing.symbol };
    }

    chemical operator-(const chemical& ing, int64_t k) {
        return { ing.amount - k, ing.symbol };
    }

    struct reaction {
        chemical output;
        std::vector<chemical> input;
    };

    reaction operator*(int64_t k, const reaction& rec) {
        return {
            k * rec.output,
            rec.input | rv::transform(
                    [k](auto&& ing)->chemical {
                        return k * ing;
                    }
                ) | r::to<std::vector>()
        };
    }

    reaction parse_reaction(const std::string& str) {
        auto words = aoc::extract_alphanumeric(str);
        auto chemicals = words | rv::chunk(2) | rv::transform(
                [](auto rng)->chemical {
                    return {
                        std::stoi(rng[0]),
                        rng[1]
                    };
                }
            ) | r::to<std::vector>();
        return {
            chemicals.back(),
            chemicals | rv::take(chemicals.size() - 1) | r::to<std::vector>()
        };
    }

    using reaction_tbl = std::unordered_map<std::string, reaction>;
    reaction_tbl make_recipe_table(const std::vector<reaction>& recipes) {
        return recipes | rv::transform(
                [](auto&& rec)->reaction_tbl::value_type {
                    return { rec.output.symbol, rec };
                }
            ) | r::to<reaction_tbl>();
    }

    class inventory {
        std::unordered_map<std::string, int64_t> chemicals_;
        int64_t ore_created_;
    public:

        inventory() : ore_created_(0) {
        }

        int64_t amount(const std::string& what) {
            return (chemicals_.contains(what)) ? chemicals_.at(what) : 0;
        }

        bool has_enough(const chemical& chem) {
            return amount(chem.symbol) >= chem.amount;
        }

        void generate(const chemical& chem) {
            if (chem.symbol == "ORE") {
                ore_created_ += chem.amount;
            }
            chemicals_[chem.symbol] += chem.amount;
        }

        void consume(const chemical& chem) {
            if (!has_enough(chem)) {
                throw std::runtime_error("chemical consumption underflow");
            }
            chemicals_[chem.symbol] -= chem.amount;
        }

        int64_t ore_created() const {
            return ore_created_;
        }
    };

    int64_t ceiling_division(int64_t dividend, int64_t divisor) {
        return (dividend + divisor - 1) / divisor;
    }

    void perform_reaction(const reaction& re, inventory& inventory, const reaction_tbl& reactions);

    void fabricate_chemical( 
            const chemical& fabricatee, inventory& inventory, const reaction_tbl& reactions) {
        
        if (inventory.has_enough(fabricatee)) {
            return;
        }

        auto chem = fabricatee;
        chem = chem - inventory.amount(chem.symbol);

        if (chem.symbol == "ORE") {
            inventory.generate(chem);
            return;
        }

        auto reaction = reactions.at(chem.symbol);
        int64_t reaction_multiplier = (chem.amount <= reaction.output.amount) ?
            1 :
            ceiling_division(chem.amount, reaction.output.amount);
        reaction = reaction_multiplier * reaction;

        perform_reaction(reaction, inventory, reactions);
    }

    void perform_reaction(const reaction& rec, inventory& inventory, const reaction_tbl& reactions) {
        for (const auto& chem : rec.input) {
            fabricate_chemical(chem, inventory, reactions);
            inventory.consume(chem);
        }
        inventory.generate(rec.output);
    }

    int64_t minimum_ore_for_fuel(const reaction_tbl& reactions, int64_t amount_of_fuel) {
        inventory inv;
        fabricate_chemical({ amount_of_fuel, "FUEL" }, inv, reactions);
        return inv.ore_created();
    }
    
    bool can_make_fuel_from_ore(const reaction_tbl& reactions, int64_t fuel, int64_t ore) {
        auto required_ore = minimum_ore_for_fuel(reactions, fuel);
        return ore > required_ore;
    }

    int64_t maximum_fuel_from_given_ore(const reaction_tbl& reactions, int64_t ore,
            int64_t lower_fuel, int64_t upper_fuel) {
        if (lower_fuel == upper_fuel || upper_fuel - lower_fuel == 1) {
            return lower_fuel;
        }
        auto mid_fuel = (lower_fuel + upper_fuel) / 2;
        if (can_make_fuel_from_ore(reactions, mid_fuel, ore)) {
            return maximum_fuel_from_given_ore(reactions, ore, mid_fuel, upper_fuel);
        } else {
            return maximum_fuel_from_given_ore(reactions, ore, lower_fuel, mid_fuel);
        }
    }

    int64_t maximum_fuel_from_given_ore(const reaction_tbl& reactions, int64_t ore) {
        int64_t upper_fuel = ore;
        int64_t lower_fuel = 1;

        if (can_make_fuel_from_ore(reactions, upper_fuel, ore)) {
            return -1;
        }

        if (!can_make_fuel_from_ore(reactions, lower_fuel, ore)) {
            return -1;
        }

        return maximum_fuel_from_given_ore(reactions, ore, lower_fuel, upper_fuel);
    }
}

void aoc::y2019::day_14(const std::string& title) {

    auto reactions = make_recipe_table(
            aoc::file_to_string_vector(
                aoc::input_path(2019, 14)
            ) | rv::transform(
                parse_reaction
            ) | r::to<std::vector<reaction>>()
        );

    std::println("--- Day 14: {} ---", title);
    std::println("  part 1: {}", minimum_ore_for_fuel(reactions, 1));
    std::println("  part 2: {}",
        maximum_fuel_from_given_ore(reactions, 1000000000000)
    );
}