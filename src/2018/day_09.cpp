
#include "../util/util.h"
#include "y2018.h"
#include <filesystem>
#include <functional>
#include <print>
#include <ranges>
#include <unordered_set>
#include <memory>
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

    class node_store {
        std::vector<std::unique_ptr<circular_list_node>> store_;
        std::unordered_map<circular_list_node*, int> node_to_index_;
    public:
        circular_list_node* make_node(int64_t val) {
            int index = static_cast<int>(store_.size());
            store_.push_back(std::make_unique<circular_list_node>(nullptr, nullptr, val));
            node_to_index_[store_.back().get()] = index;
            return store_.back().get();
        }

        void remove_node(circular_list_node* node) {
            int index = node_to_index_[node];
            node_to_index_.erase(node);
            auto* replacement = store_.back().get();
            std::swap(store_[index], store_.back());
            store_.pop_back();
            node_to_index_[replacement] = index;
        }
    };

    class marble_circle {
        circular_list_node* current_;
        node_store nodes_;
    public:

        marble_circle() {
            auto first_node = nodes_.make_node(0);
            circular::init_header(first_node);
            current_ = first_node;
        }

        void insert(int64_t val) {
            auto new_node = nodes_.make_node(val);
            current_ = current_->next_;
            circular::link_after(current_, new_node);
            current_ = current_->next_;
        }

        int64_t remove() {
            auto removee = current_;
            for (int i = 0; i < 7; ++i) {
                removee = removee->prev_;
            }
            current_ = removee->next_;
            circular::unlink(removee);
            auto val = removee->value;
            nodes_.remove_node(removee);

            return val;
        }

        void display() const {
            for (auto* p = current_->next_; p != current_; p = p->next_) {
                std::print("{} ", p->value);
            }
            std::println("{}", current_->value);
        }
    };

    int64_t play_game(int64_t num_players, int64_t last_marble) {
        std::vector<int64_t> scores(num_players, 0);
        int64_t player = 0;
        marble_circle marbles;
        for (int64_t marble = 1; marble <= last_marble; ++marble) {
            if (marble % 23 != 0) {
                marbles.insert(marble);
            } else {
                scores[player] += marble;
                scores[player] += marbles.remove();
            }
            player = (player + 1) % num_players;
        }
        return r::max(scores);
    }
}

void aoc::y2018::day_09(const std::string& title) {


    auto inp = aoc::extract_numbers_int64(
        aoc::file_to_string(
            aoc::input_path(2018, 9)
        )
    );

    std::println("--- Day 9: {} ---", title);
    std::println("  part 1: {}", play_game(inp[0], inp[1]));
    std::println("  part 2: {}", play_game(inp[0], 100 * inp[1]));
    
}
