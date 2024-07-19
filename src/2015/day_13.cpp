#include "../util/util.h"
#include "y2015.h"
#include <filesystem>
#include <functional>
#include <print>
#include <ranges>
#include <set>
#include <unordered_map>

namespace r = std::ranges;
namespace rv = std::ranges::views;

/*------------------------------------------------------------------------------------------------*/

namespace {
    struct statement {
        std::string who;
        int happiness_modifier;
        std::string next_to;
    };

    statement str_to_potential_happiness_statement(const std::string& str) {
        auto words = aoc::extract_alphanumeric(str);
        int sign = (words[2] == "gain") ? 1 : -1;
        int amount = std::stoi(words[3]);
        return {
            words.front(),
            sign * amount,
            words.back()
        };
    }

    std::vector<std::string> all_people(const std::vector<statement>& statements) {
        return statements | rv::transform(
            [](auto&& s) {
                return s.who;
            }
        ) | r::to<std::set>() | r::to<std::vector>();
    }

    using index_map = std::unordered_map<std::string, int>;
    index_map map_person_to_index(const std::vector<std::string>& people) {
        return rv::enumerate(people) | rv::transform(
            [](auto&& pair)->index_map::value_type {
                auto [index, person] = pair;
                return { person, index };
            }
        ) | r::to<index_map>();
    }

    using table = std::vector<std::vector<int>>;
    table make_happiness_table(
            const index_map& indices, const std::vector<statement>& statements) {
        auto sz = indices.size();
        auto tbl = table(sz, std::vector<int>(sz, 0));
        for (const auto& s : statements) {
            auto who = indices.at(s.who);
            auto next_to = indices.at(s.next_to);
            tbl[who][next_to] = s.happiness_modifier;
        }
        return tbl;
    }

    int happiness_of_arrangement(
            const index_map& indices,
            const std::vector<std::string>& arrangement, const table& happiness) {

        auto order = arrangement;
        order.push_back(arrangement.front());

        int sum = 0;
        for (auto pair : order | rv::slide(2)) {
            auto u = indices.at(pair[0]);
            auto v = indices.at(pair[1]);
            sum += happiness[u][v];
            sum += happiness[v][u];
        }

        return sum;
    }

    int maxmize_happiness(const std::vector<statement>& statements) {
        auto people = all_people(statements);
        auto person_to_index = map_person_to_index(people);
        auto happiness_tbl = make_happiness_table(person_to_index, statements);
        int max_happiness = 0;
        auto arrangement = people;

        do {
            int happiness = happiness_of_arrangement(person_to_index, arrangement, happiness_tbl);
            if (happiness > max_happiness) {
                max_happiness = happiness;
            }
        } while (r::next_permutation(arrangement).found);

        return max_happiness;
    }
}

void aoc::y2015::day_13(const std::string& title) {

    auto inp = aoc::file_to_string_vector(
            aoc::input_path(2015, 13)
        ) | rv::transform(
            str_to_potential_happiness_statement
        ) | r::to<std::vector>();

    std::println("--- Day 13: {} ---", title);
    std::println("  part 1: {}",
        maxmize_happiness(inp)
    );

    inp.emplace_back( "me", 0, inp.front().who );
    std::println("  part 2: {}",
        maxmize_happiness(inp)
    );
}