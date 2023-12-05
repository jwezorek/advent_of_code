#include "days.h"
#include "../util.h"
#include <print>
#include <ranges>
#include <tuple>
#include <algorithm>
#include <unordered_set>
#include <regex>
#include <map>

namespace r = std::ranges;
namespace rv = std::ranges::views;

namespace {
    struct map_range {
        int64_t dest_start;
        int64_t src_start;
        int64_t sz;

        bool in_range(int64_t val) const {
            return val >= src_start && val < src_start + sz;
        }

        int64_t value(int64_t val) const {
            return (val - src_start) + dest_start;
        }
    };

    class almanac_map {
        std::map<int, map_range> map_;
    public:
        almanac_map(const std::vector<map_range>& ranges) {
            for (const auto& rng : ranges) {
                map_[rng.src_start] = rng;
            }
        }

        int64_t value(int64_t val) const {
            auto iter = map_.lower_bound(val);
            if (iter == map_.begin()) {
                return val;
            }
            if (std::prev(iter)->second.in_range(val)) {
                return std::prev(iter)->second.value(val);
            }
            if (iter != map_.end() && iter->second.in_range(val)) {
                return iter->second.value(val);
            }
            return val;
        }
    };

    class almanac {

        std::vector<almanac_map> maps_;

    public:
        almanac(const std::vector<std::vector<map_range>>& map_ranges) {
            maps_ = map_ranges | rv::transform(
                [](auto&& ranges) {return almanac_map(ranges); }
            ) | r::to< std::vector<almanac_map>>();
        }

        int64_t process_seed(int64_t seed) const {
            for (const auto& map : maps_) {
                seed = map.value(seed);
            }
            return seed;
        }

        void debug() {
            const auto& map_0 = maps_[0];
            std::println("51 => {}", map_0.value(51));
            std::println("2 => {}", map_0.value(2));
            std::println("72 => {}", map_0.value(72));
            std::println("98 => {}", map_0.value(98));
            int aaa;
            aaa = 5;
        }
    };

    struct input_data {
        std::vector<int64_t> seeds;
        almanac almanac;
    };

    input_data parse_input(const std::vector<std::string>& lines) {
        auto maps = lines | rv::drop(2) | rv::filter(
            [](auto&& line)->bool {
                    return line.empty() || r::find_if(line, [](char ch) {return std::isdigit(ch); }) != line.end();
                }
            ) | rv::chunk_by(
                [](const std::string& str1, const std::string& str2)->bool {
                    return str1.size() != 0 && str2.size() != 0;
                }
            ) | rv::transform(
                [](auto rng)->std::vector<std::string> {
                    return rng | r::to< std::vector<std::string>>();
                }
            ) | rv::filter(
                [](auto&& vec) {
                    return vec.size() > 1 || !vec.front().empty();
                }
            ) | rv::transform(
                [](auto&& strings) {
                    return strings |
                        rv::transform(
                            [](const std::string& str)->map_range {
                                auto triple = aoc::extract_numbers_int64(str);
                                return {
                                    triple.at(0),
                                    triple.at(1),
                                    triple.at(2)
                                };
                            }
                        ) | r::to<std::vector<map_range>>();
                }
            ) | r::to<std::vector<std::vector<map_range>>>();

            return {
                 aoc::extract_numbers_int64(lines.front()),
                 almanac(maps)
            };
    }

    int64_t do_part_1(const std::vector<int64_t>& seeds, const almanac& almanac) {
        int64_t min = -1;
        for (auto seed : seeds) {
            auto val = almanac.process_seed(seed);
            if (min == -1 || val < min) {
                min = val;
            }
        }
        return min;
    }

    int64_t do_part_2(const std::vector<int64_t>& seeds, const almanac& almanac) {
        int64_t min = -1;
        for (auto rng : seeds | rv::chunk(2) ) {
            auto start = rng[0];
            auto sz = rng[1];

            for (int64_t i = 0; i < sz; ++i) {
                int64_t seed = start + i;
                auto val = almanac.process_seed(seed);
                if (min == -1 || val < min) {
                    min = val;
                }
            }
        }

        return min;
    }
}

/*------------------------------------------------------------------------------------------------*/

void aoc::y2023::day_05(const std::string& title) {

    auto inp = parse_input(aoc::file_to_string_vector(aoc::input_path(2023, 5)));
    
    
    std::println("--- Day 5: {0} ---\n", title);
    std::println("  part 1: {}", do_part_1(inp.seeds, inp.almanac));
    std::println("  part 2: {}", do_part_2(inp.seeds, inp.almanac));
}