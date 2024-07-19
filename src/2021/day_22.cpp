#include "../util/util.h"
#include "y2021.h"
#include <filesystem>
#include <functional>
#include <print>
#include <ranges>
#include <set>
#include <regex>
#include <boost/multi_array.hpp>

namespace r = std::ranges;
namespace rv = std::ranges::views;

/*------------------------------------------------------------------------------------------------*/

namespace {

    struct cuboid {
        bool on;
        int x1, x2, y1, y2, z1, z2;

        cuboid()
        {}

        cuboid(bool on, int x1, int x2, int y1, int y2, int z1, int z2) {
            this->on = on;
            std::tie(this->x1, this->x2) = { x1, x2 };
            std::tie(this->y1, this->y2) = { y1, y2 };
            std::tie(this->z1, this->z2) = { z1, z2 };
        }
    };

    std::vector<cuboid> parse_cube_ops(const std::vector<std::string>& lines) {
        std::vector<cuboid> ops(lines.size());
        std::transform(lines.begin(), lines.end(), ops.begin(),
            [](const std::string& str)->cuboid {
                cuboid op;
                bool on = str.substr(0, 2) == "on";
                auto coords = str.substr((op.on) ? 3 : 4);

                std::smatch res;
                std::string::const_iterator iter(coords.cbegin());
                std::vector<int> nums;
                while (std::regex_search(iter, coords.cend(), res, std::regex("-?[[:digit:]]+"))) {
                    nums.push_back(std::stoi(res[0]));
                    iter = res.suffix().first;
                }

                if (nums.size() != 6) {
                    throw std::runtime_error("bad input");
                }

                return cuboid(on, nums[0], nums[1], nums[2], nums[3], nums[4], nums[5]);
            }
        );
        return ops;
    }

    class index_map {
        std::vector<int> index_to_loc_;
        std::unordered_map<int, int> loc_to_index_;
    public:
        index_map(const std::set<int>& locs) {
            int index = 0;
            for (auto loc : locs) {
                index_to_loc_.push_back(loc);
                loc_to_index_[loc] = index++;
            }
        }
        int loc_to_index(int loc) const {
            return loc_to_index_.at(loc);
        }

        int index_to_loc(int index) const {
            return index_to_loc_[index];
        }
    };

    uint64_t do_cube_ops(const std::vector<cuboid>& cubes) {
        std::set<int> x_coords;
        std::set<int> y_coords;
        std::set<int> z_coords;

        for (const auto& c : cubes) {
            x_coords.insert(c.x1);
            x_coords.insert(c.x2 + 1);
            y_coords.insert(c.y1);
            y_coords.insert(c.y2 + 1);
            z_coords.insert(c.z1);
            z_coords.insert(c.z2 + 1);
        }

        index_map x_map(x_coords);
        index_map y_map(y_coords);
        index_map z_map(z_coords);

        boost::multi_array<int, 3> ary(boost::extents[x_coords.size()][y_coords.size()][z_coords.size()]);
        for (const auto& c : cubes) {
            int x1 = x_map.loc_to_index(c.x1);
            int x2 = x_map.loc_to_index(c.x2 + 1) - 1;
            int y1 = y_map.loc_to_index(c.y1);
            int y2 = y_map.loc_to_index(c.y2 + 1) - 1;
            int z1 = z_map.loc_to_index(c.z1);
            int z2 = z_map.loc_to_index(c.z2 + 1) - 1;

            for (int z = z1; z <= z2; ++z) {
                for (int y = y1; y <= y2; ++y) {
                    for (int x = x1; x <= x2; ++x) {
                        ary[x][y][z] = c.on ? 1 : 0;
                    }
                }
            }
        }

        uint64_t volume = 0;
        for (int z = 0; z < z_coords.size(); ++z) {
            for (int y = 0; y < y_coords.size(); ++y) {
                for (int x = 0; x < x_coords.size(); ++x) {
                    if (ary[x][y][z]) {
                        uint64_t xx = x_map.index_to_loc(x + 1) - x_map.index_to_loc(x);
                        uint64_t yy = y_map.index_to_loc(y + 1) - y_map.index_to_loc(y);
                        uint64_t zz = z_map.index_to_loc(z + 1) - z_map.index_to_loc(z);
                        volume += xx * yy * zz;
                    }
                }
            }
        }

        return volume;
    }

    size_t do_part_1(const std::vector<cuboid>& full_input) {
        std::vector<cuboid> cubes;
        std::copy_if(full_input.begin(), full_input.end(), std::back_inserter(cubes),
            [](const cuboid& c) {
                return (std::abs(c.x1) <= 50 && std::abs(c.x2) <= 50 &&
                    std::abs(c.y1) <= 50 && std::abs(c.y2) <= 50 &&
                    std::abs(c.z1) <= 50 && std::abs(c.z2) <= 50);
            }
        );
        return do_cube_ops(cubes);
    }
}

void aoc::y2021::day_22(const std::string& title) {
    auto cubes = parse_cube_ops(
        aoc::file_to_string_vector(aoc::input_path(2021, 22))
    );

    std::println("--- Day 22: {} ---", title);
    std::println("  part 1: {}", do_part_1(cubes));
    std::println("  part 2: {}", do_cube_ops(cubes));
}