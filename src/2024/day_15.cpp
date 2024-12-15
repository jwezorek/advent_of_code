
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

    bool can_move_crate(warehouse& wh, const point& loc, const point& dir) {
        auto new_loc = loc + dir;
        if (wh.walls.contains(new_loc)) {
            return false;
        } else if (wh.crates.contains(new_loc)) {
            return can_move_crate(wh, new_loc, dir);
        }
        return true;
    }

    bool move_crate(warehouse& wh, const point& loc, const point& dir) {

        if (!can_move_crate(wh, loc, dir)) {
            return false;
        }

        auto new_loc = loc + dir;
        if (wh.crates.contains(new_loc)) {
            if (!move_crate(wh, new_loc, dir)) {
                return false;
            }
        }

        wh.crates.erase(loc);
        wh.crates.insert(new_loc);
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
        if (wh.walls.contains(new_pos)) {
            return;
        }
        if (wh.crates.contains(new_pos)) {
            if (!move_crate(wh, new_pos, dir)) {
                return;
            }
        }
        wh.pos = new_pos;
    }

    void display(const warehouse& wh) {
        int wd = r::max(wh.walls | rv::transform([](auto&& p) {return p.x; })) + 1;
        int hgt = r::max(wh.walls | rv::transform([](auto&& p) {return p.y; })) + 1;

        if (wh.double_wide) {
            wd++;
        }

        auto grid = std::vector<std::string>( hgt, std::string(wd, '.') );
        for (int y = 0; y < hgt; ++y) {
            for (int x = 0; x < wd; ++x) {
                auto loc = point{ x,y };
                std::string tile;
                if (wh.pos == loc) {
                    tile = "@";
                } else if (wh.walls.contains(loc)) {
                    tile = (wh.double_wide) ? "##" : "#";
                }
                else if (wh.crates.contains(loc)) {
                    tile = (wh.double_wide) ? "[]" : "O";
                }
                for (int i = 0; i < tile.size(); ++i) {
                    grid[y][x + i] = tile[i];
                }
            }
        }

        for (const auto& row : grid) {
            std::println("{}", row);
        }
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
            aoc::input_path(2024, 15, "test2")
        )
    );

    display(warehouse);
    std::println("");
    display(make_double_wide(warehouse));

    std::println("--- Day 15: {} ---", title);
    std::println("  part 1: {}", score(run_instructions(warehouse, instrs)));
    std::println("  part 2: {}", 0);
    
}
