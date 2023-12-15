#include "days.h"
#include "../util.h"
#include <print>
#include <ranges>
#include <numeric>
#include <algorithm>
#include <array>
#include <optional>
#include <tuple>

namespace r = std::ranges;
namespace rv = std::ranges::views;

namespace {

    std::vector<std::string> parse_input(const std::string& inp) {
        auto input = inp;
        if (input.back() == '\n') {
            input = input.substr(0, input.size() - 1);
        }
        return aoc::split(input, ',');
    }

    int hash_string(const std::string& str) {
        return r::fold_left(str, 0,
            [](int hash, char ch)->int {
                hash += ch;
                hash *= 17;
                hash = hash % 256;
                return hash;
            }
        );
    }

    struct lens {
        std::string label;
        int focal_length;
    };

    using elf_hash_table = std::array<std::vector<lens>, 256>;

    struct hash_op {
        std::string label;
        std::optional<int> val;
    };

    hash_op parse_hash_table_op(const std::string& str) {
        if (str.back() == '-') {
            auto label = str.substr(0, str.size() - 1);
            return { label, {} };
        }
        auto pieces = aoc::split(str, '=');
        return { pieces.front(), std::stoi(pieces.back()) };
    }

    elf_hash_table generate_table(const std::vector<std::string>& input) {
        auto ops = input | rv::transform(parse_hash_table_op) | r::to<std::vector<hash_op>>();
        elf_hash_table tbl;
        for (auto op : ops) {
            auto& box = tbl[hash_string(op.label)];
            if (op.val) {
                auto iter = r::find_if(box, [op](auto&& lens) {return lens.label == op.label; });
                if (iter == box.end()) {
                    box.emplace_back(op.label, *op.val);
                } else {
                    iter->focal_length = *op.val;
                }
            } else {
                auto iter = r::find_if(box, [op](auto&& lens) {return lens.label == op.label; });
                if (iter != box.end()) {
                    box.erase(iter);
                }
            }
        }
        return tbl;
    }

    int score_table(const elf_hash_table& tbl) {
        auto score_box = [](int box_index, const std::vector<lens>& box)->int {
            return r::fold_left(
                    rv::enumerate(box) | rv::transform(
                        [box_index](auto&& tup)->int {
                            const auto& [slot, item] = tup;
                            return (box_index + 1) * (slot + 1) * item.focal_length;
                        }
                    ),
                    0,
                    std::plus<>()
                );
            };
        return r::fold_left(
            rv::enumerate(tbl) |  
                rv::transform(
                    [score_box](auto&& tup)->int {
                        const auto& [box_index, box] = tup;
                        return score_box(box_index, box);
                    }
                ),
            0,
            std::plus<>()
        );
    }

    int do_part_2(const std::vector<std::string>& input) {
        auto tbl = generate_table(input);
        return score_table(tbl);
    }
}

/*------------------------------------------------------------------------------------------------*/

void aoc::y2023::day_15(const std::string& title) {

    auto input = parse_input(aoc::file_to_string(aoc::input_path(2023, 15)));

    std::println("--- Day 15: {0} ---\n", title);
    std::println("  part 1: {}",
        r::fold_left(
            input | rv::transform(hash_string),
            0, std::plus<>()
        )
    );
    std::println("  part 2: {}", do_part_2(input));

}