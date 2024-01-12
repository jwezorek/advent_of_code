#include "../util.h"
#include "y2020.h"
#include <filesystem>
#include <functional>
#include <print>
#include <ranges>
#include <unordered_map>
#include <unordered_set>
#include <sstream>
#include <stack>
#include <boost/intrusive/circular_list_algorithms.hpp>

namespace r = std::ranges;
namespace rv = std::ranges::views;

/*------------------------------------------------------------------------------------------------*/

namespace {
    struct circular_list_node {
        circular_list_node* next_;
        circular_list_node* prev_;
        int value;
    };

    struct circular_list_node_traits {
        using node = circular_list_node;
        using node_ptr = circular_list_node*;
        using const_node_ptr = const circular_list_node*;
        static node_ptr get_next(const_node_ptr n) { return n->next_; }
        static void set_next(node_ptr n, node_ptr next) { n->next_ = next; }
        static node* get_previous(const_node_ptr n) { return n->prev_; }
        static void set_previous(node_ptr n, node_ptr prev) { n->prev_ = prev; }
    };

    using circular = boost::intrusive::circular_list_algorithms<circular_list_node_traits>;

    class cup_circle {
        std::unordered_map<int, circular_list_node*> val_to_node_;
        std::vector<circular_list_node> nodes_;
        circular_list_node* current_;
        int lowest_;
        int highest_;

        static circular_list_node num_to_node(int v) {
            return { nullptr,nullptr,v };
        }

    public:
        cup_circle(const std::vector<int>& numbers) :
                nodes_(numbers | rv::transform(num_to_node)
                    | r::to<std::vector<circular_list_node>>()
                ) {

            circular::init_header(&nodes_.front());
            for (auto pair : rv::iota(0, length()) | rv::slide(2)) {
                circular::link_after(&nodes_[pair[0]], &nodes_[pair[1]]);
            }

            for (auto& node : nodes_) {
                val_to_node_[node.value] = &node;
            }

            current_ = &nodes_.front();
            lowest_ = r::min(
                nodes_ | rv::transform([](auto&& v) {return v.value; })
            );
            highest_ = r::max(
                nodes_ | rv::transform([](auto&& v) {return v.value; })
            );
        }

        void increment_curr() {
            current_ = current_->next_;
        }

        int curr_val() const {
            return current_->value;
        }

        int length() const {
            return static_cast<int>(nodes_.size());
        }

        int lowest() const {
            return lowest_;
        }

        int highest() const {
            return highest_;
        }

        std::string to_string(circular_list_node* start = nullptr) const {
            if (!start) {
                start = current_;
            }
            std::stringstream ss;
            circular_list_node* p = nullptr;
            while (p != start) {
                if (!p) {
                    p = start;
                }
                ss << p->value;
                p = p->next_;
            }
            return ss.str();
        }

        circular_list_node* pop_next() {
            auto* next = current_->next_;
            circular::unlink(next);
            return next;
        }
        
        circular_list_node* find(int val) {
            return val_to_node_[val];
        }
    };

    std::vector<int> string_to_ints(const std::string& str) {
        return str | rv::transform(
                [](char ch)->int {
                    return ch - '0';
                }
        ) | r::to<std::vector<int>>();
    }

    int next_dest_val(const std::unordered_set<int>& removed, const cup_circle& cup_circle) {
        int next = cup_circle.curr_val();
        do {
            if (--next < cup_circle.lowest()) {
                next = cup_circle.highest();
            }
        } while (removed.contains(next));
        return next;
    }

    void do_one_move(cup_circle& cup_circle) {
        std::unordered_set<int> removed;
        std::stack<circular_list_node*> stack;
        for (int i = 0; i < 3; ++i) {
            auto* cup = cup_circle.pop_next();
            stack.push(cup);
            removed.insert(cup->value);
        }

        circular_list_node* dest = nullptr;
        int dest_val = next_dest_val(removed, cup_circle);
        dest = cup_circle.find(dest_val);

        while (!stack.empty()) {
            auto* node = stack.top();
            stack.pop();
            circular::link_after(dest, node);
        }

        cup_circle.increment_curr();
    }

    std::string cup_game(cup_circle& cup_circle, int num_moves) {
        for (int i = 0; i < num_moves; ++i) {
            do_one_move(cup_circle);
        }
        auto* start = cup_circle.find(1);
        auto result = cup_circle.to_string(start);
        return result.substr(1, result.size() - 1);
    }

    int64_t do_part_2(const std::string& str) {
        auto nums = string_to_ints(str);
        nums.reserve(1000000);
        int highest = r::max(nums);
        for (int i = highest + 1; i <= 1000000; ++i) {
            nums.push_back(i);
        }
        cup_circle cups(nums);
        for (int i = 0; i < 10000000; ++i) {
            do_one_move(cups);
        }
        auto* start = cups.find(1);
        return static_cast<int64_t>(start->next_->value) *
            static_cast<int64_t>(start->next_->next_->value);
    }
}

void aoc::y2020::day_23(const std::string& title) {
    auto input = aoc::file_to_string(aoc::input_path(2020, 23));
    cup_circle cups(string_to_ints(input) );

    std::println("--- Day 23: {} ---", title);
    std::println("  part 1: {}", cup_game(cups, 100));
    std::println("  part 2: {}", do_part_2(input));
}