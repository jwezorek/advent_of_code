
#include "../util/util.h"
#include "y2024.h"
#include <filesystem>
#include <functional>
#include <print>
#include <ranges>
#include <unordered_set>

namespace r = std::ranges;
namespace rv = std::ranges::views;

/*------------------------------------------------------------------------------------------------*/

namespace {
    struct run {
        int64_t id;
        int64_t sz;
    };

    struct disk_map {
        std::vector<run> blocks;
        std::vector<int64_t> spaces;
    };

    disk_map parse_input(const std::string& inp) {
        auto nums = inp | rv::filter(
                [](char ch) {
                    return ch >= '0' && ch <= '9';
                }
            ) | rv::transform(
                [](char ch)->int64_t {
                    return ch - '0';
                }
            ) | r::to<std::vector>();

        disk_map output;
        output.blocks.emplace_back(0, nums.front());
        for (auto [id, rng] : rv::enumerate(nums | rv::drop(1) | rv::chunk(2)) ) {
            output.spaces.emplace_back(rng[0]);
            output.blocks.emplace_back(id + 1, rng[1]);
        }

        return output;
    }

    void pack_next_space(std::vector<run>& packing, int64_t space_sz, disk_map& dm) {
        while (space_sz > 0) {
            if (dm.blocks.back().sz > space_sz) {
                auto& curr_block = dm.blocks.back();
                curr_block.sz -= space_sz;
                packing.emplace_back(curr_block.id, space_sz);
                space_sz = 0;
            } else {
                auto& curr_block = dm.blocks.back();
                packing.emplace_back(curr_block);
                space_sz -= curr_block.sz;
                dm.blocks.pop_back();
                dm.spaces.pop_back();
            }
        }
    }

    disk_map pack_disk_map(const disk_map& inp) {
        auto dm = inp;
        std::vector<run> packing;

        int64_t curr_block = 0;
        int64_t curr_space = 0;

        while (curr_space < dm.spaces.size()) {
            packing.push_back(dm.blocks[curr_block++]);
            pack_next_space(packing, dm.spaces[curr_space++], dm);
        }

        if (curr_block != dm.blocks.size() - 1) {
            throw std::runtime_error("this shouldnt happen");
        }

        if (packing.back().id == dm.blocks.back().id) {
            packing.back().sz += dm.blocks.back().sz;
        } else {
            packing.emplace_back(dm.blocks.back());
        }

        return {
            packing,
            std::vector<int64_t>(packing.size() - 1, 0)
        };
    }

    void print_packing(const std::vector<run>& runs) {
        for (const auto& run : runs) {
            auto str = std::string(run.sz, run.id + '0');
            std::print("{}", str);
        }
        std::println("");
    }

    int64_t check_sum(const disk_map& dm) {
        int64_t chksum = 0;
        int64_t i = 0;
        auto space_iter = dm.spaces.begin();
        for (const auto& run : dm.blocks) {
            for (int64_t j = 0; j < run.sz; ++j) {
                chksum += i++ * run.id;
            }
            if (space_iter != dm.spaces.end()) {
                i += *(space_iter++);
            }
        }
        return chksum;
    }

    disk_map pack_disk_map_whole_files(const disk_map& inp) {


    }
}

void aoc::y2024::day_09(const std::string& title) {

    auto inp = parse_input(
        aoc::file_to_string(
            aoc::input_path(2024, 9)
        )
    ); 

    std::println("--- Day 9: {} ---", title);
    std::println("  part 1: {}", check_sum(pack_disk_map(inp)) );
    std::println("  part 2: {}", 0);
    
}
