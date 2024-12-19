
#include "../util/util.h"
#include "y2024.h"
#include <filesystem>
#include <functional>
#include <print>
#include <ranges>
#include <unordered_set>
#include <unordered_map>
#include <memory>
#include <stack>

namespace r = std::ranges;
namespace rv = std::ranges::views;

/*------------------------------------------------------------------------------------------------*/

namespace {

    using strings = std::vector<std::string>;

    std::tuple<strings, strings> parse_inp(const strings& inp) {
        auto groups = aoc::group_strings_separated_by_blank_lines(inp);
        return {
            aoc::extract_alphabetic(groups[0][0]),
            std::move(groups.back())
        };
    }

    int to_index(char color) {
        static const std::unordered_map<char, int> to_index_tbl = {
            {'w',0},
            {'u',1},
            {'b',2},
            {'r',3},
            {'g',4}
        };
        return to_index_tbl.at(color);
    }

    struct trie {
        char ch;
        std::array<std::shared_ptr<trie>, 5> children;
        bool is_terminal;

        trie() : ch(0), is_terminal(false) {
        }
    };

    using  str_iter = std::string::const_iterator;

    void insert_into_trie(trie& tri, str_iter& i, str_iter end) {
        if (i == end) {
            return;
        }
        auto ch = *i;
        auto ch_index = to_index(ch);
        if (! tri.children[ch_index]) {
            tri.children[ch_index] = std::make_shared<trie>();
        }
        auto& next_trie_node = *tri.children[ch_index];
        next_trie_node.ch = ch;
        next_trie_node.is_terminal = (next_trie_node.is_terminal || std::next(i) == end);

        ++i;
        insert_into_trie(next_trie_node, i, end);
    }

    void insert_into_trie(trie& trie, const std::string& str) {
        str_iter i = str.begin();
        insert_into_trie(trie, i, str.end());
    }

    bool has_word(const trie& tri, str_iter& i, str_iter end) {
        if (i == end) {
            return tri.is_terminal;
        }
        auto ch = *i;
        auto ch_index = to_index(ch);
        if (tri.children[ch_index]) {
            ++i;
            return has_word(*tri.children[ch_index], i, end);
        }
        return false;
    }

    bool has_word(const trie& tri, const std::string& str) {
        str_iter i = str.begin();
        return has_word(tri, i, str.end());
    }

    struct state {
        int i;
        const trie* trie_node;
    };

    bool test_design(const trie& tri, const std::string& str) {
        std::stack<state> stack;
        stack.emplace(
            0,
            &tri
        );
        while (!stack.empty()) {
            auto curr_state = stack.top();
            stack.pop();

            if (curr_state.i == str.size()) {
                return true;
            }

            if (curr_state.trie_node->is_terminal) {
                stack.emplace(
                    curr_state.i+1,
                    &tri
                );
            }

            auto ch = str[curr_state.i];
            if (curr_state.trie_node->children[to_index(ch)]) {
                stack.emplace(
                    curr_state.i + 1,
                    curr_state.trie_node->children[to_index(ch)].get()
                );
            }
        }

        return false;
    }
}

void aoc::y2024::day_19(const std::string& title) {

    auto [towels, designs] = parse_inp(
        aoc::file_to_string_vector(
            aoc::input_path(2024, 19)
        )
    );

    trie tri;
    for (const auto& towel : towels) {
        insert_into_trie(tri, towel);
    }

    std::println("u => {}", test_design(tri, "u"));

    std::println("--- Day 19: {} ---", title);
    std::println("  part 1: {}",
        r::count_if(
            designs,
            [&](const std::string& str) {
                return test_design(tri, str);
            }
        )
    );
    std::println("  part 2: {}", 0);
    
}
