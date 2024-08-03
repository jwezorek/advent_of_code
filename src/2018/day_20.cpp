
#include "../util/util.h"
#include "../util/vec2.h"
#include "y2018.h"
#include <boost/functional/hash.hpp>
#include <filesystem>
#include <functional>
#include <print>
#include <ranges>
#include <unordered_set>
#include <memory>
#include <stack>
#include <queue>

namespace r = std::ranges;
namespace rv = std::ranges::views;

/*------------------------------------------------------------------------------------------------*/

namespace {

    struct graph_node;
    using node_ptr = std::shared_ptr<graph_node>;

    struct graph_node {
        char direction;
        std::vector<node_ptr> adj_list;

        graph_node(char dir = {}) : direction(dir)
        {}
    };

    struct digraph {
        node_ptr src;
        node_ptr dst;
    };

    class regex_node {
    public:
        virtual std::string to_string() const = 0;
        virtual digraph to_graph() const = 0;
    };

    using regex_ptr = std::shared_ptr<regex_node>;

    class regex : public regex_node {
        regex_ptr child_;
    public:
        regex(regex_ptr child) : child_(child)
        {}

        std::string to_string() const override {
            return child_->to_string();
        }

        digraph to_graph() const override {
            return child_->to_graph();
        }
    };

    class disjunction : public regex_node {
        std::vector<regex_ptr> children_;
        bool has_eps_trans_;
    public:
        disjunction(const std::vector<regex_ptr>& nodes, bool has_eps) :
            children_(nodes), has_eps_trans_(has_eps)
        {}

        std::string to_string() const override {
            std::stringstream ss;
            ss << "( ";
            for (int i = 0; i < children_.size(); ++i) {
                ss << children_[i]->to_string();
                if (i < children_.size() - 1) {
                    ss << " | ";
                }
            }
            if (has_eps_trans_) {
                ss << "*";
            }
            ss << " )";
            return ss.str();
        }

        digraph to_graph() const override {
            digraph g{
                std::make_shared<graph_node>(),
                std::make_shared<graph_node>()
            };
            for (auto child : children_) {
                auto subgraph = child->to_graph();
                g.src->adj_list.push_back(subgraph.src);
                subgraph.dst->adj_list.push_back(g.dst);
            }

            if (has_eps_trans_) {
                g.src->adj_list.push_back(g.dst);
            }

            return g;
        }

    };

    class sequence : public regex_node {
        std::vector<regex_ptr> children_;
    public:
        sequence(const std::vector<regex_ptr>& nodes) :
            children_(nodes)
        {}

        std::string to_string() const override {
            std::stringstream ss;
            for (int i = 0; i < children_.size(); ++i) {
                ss << children_[i]->to_string();
                ss << " ";
            }
            return ss.str();
        }

        digraph to_graph() const override {
            digraph g;
            node_ptr prev;

            digraph subgraph;
            for (auto child : children_) {
                subgraph = child->to_graph();
                if (prev) {
                    prev->adj_list.push_back(subgraph.src);
                } else {
                    g.src = subgraph.src;
                }
                prev = subgraph.dst;
            }

            g.dst = subgraph.dst;
            return g;
        }

    };

    class text : public regex_node {
        std::string txt_;
    public:
        text(const std::string& txt) :
            txt_(txt)
        {}

        std::string to_string() const override {
            return txt_;
        }

        digraph to_graph() const override {
            digraph g;
            node_ptr prev;

            node_ptr dir_node;
            for (auto dir : txt_) {
                dir_node = std::make_shared<graph_node>(dir);
                if (prev) {
                    prev->adj_list.push_back(dir_node);
                } else {
                    g.src = dir_node;
                }
                prev = dir_node;
            }

            g.dst = dir_node;
            return g;
        }

    };

    using iterator = std::string::iterator;

    bool parse_char(iterator& i, const iterator& end, char ch) {
        if (i == end) {
            return false;
        }
        if (*i == ch) {
            ++i;
            return true;
        }
        return false;
    }

    regex_ptr parse(iterator& i, const iterator& end);

    regex_ptr parse_regex(iterator& i, const iterator& end) {
        if (i == end) {
            return {};
        }

        if (!parse_char(i, end, '^')) {
            return {};
        }
        auto contents = parse(i, end);
        if (!contents) {
            return {};
        }
        if (!parse_char(i, end, '$')) {
            return {};
        }

        return std::make_shared<regex>(contents);
    }

    regex_ptr  parse_text(iterator& i, const iterator& end) {
        if (i == end) {
            return {};
        }
        std::stringstream ss;
        while (i != end && std::isalpha(*i)) {
            ss << *i;
            i++;
        }
        auto txt = ss.str();
        if (txt.empty()) {
            return {};
        }
        return std::make_shared<text>(txt);
    }

    regex_ptr parse_sequence(iterator& i, const iterator& end) {

        if (i == end || *i == ')' || *i == '|' || *i == '$') {
            return {};
        }

        std::vector<regex_ptr> contents;
        regex_ptr reg;

        do {
            if (i != end && std::isalpha(*i)) {
                reg = parse_text(i, end);
                if (reg) {
                    contents.push_back(reg);
                }
                continue;
            }

            reg = parse(i, end);
            if (reg) {
                contents.push_back(reg);
            }

        } while (reg);

        if (contents.empty()) {
            return {};
        }

        return std::make_shared<sequence>(contents);
    }

    regex_ptr parse_disjunction(iterator& i, const iterator& end) {
        if (i == end) {
            return {};
        }

        std::vector<regex_ptr> contents;
        bool has_eps = false;

        if (!parse_char(i, end, '(')) {
            return {};
        }
        while (i != end && *i != ')') {
            auto reg = parse(i, end);
            if (reg) {
                contents.push_back(reg);
            }
            if (i != end && *i == '|') {
                ++i;
                if (i != end && *i == ')') {
                    has_eps = true;
                }
            }
        }
        if (!parse_char(i, end, ')')) {
            return {};
        }

        return std::make_shared<disjunction>(contents, has_eps);
    }

    regex_ptr parse(iterator& i, const iterator& end) {
        if (i == end) {
            return {};
        }

        auto reg = parse_regex(i, end);
        if (reg) {
            return reg;
        }

        reg = parse_disjunction(i, end);
        if (reg) {
            return reg;
        }

        return parse_sequence(i, end);
    }

    using point = aoc::vec2<int>;
    using point_set = aoc::vec2_set<int>;

    class edge {
        point u_;
        point v_;

        bool compare_pts(const auto& u, const auto& v) {
            if (u.y < v.y) {
                return true;
            }
            if (u.y > v.y) {
                return false;
            }
            return u.x < v.x;
        }

    public:
        edge(const auto& u, const auto& v) : u_(u), v_(v) {
            if (!compare_pts(u_, v_)) {
                std::swap(u_, v_);
            }
        }

        point u() const {
            return u_;
        }

        point v() const {
            return v_;
        }

        bool operator==(const edge& e) const {
            return u_ == e.u() && v_ == e.v();
        }
    };

    struct edge_hash {
        size_t operator()(const edge& e) const {
            size_t seed = 0;
            boost::hash_combine(seed, aoc::hash_vec2<int>{}(e.u()));
            boost::hash_combine(seed, aoc::hash_vec2<int>{}(e.v()));
            return seed;
        }
    };

    using edge_set = std::unordered_set<edge, edge_hash>;

    struct traversal_state {
        node_ptr node;
        point loc;

        bool operator==(const traversal_state& state) const {
            return node == state.node && loc == state.loc;
        }
    };

    struct traversal_state_hash {
        size_t operator()(const traversal_state& node) const {
            size_t seed = 0;
            boost::hash_combine(seed, node.node.get());
            boost::hash_combine(seed, aoc::hash_vec2<int>{}(node.loc));
            return seed;
        }
    };
    using trav_state_set = std::unordered_set<traversal_state, traversal_state_hash>;

    point move_in_direction(const point& loc, char dir) {
        const static std::unordered_map<char, point> dir_to_delta = {
            {'N', {0,-1}},
            {'E', {1, 0}},
            {'S', {0,1}},
            {'W', {-1, 0}}
        };
        return loc + dir_to_delta.at(dir);
    }

    std::tuple<point_set, edge_set> find_rooms_and_doors(const digraph& g) {
        edge_set doors;
        point_set rooms;
        trav_state_set visited;
        std::stack<traversal_state> stack;
        
        stack.push( {g.src, point{0,0}} );

        while (!stack.empty()) {
            auto current = stack.top();
            stack.pop();

            if (visited.contains(current)) {
                continue;
            }
            visited.insert(current);
            rooms.insert(current.loc);

            auto next_loc = (current.node->direction) ?
                move_in_direction(current.loc, current.node->direction) :
                current.loc;

            if (next_loc != current.loc) {
                doors.insert({ current.loc, next_loc });
                rooms.insert( next_loc );
            }
            
            for (auto next_node : current.node->adj_list) {
                stack.push({ next_node, next_loc });
            }
        }

        return { std::move(rooms), std::move(doors) };
    }

    using north_pole_base = aoc::vec2_map<int,std::vector<point>>;
    
    north_pole_base build_base(const edge_set& doors) {
        north_pole_base base;
        for (const auto& door : doors) {
            base[door.u()].push_back(door.v());
            base[door.v()].push_back(door.u());
        }
        return base;
    }

    int most_doors(const north_pole_base& base) {
        
        struct item {
            point loc;
            int dist;
        };

        int max_dist = 0;
        std::queue<item> queue;
        point_set visited;

        queue.push({ {0,0}, 0 });
        while (!queue.empty()) {
            auto current = queue.front();
            queue.pop();

            if (visited.contains( current.loc )) {
                continue;
            }
            visited.insert( current.loc );

            max_dist = std::max(max_dist, current.dist);

            for (const auto& next : base.at(current.loc)) {
                queue.push({ next, current.dist + 1 });
            }
        }

        return max_dist;
    }
}

void aoc::y2018::day_20(const std::string& title) {

    auto inp = aoc::file_to_string(
            aoc::input_path(2018, 20)
        ); 

    auto i = inp.begin();
    auto regex = parse(i, inp.end());
    auto graph = regex->to_graph();
    auto [rooms, doors] = find_rooms_and_doors(graph);
    auto base = build_base(doors);

    std::println("--- Day 20: {} ---", title);
    std::println("  part 1: {}", most_doors(base) );
    std::println("  part 2: {}", 0);
    
}
