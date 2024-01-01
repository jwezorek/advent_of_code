#include "../util.h"
#include "y2021.h"
#include <filesystem>
#include <functional>
#include <print>
#include <ranges>
#include <unordered_map>
#include <unordered_set>
#include <boost/functional/hash.hpp>
#include <queue>

namespace r = std::ranges;
namespace rv = std::ranges::views;

/*------------------------------------------------------------------------------------------------*/

namespace {
    enum room : unsigned char {
        leftwing_2 = 0, leftwing_1, a_vestibule, a_b_hallway, 
        b_vestibule, b_c_hallway, c_vestibule, c_d_hallway, 
        d_vestibule, rightwing_1, rightwing_2, 
        side_room_a_1, side_room_a_2, side_room_a_3, side_room_a_4,
        side_room_b_1, side_room_b_2, side_room_b_3, side_room_b_4,
        side_room_c_1, side_room_c_2, side_room_c_3, side_room_c_4,
        side_room_d_1, side_room_d_2, side_room_d_3, side_room_d_4,
        none
    };

    struct loc {
        int col;
        int row;
    };

    loc room_to_loc(room r) {
        static const std::unordered_map<room, loc> tbl = {
            {leftwing_2, {1,1}},
            {leftwing_1, {2,1}},
            {a_vestibule,{3,1}},
            {a_b_hallway,{4,1}},
            {b_vestibule,{5,1}},
            {b_c_hallway,{6,1}},
            {c_vestibule,{7,1}},
            {c_d_hallway,{8,1}},
            {d_vestibule,{9,1}},
            {rightwing_1,{10,1}},
            {rightwing_2,{11,1}},
            {side_room_a_1,{3,2}},
            {side_room_a_2,{3,3}},
            {side_room_a_3,{3,4}},
            {side_room_a_4,{3,5}},
            {side_room_b_1,{5,2}},
            {side_room_b_2,{5,3}},
            {side_room_b_3,{5,4}},
            {side_room_b_4,{5,5}},
            {side_room_c_1,{7,2}},
            {side_room_c_2,{7,3}},
            {side_room_c_3,{7,4}},
            {side_room_c_4,{7,5}},
            {side_room_d_1,{9,2}},
            {side_room_d_2,{9,3}},
            {side_room_d_3,{9,4}},
            {side_room_d_4,{9,5}}
        };
        return tbl.at(r);
    }

    auto amphipod_types() {
        static const std::array<char, 4> amphi_type = { {'A','B','C','D'} };
        return rv::all(amphi_type);
    }

    auto amphipods(bool supersized) {
        return rv::cartesian_product(
            amphipod_types(), 
            rv::iota(0, supersized ? 4 : 2)
        );
    }

    room vestibule_for_amphi(char amphi) {
        static const std::unordered_map<char, room> tbl = {
            {'A', a_vestibule},
            {'B', b_vestibule},
            {'C', c_vestibule},
            {'D', d_vestibule}
        };
        return tbl.at(amphi);
    }

    room side_room_for_amphi(char amphi, int i) {
        static const std::array<std::unordered_map<char, room>, 4> tbl = {{{
            {'A', side_room_a_1}, {'B', side_room_b_1}, {'C', side_room_c_1}, {'D', side_room_d_1}
        }, {
            {'A', side_room_a_2}, {'B', side_room_b_2}, {'C', side_room_c_2}, {'D', side_room_d_2}
        }, {
            {'A', side_room_a_3}, {'B', side_room_b_3}, {'C', side_room_c_3}, {'D', side_room_d_3}
        }, {
            {'A', side_room_a_4}, {'B', side_room_b_4}, {'C', side_room_c_4}, {'D', side_room_d_4}
        }}};
        return tbl.at(i).at(amphi);
    }

    auto hall_rooms() {
        return rv::iota(0, static_cast<int>(side_room_a_1)) |
            rv::transform(
                [](int i) {
                    return static_cast<room>(i);
                }
        );
    }

    std::vector<room> home_for_amphi(char amphi, bool supersized) {
        static const std::unordered_map<char, std::vector<room>> tbl = {
            {'A', {side_room_a_1, side_room_a_2, side_room_a_3, side_room_a_4}},
            {'B', {side_room_b_1, side_room_b_2, side_room_b_3, side_room_b_4}},
            {'C', {side_room_c_1, side_room_c_2, side_room_c_3, side_room_c_4}},
            {'D', {side_room_d_1, side_room_d_2, side_room_d_3, side_room_d_4}}
        };
        return (supersized) ? tbl.at(amphi) :
            tbl.at(amphi) | rv::take(2) | r::to<std::vector<room>>();
    }

    bool is_side_room(room loc) {
        return loc >= side_room_a_1 && loc <= side_room_d_4;
    }

    bool is_hallway(room loc) {
        auto hall = hall_rooms();
        return r::find(hall, loc) != hall.end();
    }

    bool is_supersized_room(room loc) {
        static const std::unordered_set<room> supersized = {
            side_room_a_3, side_room_a_4,
            side_room_b_3, side_room_b_4,
            side_room_c_3, side_room_c_4,
            side_room_d_3, side_room_d_4
        };
        return supersized.contains(loc);
    }

    auto side_room_row(int row) {
        static const std::array<std::array<room, 4>, 4> rows = {{
            {side_room_a_1, side_room_b_1, side_room_c_1, side_room_d_1},
            {side_room_a_2, side_room_b_2, side_room_c_2, side_room_d_2},
            {side_room_a_3, side_room_b_3, side_room_c_3, side_room_d_3},
            {side_room_a_4, side_room_b_4, side_room_c_4, side_room_d_4}
        }};
        return rv::all(rows[row]);
    }

    using burrow_graph = std::unordered_map<room, std::vector<room>>;
    void add_edge(burrow_graph& g, room u, room v) {
        g[u].push_back(v);
        g[v].push_back(u);
    }

    
    burrow_graph make_borrow_graph(bool supersized) {
        burrow_graph graph;
        for (auto [left, right] : hall_rooms() | rv::pairwise) {
            add_edge(graph, left, right);
        }
        for (char amphi = 'A'; amphi <= 'D'; ++amphi) {
            auto vestibule = vestibule_for_amphi(amphi);
            auto side_room_1 = side_room_for_amphi(amphi, 0);
            auto side_room_2 = side_room_for_amphi(amphi, 1);

            add_edge(graph, vestibule, side_room_1);
            add_edge(graph, side_room_1, side_room_2);
            if (supersized) {
                auto side_room_3 = side_room_for_amphi(amphi, 2);
                auto side_room_4 = side_room_for_amphi(amphi, 3);
                add_edge(graph, side_room_2, side_room_3);
                add_edge(graph, side_room_3, side_room_4);
            }
        }
        return graph;
    }

    class burrow_state {
        friend struct state_hash;
        std::vector<room> amphi_to_loc_;
        uint16_t has_been_moved_;

        int amphi_index(char amphi, int i) const {
            return num_amphis() * (amphi - 'A') + i;
        }

        room& amphi_loc(char amphi, int i) {
            return amphi_to_loc_[amphi_index(amphi,i)];
        }

        void normalize_amphi_order() {
            for (char amphi : amphipod_types()) {
                int start = amphi_index(amphi, 0);
                int end = start + num_amphis();
                auto amphi_range = r::subrange(
                    amphi_to_loc_.begin() + start,
                    amphi_to_loc_.begin() + end
                );
                r::sort(amphi_range);
            }
        }

        void set_moved_from_side_room(room sideroom) {
            if (!is_side_room(sideroom)) {
                return;
            }
            int sideroom_index = 
                static_cast<int>(sideroom) - static_cast<int>(side_room_a_1);
            unsigned char mask = 1 << sideroom_index;
            has_been_moved_ = has_been_moved_ | mask;
        }

    public:

        burrow_state(const burrow_state& bs) : 
            amphi_to_loc_(bs.amphi_to_loc_), has_been_moved_(bs.has_been_moved_)
        {}

        int num_amphis() const {
            return static_cast<int>(amphi_to_loc_.size()) / 4;
        }

        bool is_supersized() const {
            return amphi_to_loc_.size() == 16;
        }

        auto occupied_rooms() const {
            return amphi_to_loc_ | rv::filter(
                [](room loc) {return loc != none; }
            );
        }

        const room& amphi_loc(char amphi, int i) const {
            return amphi_to_loc_.at(amphi_index(amphi,i));
        }

        burrow_state(const std::vector<std::string>& rows) : has_been_moved_(0) {
            int num_home_rows = rows.size() - 3;
            auto home_rows = rows | rv::transform(
                [](auto&& row)->std::string {
                    return aoc::extract_alphabetic(row) |  rv::join | r::to<std::string>();
                }
            ) | rv::drop(2) | rv::take(num_home_rows) | r::to<std::vector<std::string>>();

            std::unordered_map<char, int> amphi_index = {
                {'A',0},{'B',0},{'C',0},{'D',0}
            };
            amphi_to_loc_ = std::vector<room>(num_home_rows * 4, none);
            for (const auto& [index, row] : rv::enumerate(home_rows)) {
                for (auto [amphi, room] : rv::zip(row, side_room_row(index))) {
                    int index = amphi_index[amphi]++;
                    amphi_loc(amphi, index) = room;
                }
            }
            
            normalize_amphi_order();
        }

        bool operator==(const burrow_state& bs) const {
            if (has_been_moved_ != bs.has_been_moved_) {
                return false;
            }
            for (auto [left, right] : rv::zip(amphi_to_loc_, bs.amphi_to_loc_)) {
                if (left != right) {
                    return false;
                }
            }
            return true;
        }

        burrow_state move_amphi(char amphi, int i, room dest) const {
            burrow_state moved(*this);
            auto old_loc = moved.amphi_loc(amphi, i);

            moved.amphi_loc(amphi, i) = dest;
            moved.normalize_amphi_order();

            if (is_side_room(old_loc)) {
                moved.set_moved_from_side_room(old_loc);
            }
            
            return moved;
        }

        bool has_moved_from_sideroom(room sideroom) const {
            if (!is_side_room(sideroom)) {
                return false;
            }
            int sideroom_index =
                static_cast<int>(sideroom) - static_cast<int>(side_room_a_1);
            unsigned char mask = 1 << sideroom_index;
            return has_been_moved_ & mask;
        }

        bool is_winning_state() const {
            static const std::vector<room> supersized_win_state = { {
                side_room_a_1,side_room_a_2,side_room_a_3,side_room_a_4,
                side_room_b_1,side_room_b_2,side_room_b_3,side_room_b_4,
                side_room_c_1,side_room_c_2,side_room_c_3,side_room_c_4,
                side_room_d_1,side_room_d_2,side_room_d_3,side_room_d_4
            }};

            auto win_state = is_supersized() ? supersized_win_state:
                supersized_win_state | rv::filter(
                    [](room loc) { return !is_supersized_room(loc); }
                ) | r::to<std::vector<room>>();

            for (auto [lhs, rhs] : rv::zip(win_state, amphi_to_loc_)) {
                if (lhs != rhs) {
                    return false;
                }
            }
            return true;
        }
    };

    struct state_hash {
        size_t operator()(const burrow_state& state) const {
            size_t seed = 0;
            boost::hash_combine(seed, state.has_been_moved_);
            for (auto room : state.amphi_to_loc_) {
                boost::hash_combine(seed, room);
            }
            return seed;
        }
    };

    struct weighted_state {
        burrow_state state;
        int weight;
    };

    std::optional<char> contents_of_room(const burrow_state& state, room loc) {
        auto amphis = amphipods(state.is_supersized());
        auto iter = r::find_if(amphis,
            [&](auto&& tup) {
                const auto& [amphi, i] = tup;
                return (state.amphi_loc(amphi, i) == loc);
            }
        );
        if (iter == amphis.end()) {
            return {};
        }
        return std::get<0>(*iter);
    }

    std::optional<room> open_home_room(const burrow_state& state, char amphi) {
        auto home = home_for_amphi(amphi, state.is_supersized());
        auto home_state = home | rv::transform(
                [&](room loc)->char {
                    auto contents = contents_of_room(state, loc);
                    return (contents) ? *contents : '.';
                }
            ) | r::to<std::string>();
        for (char not_amphi : 
                amphipod_types() | rv::filter([&](char a) {return a != amphi; })) {
            if (home_state.contains(not_amphi)) {
                return {};
            }
        }
        r::reverse(home);
        r::reverse(home_state);
        for (auto [room, content] : rv::zip(home, home_state)) {
            if (content == '.') {
                return room;
            }
        }
        return {};
    }

    std::optional<int> length_of_path_to(
            const burrow_graph& graph, const burrow_state& state, room u, room v) {
        std::queue<std::tuple<room, int>> queue;
        std::unordered_set<room> visited;
        std::unordered_set<room> occupied = state.occupied_rooms() | rv::filter(
                [u](auto loc) {
                    return loc != u;
                }
            ) | r::to<std::unordered_set<room>>();
        if (occupied.contains(v)) {
            return {};
        }
        queue.push({ u,0 });
        while (!queue.empty()) {
            auto [loc, dist] = queue.front();
            queue.pop();

            if (occupied.contains(loc)) {
                continue;
            }

            if (visited.contains(loc)) {
                continue;
            }
            visited.insert(loc);

            if (loc == v) {
                return dist;
            }

            for (auto neighbor : graph.at(loc)) {
                queue.push({ neighbor, dist + 1 });
            }
        }

        return {};
    }

    weighted_state weighted_neighbor(
            const burrow_state& state, char amphi, int amphi_index, room dest, int dist) {
        static const std::unordered_map<char, int> amphi_type_to_energy = {
            {'A', 1}, {'B', 10}, {'C', 100}, {'D', 1000}
        };
        return {
            state.move_amphi(amphi, amphi_index, dest),
            amphi_type_to_energy.at(amphi) * dist
        };
    }

    std::vector<weighted_state> neighboring_states(
            const burrow_graph& graph, const burrow_state& state, char amphi, int amphi_index) {
        auto curr_loc = state.amphi_loc(amphi, amphi_index);
        if (is_side_room(curr_loc) && state.has_moved_from_sideroom(curr_loc)) {
            // this amphipod is done moving...
            return {};
        }
        if (is_hallway(curr_loc)) {
            auto home_room = open_home_room(state, amphi);
            if (!home_room) {
                return {};
            }
            auto path_length = length_of_path_to(graph, state, curr_loc, *home_room);
            if (!path_length) {
                return {};
            }
            return {
                weighted_neighbor(state, amphi, amphi_index, *home_room, *path_length)
            };
        }
        static const std::array<room, 7> hall_destinations = { {
            leftwing_2, leftwing_1, a_b_hallway, b_c_hallway,
            c_d_hallway, rightwing_1, rightwing_2
        } };
        return hall_destinations | rv::transform(
                [&](auto dest)->std::tuple<room, std::optional<int>> {
                    return {
                        dest,
                        length_of_path_to(graph, state, curr_loc, dest)
                    };
                }
            ) | rv::filter(
                [](auto&& tup) {
                    return std::get<1>(tup).has_value();
                }
            ) | rv::transform(
                [&](auto && tup)->weighted_state {
                    auto [dest, dist] = tup;
                    return weighted_neighbor(state, amphi, amphi_index, dest, *dist);
                }
            ) | r::to<std::vector<weighted_state>>();
    }

    std::vector<weighted_state> neighboring_states(
            const burrow_graph& graph, const burrow_state& state) {
        std::vector<std::vector<weighted_state>> moves =
            amphipods(state.is_supersized()) | rv::transform(
                [&](auto&& tup)->std::vector<weighted_state> {
                    auto [amphi_type, amphi_index] = tup;
                    return neighboring_states(graph, state, amphi_type, amphi_index);
                }
            ) | rv::filter(
                [](auto&& vec) {
                    return !vec.empty();
                }
            ) | r::to<std::vector<std::vector<weighted_state>>>();

        return moves | rv::join | r::to<std::vector<weighted_state>>();
    }

    void display_state(const burrow_state& state) {
        std::vector<std::string> burrow = {
            "#############",
            "#...........#",
            "###.#.#.#.###",
            "  #.#.#.#.#  ",
            "  #########  "
        };
        if (state.is_supersized()) {
            burrow.insert(burrow.begin() + 3, "  #.#.#.#.#");
            burrow.insert(burrow.begin() + 3, "  #.#.#.#.#");
        }
        for (auto [amphi, i] : amphipods(state.is_supersized())) {
            auto room = state.amphi_loc(amphi, i);
            auto pos = room_to_loc(room);
            burrow[pos.row][pos.col] = amphi;
        }
        for (const auto& row : burrow) {
            std::println("{}", row);
        }
        std::println("");
    }

    int dijkstra_shortest_path(const burrow_graph& g, const burrow_state& start) {

        std::unordered_map<burrow_state, int, state_hash> dist;
        std::unordered_map<burrow_state, burrow_state, state_hash> prev;
        aoc::priority_queue<burrow_state, state_hash> queue;
        queue.insert(start, 0);
        dist[start] = 0;
        int least = std::numeric_limits<int>::max();

        while (!queue.empty()) {
            auto u = queue.extract_min();
            for (auto weighted_v : neighboring_states(g, u)) {
                auto dist_to_u = dist.at(u);
                auto dist_through_u_to_v = dist_to_u + weighted_v.weight;
                const auto& v = weighted_v.state;
                auto curr_dist_to_v = dist.contains(v) ? dist.at(v) : std::numeric_limits<int>::max();

                if (dist_through_u_to_v < curr_dist_to_v) {
                    if (v.is_winning_state()) {
                        least = (dist_through_u_to_v < least) ? dist_through_u_to_v : least;
                    }
                    dist[v] = dist_through_u_to_v;
                    if (queue.contains(v)) {
                        queue.change_priority(v, dist_through_u_to_v);
                    }
                    else {
                        queue.insert(v, dist_through_u_to_v);
                    }
                }
            }
        }

        return least;
    }

}

void aoc::y2021::day_23(const std::string& title) {
    auto input = aoc::file_to_string_vector(aoc::input_path(2021, 23));
    auto burrow = make_borrow_graph(false);
    auto start = burrow_state(input);

    auto supersized_burrow = make_borrow_graph(true);
    input.insert(input.begin() + 3, "  #D#B#A#C#");
    input.insert(input.begin() + 3, "  #D#C#B#A#");
    auto supersized_start = burrow_state(input);

    std::println("--- Day 23: {} ---", title);
    std::println("  part 1: {}", dijkstra_shortest_path(burrow, start));
    std::println("  part 2: {}", dijkstra_shortest_path(supersized_burrow, supersized_start));
}