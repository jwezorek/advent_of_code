#include "../util.h"
#include "y2022.h"
#include <filesystem>
#include <functional>
#include <print>
#include <ranges>
#include <boost/intrusive/circular_list_algorithms.hpp>

namespace r = std::ranges;
namespace rv = std::ranges::views;

/*------------------------------------------------------------------------------------------------*/


namespace {

    struct circular_list_node {
        circular_list_node* next_;
        circular_list_node* prev_;
        int64_t value;
    };

    struct circular_list_node_traits
    {
        using node = circular_list_node;
        using node_ptr = circular_list_node*;
        using const_node_ptr = const circular_list_node*;
        static node_ptr get_next(const_node_ptr n) { return n->next_; }
        static void set_next(node_ptr n, node_ptr next) { n->next_ = next; }
        static node* get_previous(const_node_ptr n) { return n->prev_; }
        static void set_previous(node_ptr n, node_ptr prev) { n->prev_ = prev; }
    };

    using circular = boost::intrusive::circular_list_algorithms<circular_list_node_traits>;

    class mix_list {
        std::vector<circular_list_node> nodes_;

        static circular_list_node num_to_node(int64_t v) {
            return { nullptr,nullptr,v };
        }

    public:
        mix_list(const std::vector<int64_t>& numbers) :
            nodes_(numbers | rv::transform(num_to_node) | r::to<std::vector<circular_list_node>>())
        {
            circular::init_header(&nodes_.front());
            for (auto pair : rv::iota(0, length()) | rv::slide(2)) {
                circular::link_after(&nodes_[pair[0]], &nodes_[pair[1]]);
            }
        }

        int length() const {
            return static_cast<int>(nodes_.size());
        }

        void mix(int i) {
            auto n = length();
            auto distance = nodes_[i].value % static_cast<int64_t>(n - 1);
            if (distance > 0) {
                circular::move_backwards(&nodes_[i], distance);
            }
            else if (distance < 0) {
                circular::move_forward(&nodes_[i], std::abs(distance));
            }
        }

        std::vector<int64_t> values() const {
            auto zero_item = r::find_if(
                nodes_, [](auto&& node) {return node.value == 0; }
            );
            struct generator {
                const circular_list_node* ptr;
                int64_t operator()() {
                    auto val = ptr->value;
                    ptr = ptr->next_;
                    return val;
                }
            };
            std::vector<int64_t> vals;
            auto gen = generator{ &(*zero_item) };

            // no views::generator in C++23 so do this the old
            // fashioned way...

            for (int i = 0; i < length(); ++i) {
                vals.push_back(gen());
            }
            return vals;
        }
    };

    int64_t mix(const std::vector<int64_t>& numbers, int n) {

        mix_list list(numbers);
        for (int j = 0; j < n; ++j) {
            for (int i = 0; i < list.length(); ++i) {
                list.mix(i);
            }
        }

        auto vec = list.values();
        std::array<int64_t, 3> indices = { {1000,2000,3000} };
        return r::fold_left(
            indices | rv::transform(
                [&vec](int64_t i)->int64_t {
                    return vec[i % vec.size()];

                }
            ),
            static_cast<int64_t>(0),
            std::plus<int64_t>()
        );
    }
}

void aoc::y2022::day_20(const std::string& title) {
    auto input = file_to_string_vector(input_path(2022, 20));

    auto part1_input = input |
        rv::transform(
            [](auto&& str)->int64_t {return std::stoi(str); }
        ) | r::to<std::vector<int64_t>>();

    auto part2_input = part1_input |
        rv::transform(
            [](auto v)->int64_t { return v * 811589153; }
        ) | r::to<std::vector<int64_t>>();

    std::println("--- Day 20: {} ---", title);
    std::println("  part 1: {}",
        mix(part1_input, 1)
    );
    std::println("  part 2: {}",
        mix(part2_input, 10)
    );
}