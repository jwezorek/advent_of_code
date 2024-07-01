#include "../util.h"
#include "y2015.h"
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

    struct character_stats {
        int hit_points;
        int mana;
        int damage;

        bool operator==(const character_stats& stats) const {
            return hit_points == stats.hit_points &&
                mana == stats.mana &&
                damage == stats.damage;
        }
    };

    struct character_hash {
        size_t operator()(const character_stats& c) const {
            size_t seed = 0;
            boost::hash_combine( seed, c.hit_points );
            boost::hash_combine( seed, c.mana );
            boost::hash_combine( seed, c.damage );
            return seed;
        }
    };

    struct game_state {
        character_stats player;
        character_stats boss;
        int shield_effect;
        int poison_effect;
        int recharge_effect;

        bool operator==(const game_state& s) const {
            return player == s.player &&
                boss == s.boss &&
                shield_effect == s.shield_effect &&
                poison_effect == s.poison_effect &&
                recharge_effect == s.recharge_effect;
        }
    };

    struct game_state_hash {
        size_t operator()(const game_state& c) const {
            size_t seed = 0;
            boost::hash_combine(seed, character_hash{}(c.player));
            boost::hash_combine(seed, character_hash{}(c.boss));
            boost::hash_combine(seed, c.shield_effect);
            boost::hash_combine(seed, c.poison_effect);
            boost::hash_combine(seed, c.recharge_effect);
            return seed;
        }
    };

    enum spell_type {
        magic_missile = 0,
        drain,
        shield,
        poison,
        recharge
    };

    const std::unordered_map<spell_type, int> g_spell_to_mana = {
        {magic_missile, 53},
        {drain, 73},
        {shield, 113},
        {poison, 173},
        {recharge, 229}
    };

    auto all_spells() {
        return rv::iota(0, 5) | rv::transform(
            [](auto i) {
                return static_cast<spell_type>(i);
            }
        );
    }

    bool can_cast_spell(const game_state& gs, spell_type spell) {
        if (gs.player.mana < g_spell_to_mana.at(spell)) {
            return false;
        }
        if (spell == shield && gs.shield_effect > 1) {
            return false;
        }
        if (spell == poison && gs.poison_effect > 1) {
            return false;
        }
        if (spell == recharge && gs.recharge_effect > 1) {
            return false;
        }
        return true;
    }

    void do_spell_effects(game_state& state) {
        if (state.poison_effect > 0) {
            state.boss.hit_points -= 3;
            --state.poison_effect;
        }
        if (state.shield_effect > 0) {
            --state.shield_effect;
        }
        if (state.recharge_effect > 0) {
            state.player.mana += 101;
            --state.recharge_effect;
        }
    }

    void do_player_turn(game_state& state, spell_type spell) {
        state.player.mana -= g_spell_to_mana.at(spell);
        switch (spell) {
            case magic_missile:
                state.boss.hit_points -= 4;
                break;
            case drain:
                state.boss.hit_points -= 2;
                state.player.hit_points += 2;
                break;
            case shield:
                state.shield_effect = 6;
                break;
            case poison:
                state.poison_effect = 6;
                break;
            case recharge:
                state.recharge_effect = 5;
                break;
        }
    }

    void do_boss_turn(game_state& state) {
        int shield_spell = (state.shield_effect > 0) ? 7 : 0;
        state.player.hit_points -= std::max(
            state.boss.damage - shield_spell, 1
        );
    }
    
    game_state do_turn(const game_state& state, spell_type spell) {
        game_state next_state = state;

        do_spell_effects(next_state);
        if (next_state.boss.hit_points <= 0) {
            return next_state;
        }

        do_player_turn(next_state, spell);
        if (next_state.boss.hit_points <= 0) {
            return next_state;
        }

        do_spell_effects(next_state);
        if (next_state.boss.hit_points <= 0) {
            return next_state;
        }

        do_boss_turn(next_state);

        return next_state;
    }

    character_stats player_stats() {
        return { 50, 500, 0 };
    }

    std::vector<std::tuple<game_state, int>> players_next_moves(const game_state& gs) {

        if (gs.player.hit_points <= 0 || gs.boss.hit_points <= 0) {
            return {};
        }

        return all_spells() | rv::filter(
                [&](auto spell) {
                    return can_cast_spell(gs, spell);
                }
            ) | rv::transform(
                [&](auto spell)->std::tuple<game_state, int> {
                    return { do_turn(gs, spell), g_spell_to_mana.at(spell) };
                }
            ) | r::to<std::vector>();
    }

    using state_to_mana_map = std::unordered_map<game_state, int, game_state_hash>;

    int find_lowest_mana_win(const state_to_mana_map& state_to_mana) {
        int min_mana_win = std::numeric_limits<int>::max();
        for (const auto& [gs, mana] : state_to_mana) {
            if (gs.player.hit_points > 0 && gs.boss.hit_points <= 0) {
                min_mana_win = std::min(min_mana_win, mana);
            }
        }
        return min_mana_win;
    }

    int lowest_mana_win(const character_stats& player, const character_stats& boss) {

        state_to_mana_map mana_consumed;
        aoc::priority_queue<game_state, game_state_hash> queue;
        game_state start_state = { player, boss, 0, 0, 0 };
        queue.insert(start_state, 0);
        mana_consumed[start_state] = 0;

        while (!queue.empty()) {
            auto u = queue.extract_min();
            auto moves = players_next_moves(u);

            for (auto [v, dist_to_v] : moves) {
                auto dist_to_u = mana_consumed.at(u);
                auto dist_through_u_to_v = dist_to_u + dist_to_v;
                auto curr_dist_to_v = mana_consumed.contains(v) ? 
                    mana_consumed.at(v) : 
                    std::numeric_limits<int>::max();

                if (dist_through_u_to_v < curr_dist_to_v) {
                    mana_consumed[v] = dist_through_u_to_v;
                    if (queue.contains(v)) {
                        queue.change_priority(v, dist_through_u_to_v);
                    } else {
                        queue.insert(v, dist_through_u_to_v);
                    }
                }
            }
        }

        return find_lowest_mana_win(mana_consumed);
    }

    character_stats parse_boss_stats(const std::string& str) {
        auto values = aoc::extract_numbers(str);
        return {
            values[0],
            0,
            values[1]
        };
    }
}

void aoc::y2015::day_22(const std::string& title) {

    auto boss = parse_boss_stats(
        aoc::file_to_string(aoc::input_path(2015, 22))
    );

    auto player = player_stats();

    std::println("--- Day 22: {} ---", title);
    std::println("  part 1: {}",
        lowest_mana_win(player, boss)
    );
    std::println("  part 2: {}",
        0
    );
}