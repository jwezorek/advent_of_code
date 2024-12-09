
#include "../util/util.h"
#include "y2024.h"
#include <filesystem>
#include <functional>
#include <print>
#include <ranges>
#include <map>
#include <set>
#include <unordered_set>

namespace r = std::ranges;
namespace rv = std::ranges::views;

/*------------------------------------------------------------------------------------------------*/

namespace {

    class disk_map {
    public:

        struct file {
            int id;
            int sz;
        };

        using iterator = std::map<int, file>::const_iterator;

    private:

        std::map<int, file> impl_;

    public:

        void insert_file(int addr, int id, int sz) {
            if (addr >= tail_addr()) {
                impl_[addr] = { id, sz };
                return;
            }

            if (impl_.contains(addr)) {
                throw std::runtime_error("bad insert: front collision");
            }

            auto iter = std::prev(impl_.lower_bound(addr));
            if (next_space_sz(iter) < sz) {
                throw std::runtime_error("bad insert: back collision");
            }

            impl_[addr] = { id,sz };
        }

        int tail_addr() {

            if (impl_.empty()) {
                return -1;
            }

            auto last = std::prev(impl_.end());
            return last->first + last->second.sz;
        }

        iterator begin() const {
            return impl_.begin();
        }

        iterator end() const {
            return impl_.end();
        }

        void erase(iterator i) {
            impl_.erase(i);
        }

        auto pop_back() {
            auto back_iter = std::prev(impl_.end());
            auto back = back_iter->second;
            impl_.erase(back_iter);
            return back;
        }
       
        int next_space_sz(iterator i) const {
            if (i == impl_.end() || i == std::prev(impl_.end())) {
                return -1;
            }
            int end_of_file = i->first + i->second.sz;
            return std::next(i)->first - end_of_file;
        }

        int64_t check_sum() const {
            return r::fold_left(
                impl_ | rv::transform(
                    [](auto&& run)->int64_t {
                        return r::fold_left(
                            rv::iota(
                                run.first, run.first + run.second.sz
                            ) | rv::transform(
                                [&](int addr)->int64_t {
                                    return addr * run.second.id;
                                }
                            ),
                            0ll,
                            std::plus<int64_t>()
                        );
                    }
                ),
                0ll,
                std::plus<int64_t>()
            );
        }
    };

    disk_map parse_input(const std::string& inp) {
        auto str = (inp.back() == '\n') ? inp.substr(0, inp.size() - 1) : inp;
        disk_map dm;

        int inital_block_sz = str.front() - '0';
        dm.insert_file(0, 0, inital_block_sz);
        int addr = inital_block_sz;
        int id = 1;
        for (const auto& pair : str | rv::drop(1) | rv::chunk(2)) {
            int space_sz = pair[0] - '0';
            int run_sz = pair[1] - '0';
            addr += space_sz;
            dm.insert_file(addr, id++, run_sz);
            addr += run_sz;
        }

        return dm;
    }

    void pack_by_block(disk_map& dm) {
        auto curr_block = dm.begin();
        while (dm.next_space_sz(curr_block) != -1) {

            auto space_sz = dm.next_space_sz(curr_block);
            if (space_sz == 0) {
                ++curr_block;
                continue;
            }
            auto space_addr = curr_block->first + curr_block->second.sz;
            auto tail = dm.pop_back();
            if (tail.sz <= space_sz) {
                dm.insert_file(space_addr, tail.id, tail.sz);
                ++curr_block;
                continue;
            }
            dm.insert_file(space_addr, tail.id, space_sz);
            dm.insert_file(dm.tail_addr(), tail.id, tail.sz - space_sz);

            ++curr_block;
        }
    }

    void pack_by_file(disk_map& dm) {
        std::unordered_set<int> packed;
        auto iter = std::prev(dm.end());
        while (iter != dm.begin()) {

            if (packed.contains(iter->second.id)) {
                iter = std::prev(iter);
                continue;
            }
            packed.insert(iter->second.id);

            bool moved = false;
            for (auto space_iter = dm.begin(); space_iter != iter; ++space_iter) {
                if (dm.next_space_sz(space_iter) >= iter->second.sz) {
                    auto file = iter->second;
                    auto new_iter = std::prev(iter);
                    dm.erase(iter);
                    iter = new_iter;
                    dm.insert_file(space_iter->first + space_iter->second.sz, file.id, file.sz);
                    moved = true;
                    break;
                }
            }

            if (!moved) {
                iter = std::prev(iter);
            }
        }
    }

    int64_t packed_by_block_checksum(const disk_map& inp) {
        auto disk_map = inp;
        pack_by_block(disk_map);
        return disk_map.check_sum();
    }

    int64_t packed_by_file_checksum(const disk_map& inp) {
        auto disk_map = inp;
        pack_by_file(disk_map);
        return disk_map.check_sum();
    }
}

void aoc::y2024::day_09(const std::string& title) {

    auto disk_map = parse_input(
        aoc::file_to_string(
            aoc::input_path(2024, 9)
        )
    ); 

    std::println("--- Day 9: {} ---", title);

    std::println("  part 1: {}", packed_by_block_checksum(disk_map) );
    std::println("  part 2: {}", packed_by_file_checksum(disk_map));
    
}
