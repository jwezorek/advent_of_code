
#include "../util/util.h"
#include "../util/vec2.h"
#include "y2018.h"
#include <filesystem>
#include <functional>
#include <print>
#include <ranges>
#include <unordered_set>
#include <queue>

namespace r = std::ranges;
namespace rv = std::ranges::views;

/*------------------------------------------------------------------------------------------------*/

namespace {

    using point = aoc::vec2<int>;
    using point_set = aoc::vec2_set<int>;
    
    template<typename U>
    using point_map = aoc::vec2_map<int, U>;

    constexpr auto k_initial_hp = 200;
    constexpr auto k_basic_attack_power = 3;
    constexpr auto k_wall = -1;

    struct battle_result {
        int score;
        bool elf_victory;
        int units_remaining;
    };

    struct combatant {
        int id;
        bool is_elf;
        point position;
        int hit_points;

        combatant(int id = -1, bool elf = false, point loc ={-1,-1}) :
            id(id),
            is_elf(elf),
            position(loc),
            hit_points(k_initial_hp) {
        }
    };

    bool compare_points(const point& lhs, const point& rhs) {
        if (lhs.y < rhs.y) {
            return true;
        }
        if (lhs.y > rhs.y) {
            return false;
        }
        return lhs.x < rhs.x;
    }

    using visit_fn = std::function<bool(const point& pt, int dist)>;

    class battle {
        point_map<int> map_;
        std::unordered_map<int, combatant> combatants_;
        int wd_;
        int hgt_;
        int elf_attack_power_;

        const int k_wall = -1;

        auto adjacent_locs(const point& loc) const {
            const static std::array<point, 4> deltas = {{
                {0,-1}, {1, 0}, {0,1}, {-1,0}
            }};
            return deltas | rv::transform(
                [&](auto&& delta) {
                    return loc + delta;
                }
            );
        }

        struct trav_item {
            point loc;
            int dist;
        };

    public:
        battle(const std::vector<std::string>& inp) :
                hgt_(static_cast<int>(inp.size())),
                wd_(static_cast<int>(inp.front().size())),
                elf_attack_power_(k_basic_attack_power) {
            for (int y = 0; y < hgt_; ++y) {
                for (int x = 0; x < wd_; ++x) {
                    point loc = { x,y };
                    auto tile = inp[y][x];
                    if (tile == '#') {
                        map_[loc] = k_wall;
                    }
                    else if (tile == 'G' || tile == 'E') {
                        int id = static_cast<int>(combatants_.size());
                        combatants_[id] = combatant(
                            id,
                            tile == 'E',
                            point{ x,y }
                        );
                        map_[loc] = id;
                    }
                }
            }
        }

        void set_elf_attack_power(int power) {
            elf_attack_power_ = power;
        }

        auto combatants() const {
            return combatants_ | rv::values | rv::filter(
                [](auto&& unit) {
                    return unit.hit_points > 0;
                }
            );
        }

        auto enemies(bool is_elf) const {
            return combatants_ | rv::values | rv::filter(
                [is_elf](auto&& c) {
                    return c.hit_points > 0 && c.is_elf == !is_elf;
                }
            ) | rv::transform(
                [](const auto& c) {
                    return &c;
                }
            );
        }

        bool contains_unit(const point& pt) const {
            if (!map_.contains(pt)) {
                return false;
            }
            return map_.at(pt) >= 0;
        }

        auto adjacent_enemies(const combatant& unit1) const {
            return adjacent_locs(unit1.position) | rv::filter(
                [&](auto&& loc) {
                    if (!contains_unit(loc)) {
                        return false;
                    }
                    const auto& unit2 = combatants_.at(map_.at(loc));
                    return unit2.hit_points > 0 && unit2.is_elf == !unit1.is_elf;
                }
            ) | rv::transform(
                [&](auto&& loc)->const combatant* {
                    return &combatants_.at(map_.at(loc));
                }
            );
        }

        auto adjacent_empty_locations(const point& pt) const {
            return adjacent_locs(pt) | rv::filter(
                [&](auto&& loc) {
                    if (!map_.contains(loc)) {
                        return true;
                    }
                    if (map_.at(loc) == k_wall) {
                        return false;
                    }
                    const auto& unit = combatants_.at(map_.at(loc));
                    return unit.hit_points <= 0;
                }
            );
        }

        std::vector<const combatant*> ordered_combatants() const {
            auto combatants = combatants_ | rv::values | rv::transform(
                [](const auto& c) {
                    return &c;
                }
            ) | r::to<std::vector>();
            r::sort(combatants,
                [](const auto* lhs, const auto* rhs) {
                    return compare_points(lhs->position, rhs->position);
                }
            );
            return combatants;
        }

        void prune_dead() {
            map_ = map_ | rv::filter(
                [&](auto&& pair) {
                    const auto& [key, val] = pair;
                    if (val < 0) {
                        return true;
                    }
                    return combatants_.at(val).hit_points > 0;
                }
            ) | r::to<point_map<int>>();
            combatants_ = combatants_ | rv::filter(
                [](auto&& pair) {
                    const auto& [key, val] = pair;
                    return val.hit_points > 0;
                }
            ) | r::to<std::unordered_map<int, combatant>>();
        }

        void move_unit(int unit_id, const point& dest) {
            auto& unit = combatants_[unit_id];
            map_.erase(unit.position);
            map_[dest] = unit_id;
            unit.position = dest;
        }

        void attack_unit(int unit_id) {
            auto& attackee = combatants_[unit_id];
            bool attacker_is_elf = !attackee.is_elf;
            attackee.hit_points -= (attacker_is_elf) ? 
                elf_attack_power_ :
                k_basic_attack_power;
        }

        void bfs(const point& start, const visit_fn& visit) const {
            std::queue<trav_item> queue;
            point_set visited;
            queue.push({ start, 0 });
            while (!queue.empty()) {
                auto current = queue.front();
                queue.pop();

                if (visited.contains(current.loc)) {
                    continue;
                }
                if (!visit(current.loc, current.dist)) {
                    return;
                }
                visited.insert(current.loc);
                for (auto&& loc : adjacent_empty_locations(current.loc)) {
                    queue.push(
                        { loc, current.dist + 1 }
                    );
                }
            }
        }

        void display() const {
            for (int y = 0; y < hgt_; ++y) {
                for (int x = 0; x < wd_; ++x) {
                    char tile;
                    if (!map_.contains({ x,y })) {
                        tile = '.';
                    } else {
                        int val = map_.at({ x,y });
                        if (val == k_wall) {
                            tile = '#';
                        } else {
                            tile = (combatants_.at(val).is_elf) ? 'E' : 'G';
                        }
                    }
                    std::print("{}", tile);
                }
                std::println("");
            }
        }
    };

    std::vector<point> empty_neighboring_sites(const std::vector<point>& sites, const battle& b) {
        point_set set;
        for (auto&& pt : sites) {
            for (auto empty_loc : b.adjacent_empty_locations(pt)) {
                set.insert(empty_loc);
            }
        }
        return set | r::to<std::vector>();
    }

    bool are_adjacent(const point& pt1, const point& pt2) {
        auto diff = pt1 - pt2;
        return (std::abs(diff.x) == 1 && diff.y == 0) ||
            (std::abs(diff.y) == 1 && diff.x == 0);
    }

    bool is_in_range(const combatant& unit, const std::vector<point>& pts) {
        for (const auto& pt : pts) {
            if (are_adjacent(unit.position, pt)) {
                return true;
            }
        }
        return false;
    }

    std::vector<point> dist_map_to_closest_points(const point_map<int>& dist_map) {
        auto closest_dist = r::min_element(
            dist_map,
            [](auto&& lhs, auto&& rhs)->bool {
                return lhs.second < rhs.second;
            }
        )->second;
        return dist_map | rv::filter(
            [&](auto&& pair) {
                return pair.second == closest_dist;
            }
        ) | rv::keys | r::to<std::vector>();
    }

    std::vector<point>  closest_reachable_sites(
            const point& start, const std::vector<point>& pts, const battle& b) {
        point_map<int> dist_map;
        point_set targets = pts | r::to<point_set>();
        auto visit = [&](const point& loc, int dist)->bool {
                if (!targets.contains(loc)) {
                    return true;
                }
                dist_map[loc] = dist;
                return dist_map.size() < pts.size();
            };
        b.bfs(start, visit);
        if (dist_map.empty()) {
            return {};
        }
        return dist_map_to_closest_points(dist_map);
    }

    std::optional<point> select_attack_site(
            const point& unit_loc, const std::vector<point>& enemy_locs, const battle& b) {

        auto site_candidates = empty_neighboring_sites(enemy_locs, b);
        if (site_candidates.empty()) {
            return {};
        }

        site_candidates = closest_reachable_sites(unit_loc, site_candidates, b);
        if (site_candidates.empty()) {
            return {};
        }
        r::sort(
            site_candidates,
            compare_points
        );

        return site_candidates.front();
    }

    point first_step_towards_target(const point& start, const point& dest, const battle& b) {
        point_map<int> dist_map;
        point_set possible_first_steps = b.adjacent_empty_locations(start) | r::to<point_set>();
        auto visit = [&](const point& loc, int dist)->bool {
                if (!possible_first_steps.contains(loc)) {
                    return true;
                }
                dist_map[loc] = dist;
                return dist_map.size() < possible_first_steps.size();
            };
        b.bfs(dest, visit);
        auto step_candidates = dist_map_to_closest_points(dist_map);
        r::sort(
            step_candidates,
            compare_points
        );

        return step_candidates.front();
    }

    bool do_move(const combatant& c, battle& battle) {
        auto enemy_locations = battle.enemies(c.is_elf) |
            rv::transform(
                [](auto&& enemy) {
                    return enemy->position;
                }
            ) | r::to<std::vector>();

        if (enemy_locations.empty()) {
            return false;
        }

        //is the unit already in range...
        if (is_in_range(c, enemy_locations)) {
            return true;
        }
        
        auto target = select_attack_site(c.position, enemy_locations, battle);
        if (!target) {
            return true;
        }

        auto new_loc = first_step_towards_target(c.position, *target, battle);
        battle.move_unit(c.id, new_loc);

        return true;
    }

    void do_attack(const combatant& c, battle& battle) {
        auto target_candidates = battle.adjacent_enemies(c) | r::to<std::vector>();
        if (target_candidates.empty()) {
            return;
        }
        int min_hit_points = r::min(
            target_candidates | rv::transform([](const auto* ptr) {return ptr->hit_points; })
        );
        target_candidates = target_candidates | rv::filter(
                [&](const auto* c) {
                    return c->hit_points == min_hit_points;
                }
            ) | r::to<std::vector>();
        r::sort(
            target_candidates,
            [](const auto* lhs, const auto* rhs) {
                return compare_points(lhs->position, rhs->position);
            }
        );
        auto target = target_candidates.front();
        battle.attack_unit(target->id);
    }

    bool do_turn(const combatant& c, battle& battle) {
        if (c.hit_points <= 0) {
            return true;
        }
        auto move_complete = do_move(c, battle);
        if (!move_complete) {
            return false;
        }
        do_attack(c, battle);
        return true;
    }

    battle_result completed_battle_score(int round, const battle& b) {
        bool elf_victory = b.combatants().front().is_elf;
        int units_remaining = r::distance(b.combatants());
        auto score = round * r::fold_left(
            b.combatants() | rv::transform(
                [](auto&& unit) {
                    return unit.hit_points;
                }
            ),
            0,
            std::plus<>()
        );
        return { score, elf_victory, units_remaining };
    }

    std::optional<battle_result> do_one_round(int& round_number, battle& battle) {

        auto combatants = battle.ordered_combatants();
        for (const auto* combatant_ptr : combatants) {
            if (!do_turn(*combatant_ptr, battle)) {
                battle.prune_dead();
                return completed_battle_score(round_number, battle);
            }
        }
        battle.prune_dead();
        ++round_number;
        return {};
    }

    battle_result simulate_battle(const battle& inp_battle, bool display) {
        auto battle = inp_battle;
        int round = 0;
        std::optional<battle_result> result;
        while (!result) {

            if (display) {
                std::println("round {}:", round);
                battle.display();
                std::println("");
            }

            result = do_one_round(round, battle);
        }
        return *result;
    }

    int do_part_2(const battle& inp_battle) {
        int elf_attack_power = k_basic_attack_power;
        battle_result result;

        int num_elves = r::count_if(
            inp_battle.combatants(),
            [](const auto& unit) {
                return unit.is_elf;
            }
        );

        do {
            auto battle = inp_battle;
            battle.set_elf_attack_power(++elf_attack_power);
            result = simulate_battle(battle, false);
        } while (!result.elf_victory || result.units_remaining != num_elves);

        return result.score;
    }

}

void aoc::y2018::day_15(const std::string& title) {

    auto input_battle = battle(
        aoc::file_to_string_vector(
            aoc::input_path(2018, 15)
        )
    );

    std::println("--- Day 15: {} ---", title);
    std::println("  part 1: {}", simulate_battle(input_battle, false).score );
    std::println("  part 2: {}", do_part_2(input_battle));
    
}
