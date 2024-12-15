
#include "../util/util.h"
#include "../util/vec2.h"
#include "y2024.h"
#include <filesystem>
#include <functional>
#include <print>
#include <ranges>
#include <unordered_map>

namespace r = std::ranges;
namespace rv = std::ranges::views;

/*------------------------------------------------------------------------------------------------*/

namespace {
    using point = aoc::vec2<int>;
    using point_set = aoc::vec2_set<int>;

    template<typename T>
    using point_map = aoc::vec2_map<int, T>;

    struct warehouse {
        bool double_wide;
        point pos;
        point_set walls;
        point_set crates;
    };

    std::tuple<warehouse, std::string> parse_input(const std::vector<std::string>& inp) {
        auto groups = aoc::group_strings_separated_by_blank_lines(inp);
        const auto& warehouse_map = groups.front();
        int wd = static_cast<int>(warehouse_map[0].size());
        int hgt = static_cast<int>(warehouse_map.size());

        warehouse warehouse;
        warehouse.double_wide = false;

        for (auto [x, y] : rv::cartesian_product(rv::iota(0, wd), rv::iota(0, hgt))) {
            point loc{ x,y };
            auto tile = warehouse_map[y][x];
            if (tile == '#') {
                warehouse.walls.insert(loc);
            } else if (tile == 'O') {
                warehouse.crates.insert(loc);
            } else if (tile == '@') {
                warehouse.pos = loc;
            }
        }

        return { 
            std::move(warehouse),  
            groups.back() | rv::join | r::to<std::string>()
        };
    }

    std::optional<point> crate_at_location(warehouse& wh, const point& loc) {
        if (wh.crates.contains(loc)) {
            return loc;
        }
        if (wh.double_wide) {
            auto crate_loc = loc + point{ -1,0 };
            if (wh.crates.contains(crate_loc)) {
                return crate_loc;
            }
        }
        return {};
    }

    bool is_wall(warehouse& wh, const point& loc) {
        if (wh.walls.contains(loc)) {
            return true;
        }
        if (wh.double_wide) {
            if (wh.walls.contains(loc + point{ -1,0 })) {
                return true;
            }
        }
        return false;
    }

    std::vector<point> adjacent_to_crate(
            const warehouse& wh, const point& crate, const point& dir) {

        std::vector<point> adj;
        if (!wh.double_wide) {
            adj.push_back(crate + dir);
            return adj;
        }

        const static point_map<std::vector<point>> adj_deltas = {
            {{0,-1},{{-1,-1},{0,-1},{1,-1}}},
            {{1,0},{{2,0}}},
            {{0,1},{{-1,1},{0,1},{1,1}}},
            {{-1,0},{{-2,0}}}
        };

        return adj_deltas.at(dir) | rv::transform(
                [&](auto&& delta) {
                    return delta + crate;
                }
            ) | r::to<std::vector>();
    }

    std::vector<point> crates_adjacent_to_crate(
            const warehouse& wh, const point& crate, const point& dir) {

        return adjacent_to_crate(wh, crate, dir) | rv::filter(
                [&](auto&& adj) {
                    return wh.crates.contains(adj);
                }
            ) | r::to<std::vector>();
    }

    bool is_wall_adjacent_to_crate(
            const warehouse& wh, const point& crate, const point& dir) {
        return r::count_if(
            adjacent_to_crate(wh, crate, dir),
            [&](auto&& loc) {
                return wh.walls.contains(loc);
            }
        ) > 0;
    }

    bool can_move_crate(warehouse& wh, const point& crate, const point& dir) {
        if (is_wall_adjacent_to_crate(wh, crate, dir)) {
            return false;
        }

        auto crates = crates_adjacent_to_crate(wh, crate, dir);
        for (const auto& crate : crates) {
            if (!can_move_crate(wh, crate, dir)) {
                return false;
            }
        }

        return true;
    }

    void move_crate_aux(warehouse& wh, const point& crate, const point& dir) {
        auto crates = crates_adjacent_to_crate(wh, crate, dir);
        for (const auto& crate : crates) {
            move_crate_aux(wh, crate, dir);
        }
        auto new_loc = crate + dir;
        wh.crates.erase(crate);
        wh.crates.insert(new_loc);
    }

    bool move_crate(warehouse& wh, const point& crate, const point& dir) {

        if (!can_move_crate(wh, crate, dir)) {
            return false;
        }

        move_crate_aux(wh, crate, dir);
        return true;
    }

    void run_instruction(warehouse& wh, char instr) {
        const static std::unordered_map<char, point> instr_to_dir = {
            {'^', {0,-1}},
            {'>', {1, 0}},
            {'v', {0,1}},
            {'<', {-1,0}}
        };
        auto dir = instr_to_dir.at(instr);
        auto new_pos = wh.pos + dir;
        if (is_wall(wh, new_pos)) {
            return;
        }
        auto blocking_crate = crate_at_location(wh, new_pos);
        if (blocking_crate) {
            if (!move_crate(wh, *blocking_crate, dir)) {
                return;
            }
        }
        wh.pos = new_pos;
    }

    warehouse run_instructions(const warehouse& inp, const std::string& instrs) {
        auto wh = inp;
        for (auto instr : instrs) {
            run_instruction(wh, instr);
        }
        return wh;
    }

    int score(const warehouse& wh) {
        return r::fold_left(
            wh.crates | rv::transform(
                [](auto&& pt) {
                    return pt.y * 100 + pt.x;
                }
            ),
            0,
            std::plus<>()
        );
    }
    
    warehouse make_double_wide(const warehouse& wh) {
        const static auto embiggen = [](const auto& pt)->point {
            return { 2 * pt.x, pt.y };
        };

        warehouse dw;
        dw.double_wide = true;
        dw.pos = embiggen(wh.pos);
        dw.walls = wh.walls | rv::transform(embiggen) | r::to<point_set>();
        dw.crates = wh.crates | rv::transform(embiggen) | r::to<point_set>();

        return dw;
    }
}

void aoc::y2024::day_15(const std::string& title) {

    auto [warehouse, instrs] = parse_input(
        aoc::file_to_string_vector(
            aoc::input_path(2024, 15)
        )
    );

    std::println("--- Day 15: {} ---", title);

    std::println("  part 1: {}", 
        score(
            run_instructions(warehouse, instrs)
        )
    );

    std::println("  part 2: {}", 
        score(
            run_instructions(make_double_wide(warehouse), instrs)
        )
    );
    
}
