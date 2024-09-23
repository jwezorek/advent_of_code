
#include "../util/util.h"
#include "y2017.h"
#include <filesystem>
#include <functional>
#include <print>
#include <ranges>
#include <unordered_map>

namespace r = std::ranges;
namespace rv = std::ranges::views;

/*------------------------------------------------------------------------------------------------*/

namespace {

    struct tower_node {
        std::string name;
        int weight;
        std::string parent;
        std::vector<std::string> children;
    };

    using tower = std::unordered_map<std::string, tower_node>;

    tower_node parse_tower_node(const std::string& str) {
        auto words = aoc::extract_alphanumeric(str);
        return {
            words[0],
            std::stoi(words[1]),
            "",
            words | rv::drop(2) | r::to<std::vector>()
        };
    }

    void insert_node(tower& tower, std::unordered_map<std::string,std::string>& parent, const tower_node& node) {
        tower[node.name] = node;
        if (parent.contains(node.name)) {
            tower[node.name].parent = parent.at(node.name);
        }
        for (const auto& child : node.children) {
            if (tower.contains(child)) {
                tower[child].parent = node.name;
            } else {
                parent[child] = node.name;
            }
        }
    }

    tower build_tower(const std::vector<tower_node>& nodes) {
        tower tower;
        std::unordered_map<std::string, std::string> parents;
        for (const auto& node : nodes) {
            insert_node(tower, parents, node);
        }
        return tower;
    }

    std::string tower_root(const tower& tower) {
        return r::find_if(tower,
            [](const auto& val)->bool {
                auto [name, node] = val;
                return node.parent.empty();
            }
        )->second.name;
    }

    int tower_weight(const tower& tower, const std::string& node_name) {
        const auto& node = tower.at(node_name);
        int weight = node.weight;
        for (const auto& child : node.children) {
            weight += tower_weight(tower, child);
        }
        return weight;
    }

    std::optional<int> odd_man_out_index(const std::vector<int>& weights) {
        std::unordered_map<int, std::vector<int>> groups;
        for (int i = 0; i < static_cast<int>(weights.size()); ++i) {
            groups[weights[i]].push_back(i);
        }
        if (groups.size() == 1) {
            return {};
        }
        if (groups.size() != 2) {
            throw std::runtime_error("invalid input: no one change will balances the tower");
        }
        return r::find_if(groups,
                [](const auto& kvp) {
                    const auto& [weight, indices] = kvp;
                    if (indices.size() == 1) {
                        return true;
                    }
                    return false;
                }
            )->second.front();
    }

    int balancing_weight(const tower& tower,  std::string root = {}, int target_weight = 0) {
        if (root.empty()) {
            root = tower_root(tower);
        }

        const auto& node = tower.at(root);
        auto child_weights = node.children | rv::transform(
                [&](const auto& child) {
                    return tower_weight(tower, child);
                }
            ) | r::to<std::vector>();

        auto bad_subtower_index = odd_man_out_index(child_weights);
        if (!bad_subtower_index) {
            return target_weight - child_weights.front() * child_weights.size();
        }

        return balancing_weight(
            tower, 
            node.children[*bad_subtower_index], 
            child_weights[(*bad_subtower_index + 1) % child_weights.size()]
        );
    }

}

void aoc::y2017::day_07(const std::string& title) {

    auto tower = build_tower(
        aoc::file_to_string_vector(
            aoc::input_path(2017, 7)
        ) | rv::transform(
            parse_tower_node
        ) | r::to<std::vector>()
    );

    std::println("--- Day 7: {} ---", title);
    std::println("  part 1: {}",
        tower_root(tower)
    );
    std::println("  part 2: {}", balancing_weight(tower));
    
}
