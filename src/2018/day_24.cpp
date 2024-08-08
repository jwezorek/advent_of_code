
#include "../util/util.h"
#include "y2018.h"
#include <filesystem>
#include <functional>
#include <print>
#include <ranges>
#include <set>
#include <unordered_set>

namespace r = std::ranges;
namespace rv = std::ranges::views;

/*------------------------------------------------------------------------------------------------*/

namespace {

    struct group {
        int num_units;
        int hit_points;
        int damage_amt;
        std::string damage_type;
        int initiative;
        std::set<std::string> weaknesses;
        std::set<std::string> immunities;
    };

    using army = std::vector<group>;

    std::tuple<std::string, std::string> extract_parenthetical_phrase(const std::string& str) {
        auto open_paren = str.find('(');
        if (open_paren == str.npos) {
            return { "", str };
        }
        auto close_paren = str.find(')');
        auto phrase = str.substr(open_paren+1, close_paren-open_paren-1);
        auto leftover = str;
        leftover.erase(open_paren, close_paren - open_paren + 1);

        return { phrase, leftover };
    }

    std::tuple<std::set<std::string>, std::set<std::string>> parse_weaknesses_and_immunities(
            const std::string& phrase) {
        auto clauses = aoc::split(phrase, ';');
        std::set<std::string> weaknesses;
        std::set<std::string> immunities;
        for (const auto clause : clauses) {
            auto words = aoc::extract_alphabetic(clause);
            std::set<std::string>& dest = (words[0] == "weak") ? weaknesses : immunities;
            for (auto quality : words | rv::drop(2)) {
                dest.insert(quality);
            }
        }
        return { weaknesses, immunities }; 
    }

    group parse_group(const std::string& inp) {
        auto [phrase , str] = extract_parenthetical_phrase(inp);
        auto words = aoc::extract_alphanumeric(str);
        auto [weaknesses, immunities] = parse_weaknesses_and_immunities(phrase);
        return {
            .num_units = std::stoi(words[0]),
            .hit_points = std::stoi(words[4]),
            .damage_amt = std::stoi(words[12]),
            .damage_type = words[13],
            .initiative = std::stoi(words[17]),
            .weaknesses = weaknesses,
            .immunities = immunities
        };
    }

    std::tuple<army, army> parse(const std::vector<std::string>& inp) {
        auto preprocess = inp | rv::drop(1) | rv::transform(
                [](auto&& line)->std::string {
                    if (line.contains( ':' )) {
                        return {};
                    }
                    return line;
                }
            ) | r::to<std::vector>();
        auto armies = aoc::group_strings_separated_by_blank_lines(preprocess) |
            rv::transform(
                [](auto&& str_group) {
                    return str_group | rv::transform(parse_group) | r::to<std::vector>();
                }
            ) | r::to<std::vector>();

        return { std::move(armies[0]) , std::move(armies[1]) };
    }

    struct results {
        bool immune_system_win;
        int units_remaining;
    };

    int units_remaining(const army& army) {
        return r::fold_left(
            army | rv::transform([](auto&& group) { return group.num_units; }),
            0,
            std::plus<>()
        );
    }

    int effective_power(const group& g) {
        return g.num_units * g.damage_amt;
    }

    int attack_damage(const group& attacker, const group& defender) {
        int modifier = 1;
        if (defender.immunities.contains(attacker.damage_type)) {
            modifier = 0;
        } else if (defender.weaknesses.contains(attacker.damage_type)) {
            modifier = 2;
        }
        return effective_power(attacker) * modifier;
    }

    bool compare_targets(const group& attacker, group& lhs, group& rhs) {
        int lhs_damage = attack_damage(attacker, lhs);
        int rhs_damage = attack_damage(attacker, rhs);
        if (lhs_damage < rhs_damage) {
            return true;
        }
        if (lhs_damage > rhs_damage) {
            return false;
        }
        if (effective_power(lhs) < effective_power(rhs)) {
            return true;
        }
        if (effective_power(lhs) > effective_power(rhs)) {
            return false;
        }
        return lhs.initiative < rhs.initiative;
    }

    group* find_target(
            const group& attacker, army& defenders, 
            const std::unordered_set<group*>& targeted) {

        auto defender_ptrs = defenders |
            rv::filter(
                [&](auto& d) {
                    if (targeted.contains(&d)) {
                        return false;
                    }
                    if (attack_damage(attacker, d) == 0) {
                        return false;
                    }
                    return true;
                }
            ) | rv::transform(
                [](auto& d) {return &d; }
            ) | r::to<std::vector>();

        if (defender_ptrs.empty()) {
            return nullptr;
        }

        auto* target = r::max(
            defender_ptrs,
            [&](auto* lhs, auto* rhs)->bool {
                return compare_targets(attacker, *lhs, *rhs);
            }
        );
        
        return target; 
    }

    void target_one_side(std::vector<std::tuple<group*, group*>>& pairings,
            army& attackers, army& defenders) {
        
        auto ordered_attackers = attackers | rv::transform(
                [](auto& attacker) {
                    return &attacker;
                }
            ) | r::to<std::vector>();

        r::sort(ordered_attackers,
            [](group* lhs_ptr, group* rhs_ptr) {
                auto& lhs = *lhs_ptr;
                auto& rhs = *rhs_ptr;
                if (effective_power(lhs) < effective_power(rhs)) {
                    return false;
                }
                if (effective_power(lhs) > effective_power(rhs)) {
                    return true;
                }
                return lhs.initiative > rhs.initiative;
            }
        );

        std::unordered_set<group*> targeted;
        for (auto* attacker : ordered_attackers) {
            auto* target = find_target(*attacker, defenders, targeted);
            if (target) {
                targeted.insert(target);
                pairings.emplace_back(attacker, target);
            }
        }
    }

    std::vector<std::tuple<group*, group*>> find_targets(army& good_guys, army& bad_guys) {
        std::vector<std::tuple<group*, group*>> pairings;
        
        target_one_side(pairings, bad_guys, good_guys);
        target_one_side(pairings, good_guys, bad_guys);

        return pairings; 
    }

    bool fight_battle(group& attacker, group& defender) {
        int damage_amt = attack_damage(attacker, defender);
        int dead_units = damage_amt / defender.hit_points;
        if (dead_units == 0) {
            return false;
        }
        defender.num_units -= dead_units;
        if (defender.num_units < 0) {
            defender.num_units = 0;
        }
        return true;
    }

    bool do_attack_phase(std::vector<std::tuple<group*, group*>>& battles) {
        r::sort(
            battles,
            [](const auto& lhs, const auto& rhs) {
                return std::get<0>(lhs)->initiative > std::get<0>(rhs)->initiative;
            }
        );
        bool did_damage = false;
        for (auto [attacker, defender] : battles) {
            if (attacker->num_units <= 0) {
                continue;
            }
            if (fight_battle(*attacker, *defender)) {
                did_damage = true;
            }
        }
        return did_damage;
    }

    army cull_dead(const army& army) {
        return army | rv::filter(
            [](auto&& group) {
                return group.num_units > 0;
            }
        ) | r::to<std::vector>();
    }

    results do_war(const army& immune_system, const army& infection) {
        auto good_guys = immune_system;
        auto bad_guys = infection;
        while (!good_guys.empty() && !bad_guys.empty()) {
            auto battles = find_targets(good_guys, bad_guys);
            if (battles.empty()) {
                return { false, units_remaining(bad_guys) };
            }
            if (!do_attack_phase(battles)) {
                return { false, units_remaining(bad_guys) };
            }
            good_guys = cull_dead(good_guys);
            bad_guys = cull_dead(bad_guys);
        }
        auto win = !good_guys.empty();
        return {
            win,
            (win) ? units_remaining(good_guys) : units_remaining(bad_guys)
        };
    }

    int do_part_2(const army& immune_system, const army& infection) {
        int boost_amt = 1;
        results war_result = { false, 0 };
        while (!war_result.immune_system_win) {
            auto boosted_immune_sys = immune_system;
            for (auto& group : boosted_immune_sys) {
                group.damage_amt += boost_amt;
            }
            war_result = do_war(boosted_immune_sys, infection);
            ++boost_amt;
        }
        return war_result.units_remaining;
    }
}

void aoc::y2018::day_24(const std::string& title) {

    auto inp = aoc::file_to_string_vector(
            aoc::input_path(2018, 24)
        ); 

    auto [immune_system, infection] = parse(inp);

    std::println("--- Day 24: {} ---", title);
    std::println("  part 1: {}", 
        do_war(immune_system, infection).units_remaining
    );
    std::println("  part 2: {}", do_part_2(immune_system, infection));
    
}
