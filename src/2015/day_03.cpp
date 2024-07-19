#include "../util/util.h"
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
    struct point {
        int x;
        int y;

        bool operator==(const point& p) const {
            return x == p.x && y == p.y;
        }
    };

    point operator+(const point& lhs, const point& rhs) {
        return  { lhs.x + rhs.x, lhs.y + rhs.y };
    }

    struct point_hash {
        size_t operator()(const point& pt) const {
            size_t seed = 0;
            boost::hash_combine(seed, pt.x);
            boost::hash_combine(seed, pt.y);
            return seed;
        }
    };

    using point_set = std::unordered_set<point, point_hash>;

    point tile_to_delta(char tile) {
        static const std::unordered_map<char, point> tile_to_delta_tbl = {
            {'^', {0,-1}}, {'v', {0,1}}, {'>', {1, 0}}, {'<', {-1,0}}
        };
        return tile_to_delta_tbl.at(tile);
    }

    point_set visited_locations(auto directions) {
        point_set visited;
        point loc = { 0,0 };
        visited.insert(loc);
        for (auto dir : directions) {
            loc = loc + tile_to_delta(dir);
            visited.insert(loc);
        }
        return visited;
    }

    int visited_by_two_santas(const std::string& directions) {
        auto set = visited_locations(directions | rv::stride(2));
        for (auto pt : visited_locations(directions | rv::drop(1) | rv::stride(2))) {
            set.insert(pt);
        }
        return set.size();
    }
}

void aoc::y2015::day_03(const std::string& title) {

    auto directions = aoc::file_to_string(aoc::input_path(2015, 3));

    std::println("--- Day 3: {} ---", title);
    std::println("  part 1: {}",
        visited_locations( rv::all(directions) ).size()
    );
    std::println("  part 2: {}",
        visited_by_two_santas( directions )
    );
}