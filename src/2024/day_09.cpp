
#include "../util/util.h"
#include "y2024.h"
#include <filesystem>
#include <functional>
#include <print>
#include <ranges>
#include <map>
#include <unordered_set>
#include <list>

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

        struct node {
            int addr;
            file file;
        };

        using iterator = std::list<node>::const_iterator;

    private:

        std::list<node> impl_;

    public:

        void insert_at_end(int space, int id, int sz) {
            auto addr = tail_addr() + space;
            impl_.emplace_back(addr, file{ id,sz });
        }

        void insert(iterator prev, int id, int sz) {
            if (prev == impl_.end()) {
                impl_.emplace_back(tail_addr(), file{ id,sz });
                return;
            }

            if (next_space_sz(prev) < sz) {
                throw std::runtime_error("bad insert: back collision");
            }

            int addr = prev->addr + prev->file.sz;
            impl_.insert(std::next(prev), { addr, file{ id,sz } });
        }

        int tail_addr() {

            if (impl_.empty()) {
                return 0;
            }

            auto last = std::prev(impl_.end());
            return last->addr + last->file.sz;
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
            auto back = back_iter->file;
            impl_.erase(back_iter);
            return back;
        }
       
        int next_space_sz(iterator i) const {
            if (i == impl_.end() || i == std::prev(impl_.end())) {
                return -1;
            }
            int end_of_file = i->addr + i->file.sz;
            return std::next(i)->addr - end_of_file;
        }

        int64_t check_sum() const {
            return r::fold_left(
                impl_ | rv::transform(
                    [](auto&& node)->int64_t {
                        return r::fold_left(
                            rv::iota(
                                node.addr, node.addr + node.file.sz
                            ) | rv::transform(
                                [&](int addr)->int64_t {
                                    return addr * node.file.id;
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

        int inital_file_sz = str.front() - '0';
        dm.insert_at_end(0, 0, inital_file_sz);
        auto iter = dm.begin();
        int id = 1;
        for (const auto& pair : str | rv::drop(1) | rv::chunk(2)) {
            int space_sz = pair[0] - '0';
            int run_sz = pair[1] - '0';
            dm.insert_at_end(space_sz, id++, run_sz);
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
            auto tail = dm.pop_back();
            if (tail.sz <= space_sz) {
                dm.insert(curr_block, tail.id, tail.sz);
                ++curr_block;
                continue;
            }
            dm.insert(curr_block, tail.id, space_sz);
            dm.insert_at_end(0, tail.id, tail.sz - space_sz);

            ++curr_block;
        }
    }

    void pack_by_file(disk_map& dm) {
        std::unordered_set<int> packed;
        auto iter = std::prev(dm.end());
        while (iter != dm.begin()) {

            if (packed.contains(iter->file.id)) {
                iter = std::prev(iter);
                continue;
            }
            packed.insert(iter->file.id);

            bool moved = false;
            for (auto space_iter = dm.begin(); space_iter != iter; ++space_iter) {
                if (dm.next_space_sz(space_iter) >= iter->file.sz) {
                    auto file = iter->file;
                    auto new_iter = std::prev(iter);
                    dm.erase(iter);
                    iter = new_iter;
                    dm.insert(space_iter, file.id, file.sz);
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
