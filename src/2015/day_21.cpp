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
    struct character_stats {
        int hit_points;
        int damage;
        int armor;
    };

    character_stats parse_boss_stats(const std::string& inp) {
        auto values = aoc::extract_numbers(inp);
        return { values[0], values[1], values[2] };
    }

    struct item {
        std::string name;
        int cost;
        int damage;
        int armor;
    };

    item operator+(const item& lhs, const item& rhs) {
        return {
            lhs.name + " , " + rhs.name,
            lhs.cost + rhs.cost,
            lhs.damage + rhs.damage,
            lhs.armor + rhs.armor
        };
    }

    const std::array<item, 5> k_weapons = {{
        {"Dagger",        8,     4,       0},
        {"Shortsword",   10,     5,       0},
        {"Warhammer",    25,     6,       0},
        {"Longsword",    40,     7,       0},
        {"Greataxe",     74,     8,       0}
    }};

    const std::array<item, 6> k_armor = {{
        {"no armor",      0,     0,       0},
        {"Leather",      13,     0,       1},
        {"Chainmail",    31,     0,       2},
        {"Splintmail",   53,     0,       3},
        {"Bandedmail",   75,     0,       4},
        {"Platemail",   102,     0,       5}
    }};

    const std::array<item, 6> k_rings = {{
        {"Damage + 1",    25,     1,       0},
        {"Damage + 2",    50,     2,       0},
        {"Damage + 3",   100,     3,       0},
        {"Defense + 1",   20,     0,       1},
        {"Defense + 2",   40,     0,       2},
        {"Defense + 3",   80,     0,       3}
    }};

    auto ring_options() {
        static std::vector<item> rings;
        if (rings.empty()) {
            rings.push_back({ "no ring", 0, 0, 0, });
            for (const auto& ring : k_rings) {
                rings.push_back(ring);
            }
            for (const auto& [ring1, ring2] : aoc::two_combinations(k_rings)) {
                rings.push_back(ring1 + ring2);
            }
        }
        return rv::all(rings);
    }

    auto all_player_options() {
        static std::vector<item> items;
        if (items.empty()) {
            items = rv::cartesian_product(k_weapons, k_armor, ring_options()) |
                rv::transform(
                    [](auto&& triple)->item {
                        const auto& [weapon, armor, rings] = triple;
                        return weapon + armor + rings;
                    }
                ) | r::to<std::vector>();
        }
        return rv::all(items);
    }

    bool does_player_win( character_stats player, character_stats boss) {
        while (player.hit_points > 0 && boss.hit_points > 0) {
            int players_hit = std::max(player.damage - boss.armor, 1);
            boss.hit_points -= players_hit;

            if (boss.hit_points > 0) {
                int bosses_hit = std::max(boss.damage - player.armor, 1);
                player.hit_points -= bosses_hit;
            }
        }
        return player.hit_points > 0;
    }

    int find_cheapest_winning_player_stats(const character_stats& boss) {
        int lowest_cost_winner = std::numeric_limits<int>::max();
        for (const auto& config : all_player_options()) {
            character_stats player = { 100, config.damage, config.armor };
            if (does_player_win(player, boss)) {
                lowest_cost_winner = std::min(lowest_cost_winner, config.cost);
            }
        }
        return lowest_cost_winner;
    }

    int find_priciest_losing_player_stats(const character_stats& boss) {
        int priciest_loser = -1;
        for (const auto& config : all_player_options()) {
            character_stats player = { 100, config.damage, config.armor };
            if (! does_player_win(player, boss)) {
                priciest_loser = std::max(priciest_loser, config.cost);
            }
        }
        return priciest_loser;
    }
}

void aoc::y2015::day_21(const std::string& title) {

    auto boss = parse_boss_stats(
        aoc::file_to_string(aoc::input_path(2015, 21))
    );

    auto items = all_player_options() | r::to<std::vector>();

    std::println("--- Day 21: {} ---", title);
    std::println("  part 1: {}",
        find_cheapest_winning_player_stats( boss )
    );
    std::println("  part 2: {}",
        find_priciest_losing_player_stats( boss )
    );
}