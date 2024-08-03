
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

    struct finite_automata_node {
        char direction;
        std::vector<int> adj_list;

        finite_automata_node(char dir = {}) : direction(dir)
        {}
    };

    using finite_automata = std::vector<finite_automata_node>;
    struct subgraph {
        int src;
        int dst;
    };

    int make_node(finite_automata& g, char dir = {}) {
        int new_node_id = static_cast<int>(g.size());
        g.push_back({ dir });
        return new_node_id;
    }

    using iterator = std::string::const_iterator;

    std::optional<subgraph> parse(iterator& i, const iterator& end, finite_automata& graph);

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

    std::optional<subgraph> parse_regex(
            iterator& i, const iterator& end, finite_automata& graph) {

        if (i == end) {
            return {};
        }

        if (!parse_char(i, end, '^')) {
            return {};
        }
        auto body = parse(i, end, graph);
        if (!body) {
            return {};
        }

        if (!parse_char(i, end, '$')) {
            return {};
        }

        subgraph out;
        out.src = make_node(graph);
        out.dst = make_node(graph);
        graph[out.src].adj_list.push_back(body->src);
        graph[body->dst].adj_list.push_back(out.dst);

        return out;
    }

    std::optional<subgraph> parse_text(
            iterator& i, const iterator& end, finite_automata& graph) {

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

        subgraph directions_subgraph;
        std::optional<int> prev;

        int dir_node = -1;
        for (auto dir : txt) {
            dir_node = make_node(graph, dir);
            if (prev) {
                graph[*prev].adj_list.push_back(dir_node);
            } else {
                directions_subgraph.src = dir_node;
            }
            prev = dir_node;
        }

        directions_subgraph.dst = dir_node;
        return directions_subgraph;
    }

    std::optional<subgraph> parse_sequence(
            iterator& i, const iterator& end, finite_automata& g) {

        if (i == end || *i == ')' || *i == '|' || *i == '$') {
            return {};
        }

        std::vector<subgraph> contents;
        std::optional<subgraph> reg;

        do {
            if (i != end && std::isalpha(*i)) {
                reg = parse_text(i, end, g);
                if (reg) {
                    contents.push_back(*reg);
                }
                continue;
            }

            reg = parse(i, end, g);
            if (reg) {
                contents.push_back(*reg);
            }

        } while (reg);

        if (contents.empty()) {
            return {};
        }

        subgraph seq;
        std::optional<int> prev;
        subgraph child_subgraph = { -1,-1 };

        for (auto cs : contents) {
            child_subgraph = cs;
            if (prev) {
                g[*prev].adj_list.push_back(child_subgraph.src);
            } else {
                seq.src = child_subgraph.src;
            }
            prev = child_subgraph.dst;
        }

        seq.dst = child_subgraph.dst;
        return seq;
    }

    std::optional<subgraph> parse_disjunction(
            iterator& i, const iterator& end, finite_automata& g) {

        if (i == end) {
            return {};
        }

        std::vector<subgraph> contents;
        bool has_eps_trans = false;

        if (!parse_char(i, end, '(')) {
            return {};
        }
        while (i != end && *i != ')') {
            auto reg = parse(i, end, g);
            if (reg) {
                contents.push_back(*reg);
            }
            if (i != end && *i == '|') {
                ++i;
                if (i != end && *i == ')') {
                    has_eps_trans = true;
                }
            }
        }
        if (!parse_char(i, end, ')')) {
            return {};
        }

        subgraph disjunc{
                make_node(g),
                make_node(g)
        };

        for (auto subgraph : contents) {
            g[disjunc.src].adj_list.push_back(subgraph.src);
            g[subgraph.dst].adj_list.push_back(disjunc.dst);
        }

        if (has_eps_trans) {
            g[disjunc.src].adj_list.push_back(disjunc.dst);
        }

        return disjunc;
    }

    std::optional<subgraph> parse(
            iterator& i, const iterator& end, finite_automata& g) {

        if (i == end) {
            return {};
        }

        auto reg = parse_regex(i, end, g);
        if (reg) {
            return reg;
        }

        reg = parse_disjunction(i, end, g);
        if (reg) {
            return reg;
        }

        return parse_sequence(i, end, g);
    }

    std::tuple<int, finite_automata> regex_to_finite_automata(const std::string& inp) {
        finite_automata fa;
        auto iter = inp.begin();
        auto src_and_dst = parse(iter, inp.end(), fa);
        return { src_and_dst->src, std::move(fa) };
    }

    using point = aoc::vec2<int>;
    using point_set = aoc::vec2_set<int>;

    template<typename U>
    using point_map = aoc::vec2_map<int, U>;

    using north_pole_base = point_map<std::vector<point>>;
    using dist_map = point_map<int>;

    struct door {
        point u;
        point v;

        bool operator==(const door& e) const {
            return u == e.u && v == e.v;
        }
    };

    struct door_hash {
        size_t operator()(const door& e) const {
            size_t seed = 0;
            boost::hash_combine(seed, aoc::hash_vec2<int>{}(e.u));
            boost::hash_combine(seed, aoc::hash_vec2<int>{}(e.v));
            return seed;
        }
    };

    using door_set = std::unordered_set<door, door_hash>;

    struct traversal_state {
        int node;
        point loc;

        bool operator==(const traversal_state& state) const {
            return node == state.node && loc == state.loc;
        }
    };

    struct traversal_state_hash {
        size_t operator()(const traversal_state& node) const {
            size_t seed = 0;
            boost::hash_combine(seed, node.node);
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

    door_set traverse_all_doors(int src, const finite_automata& g) {
        door_set doors;
        trav_state_set visited;
        std::stack<traversal_state> stack;
        
        stack.push( {src, point{0,0}} );

        while (!stack.empty()) {
            auto current = stack.top();
            stack.pop();

            if (visited.contains(current)) {
                continue;
            }
            visited.insert(current);

            auto next_loc = (g[current.node].direction) ?
                move_in_direction(current.loc, g[current.node].direction) :
                current.loc;

            if (next_loc != current.loc) {
                doors.insert({ current.loc, next_loc });
                doors.insert({ next_loc,  current.loc });
            }
            
            for (auto next_node : g[current.node].adj_list) {
                stack.push({ next_node, next_loc });
            }
        }

        return doors;
    }
    
    north_pole_base build_base(const std::string& base_directions) {
        auto [src, finite_automata] = regex_to_finite_automata(base_directions);
        auto doors = traverse_all_doors( src, finite_automata );

        north_pole_base base;
        for (const auto& door : doors) {
            base[door.u].push_back(door.v);
        }

        return base;
    }

    dist_map explore_north_pole_base(const std::string& base_directions) {
        auto base = build_base(base_directions);
        dist_map distances;
        std::queue<std::tuple<point,int>> queue;

        queue.push({ {0,0}, 0 });
        while (!queue.empty()) {
            auto [loc , dist] = queue.front();
            queue.pop();

            if (distances.contains(loc)) {
                continue;
            }
            distances[loc] = dist;

            for (const auto& next : base.at(loc)) {
                queue.push({ next, dist + 1 });
            }
        }

        return distances;
    }
}

void aoc::y2018::day_20(const std::string& title) {

    auto inp = aoc::file_to_string(
            aoc::input_path(2018, 20)
        ); 

    auto distances = explore_north_pole_base(inp);

    std::println("--- Day 20: {} ---", title);
    std::println("  part 1: {}", r::max( distances | rv::values ) );
    std::println("  part 2: {}",
        r::count_if(
            distances | rv::values,
            [](int dist) {
                return dist >= 1000;
            }
        )
    );
    
}
