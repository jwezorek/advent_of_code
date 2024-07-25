
#include "../util/util.h"
#include "y2018.h"
#include <filesystem>
#include <functional>
#include <print>
#include <ranges>
#include <unordered_set>
#include <memory>

namespace r = std::ranges;
namespace rv = std::ranges::views;

/*------------------------------------------------------------------------------------------------*/

namespace {
    using iterator = std::vector<int>::const_iterator;

    struct node;
    using node_ptr = std::shared_ptr<node>;

    struct node {
        std::vector<node_ptr> children;
        std::vector<int> metadata;
    };

    node_ptr parse_node(iterator& iter) {
        auto new_node = std::make_shared<node>();
        auto num_children = *(iter++);
        auto num_metadata = *(iter++);
        for (int i = 0; i < num_children; ++i) {
            new_node->children.push_back(
                parse_node(iter)
            );
        }
        for (int i = 0; i < num_metadata; ++i) {
            new_node->metadata.push_back(
                *(iter++)
            );
        }
        return new_node;
    }

    node_ptr parse(const std::vector<int>& vals) {
        auto iter = vals.begin();
        return parse_node(iter);
    }

    int sum_of_metadata(node_ptr node) {
        return r::fold_left(
                node->children | rv::transform([](auto ptr) { return sum_of_metadata(ptr); }),
                r::fold_left(node->metadata, 0, std::plus<>()),
                std::plus<>()
            );
    }

    int value_of_node(node_ptr node) {
        int value = 0;
        if (node->children.empty()) {
            value = r::fold_left(node->metadata, 0, std::plus<>());
        } else {
            std::unordered_map<int, int> memos;
            for (auto index : node->metadata | rv::transform([](auto i){return i-1;})) {
                if (index >= node->children.size()) {
                    continue;
                }
                if (memos.contains(index)) {
                    value += memos.at(index);
                } else {
                    auto node_val = value_of_node(node->children[index]);
                    memos[index] = node_val;
                    value += node_val;
                }
            }
        }
        return value;
    }

}

void aoc::y2018::day_08(const std::string& title) {

    auto inp = aoc::extract_numbers(
        aoc::file_to_string(
            aoc::input_path(2018, 8)
        )
    );

    auto root = parse(inp);

    std::println("--- Day 8: {} ---", title);
    std::println("  part 1: {}", sum_of_metadata(root) );
    std::println("  part 2: {}", value_of_node(root) );
    
}
