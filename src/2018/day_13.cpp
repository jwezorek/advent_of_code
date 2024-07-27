
#include "../util/util.h"
#include "../util/vec2.h"
#include "y2018.h"
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
    using vec2 = aoc::vec2<int>;
    using vec2_set = aoc::vec2_set<int>;
    using track_features = aoc::vec2_map<int,char>;

    enum direction {
        north = 0,
        east,
        south,
        west
    };

    enum turn_type {
        left = -1,
        no_turn,
        right
    };

    struct mine_cart {
        vec2 position;
        direction dir;
        turn_type next_turn;

        mine_cart(int x, int y, direction d) : 
            position{ x,y }, dir(d), next_turn(left)
        {}
    };

    std::optional<direction> char_to_dir(char tile) {
        static const std::unordered_map<char, direction> tbl = {
            {'^', north}, {'v', south}, {'<', west}, {'>',east}
        };
        if (tbl.contains(tile)) {
            return tbl.at(tile);
        }
        return {};
    }

    std::tuple<std::vector<mine_cart>, track_features> parse(const std::vector<std::string>& inp) {
        static const std::unordered_set<char> feature_tiles = { '\\', '/', '+' };
        int hgt = static_cast<int>(inp.size());
        int wd = static_cast<int>(inp.front().size());

        std::vector<mine_cart> mine_carts;
        track_features tracks;
        for (int y = 0; y < hgt; ++y) {
            for (int x = 0; x < wd; ++x) {
                auto tile = inp[y][x];
                auto dir = char_to_dir(tile);
                if (dir) {
                    mine_carts.emplace_back(x, y, *dir);
                    continue;
                }
                if (feature_tiles.contains(tile)) {
                    tracks[{x, y}] = tile;
                }
            }
        }

        return { std::move(mine_carts), std::move(tracks) };
    }

    void sort_mine_carts(std::vector<mine_cart>& carts) {
        r::sort(
            carts,
            [](auto&& lhs, auto&& rhs)->bool {
                if (lhs.position.y < rhs.position.y) {
                    return true;
                }
                if (lhs.position.y > rhs.position.y) {
                    return false;
                }
                return lhs.position.x < rhs.position.x;
            }
        );
    }

    const mine_cart* find_collision(
            const mine_cart& cart, const std::vector<mine_cart>& carts,
            const std::unordered_set<const mine_cart*> dead_carts) {
        auto is_other_live_cart = [&](const mine_cart& c)->bool {
            return &c != &cart && !dead_carts.contains(&c);
        };
        for (const auto& other : carts | rv::filter(is_other_live_cart)) {
            if (cart.position == other.position) {
                return &other;
            }
        }
        return nullptr;
    }

    vec2 delta_for_direction(direction dir) {
        const static std::unordered_map<direction, vec2> tbl = {
            {north, {0,-1} }, {east,{1,0}}, {south,{0,1}}, {west,{-1,0}}
        };
        return tbl.at(dir);
    }

    direction direction_after_curve(direction dir, char curve) {
        if (curve == '/') {
            const static std::unordered_map<direction, direction> tbl = {
                {east, north}, {north, east}, {west, south}, {south, west}
            };
            return tbl.at(dir);
        }
        const static std::unordered_map<direction, direction> tbl = {
            { east, south }, { north, west }, { west, north }, { south, east }
        };
        return tbl.at(dir);
    }

    direction perform_turn(direction dir, turn_type turn) {
        int dir_num = static_cast<int>(dir);
        int turn_num = static_cast<int>(turn);
        const int num_directions = 4;
        auto new_dir = dir_num + turn_num;
        if (new_dir < 0) {
            new_dir += num_directions;
        }
        if (new_dir >= num_directions) {
            new_dir = new_dir % num_directions;
        }
        return static_cast<direction>(new_dir);
    }

    turn_type increment_turn(turn_type turn) {
        int val = static_cast<int>(turn);
        val++;
        if (val >= 2) {
            val = -1;
        }
        return static_cast<turn_type>(val);
    }

    void handle_cart_on_tile(mine_cart& cart, char tile) {
        if (tile == '/' || tile == '\\') {
            cart.dir = direction_after_curve(cart.dir, tile);
            return;
        }
        // handle intersection
        cart.dir = perform_turn(cart.dir, cart.next_turn);
        cart.next_turn = increment_turn(cart.next_turn);
    }

    std::optional<vec2> simulate_one_turn(
            const track_features& tracks, std::vector<mine_cart>& carts) {

        sort_mine_carts(carts);

        std::unordered_set<const mine_cart*> dead_carts;
        std::optional<vec2> first_collision = {};

        for (auto& cart : carts) {
            if (dead_carts.contains(&cart)) {
                continue;
            }

            cart.position = cart.position + delta_for_direction(cart.dir);
            auto* collision = find_collision(cart, carts, dead_carts);
            if (collision) {
                if (!first_collision) {
                    first_collision = collision->position;
                }
                dead_carts.insert(&cart);
                dead_carts.insert(collision);
            }

            if (tracks.contains(cart.position) && !dead_carts.contains(&cart)) {
                handle_cart_on_tile(cart, tracks.at(cart.position));
            }
        }

        carts = carts | rv::filter(
                [&](const auto& c) {
                    return !dead_carts.contains(&c);
                }
            ) | r::to<std::vector>();

        return first_collision;
    }

    vec2 simulate_until_first_collision(
            const track_features& tracks, std::vector<mine_cart>& init_carts) {
        auto carts = init_carts;
        std::optional<vec2> collision = {};
        while (!collision) {
            collision = simulate_one_turn(tracks, carts);
        }
        return collision.value();
    }

    vec2 simulate_until_final_cart(
        const track_features& tracks, std::vector<mine_cart>& init_carts) {
        auto carts = init_carts;
        std::optional<vec2> final_cart = {};
        while (!final_cart) {
            simulate_one_turn(tracks, carts);
            if (carts.size() == 1) {
                final_cart = carts.front().position;
            }
        }
        return final_cart.value();
    }
}

void aoc::y2018::day_13(const std::string& title) {

    auto [carts, tracks] = parse(
        aoc::file_to_string_vector(
            aoc::input_path(2018, 13)
        )
    );

    std::println("--- Day 13: {} ---", title);

    auto collision = simulate_until_first_collision(tracks, carts);
    std::println("  part 1: {},{}", collision.x, collision.y);

    auto final_cart = simulate_until_final_cart(tracks, carts);
    std::println("  part 2: {},{}", final_cart.x, final_cart.y);
    
}
