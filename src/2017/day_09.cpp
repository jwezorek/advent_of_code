
#include "../util/util.h"
#include "y2017.h"
#include <filesystem>
#include <functional>
#include <print>
#include <ranges>
#include <variant>

namespace r = std::ranges;
namespace rv = std::ranges::views;

/*------------------------------------------------------------------------------------------------*/

namespace {

    struct group;
    
    struct group : public std::variant<std::string, std::vector<group>> {
        using group_variant = std::variant<std::string, std::vector<group>>;
        using group_variant::operator=;
    };

    class parsing_state {
        std::string::const_iterator iter_;
        std::string::const_iterator end_;
    public:
        parsing_state(const std::string& inp) : iter_(inp.begin()), end_(inp.end())
        {}

        bool complete() const {
            return iter_ == end_;
        }

        char current() const {
            return *iter_;
        }

        void operator++() { // pre increment
            ++iter_;
        }
    };

    std::optional<std::string> parse_junk(parsing_state& state) {
        if (state.complete()) {
            return {};
        }
        if (state.current() != '<') {
            return {};
        }
        ++state;
        std::stringstream ss;
        while (state.current() != '>') {
            auto curr = state.current();
            if (curr != '!') {
                ss << curr;
                ++state;
            } else {
                ++state;
                ++state;
            }
        } 
        ++state;
        return ss.str();
    }

    std::optional<group> parse(parsing_state& state) {
        if (state.complete()) {
            return {};
        }
        auto junk = parse_junk(state);
        if (junk) {
            return group{ *junk };
        }

        if (state.current() != '{') {
            return {};
        }
        std::vector<group> output;
        
        ++state;
        if (state.current() == '}') {
            ++state;
            return group{ output };
        }

        output.push_back(*parse(state));
        while (state.current() == ',') {
            ++state;
            auto next_item = parse(state);
            if (next_item) {
                output.push_back(*next_item);
            }
        }
        ++state;

        return group{ output };
    }

    group parse(const std::string& inp) {
        parsing_state state(inp);
        return *parse(state);
    }

    int score_group(const group& groups, int curr_score = 0) {
        return std::visit(
            aoc::overload{
                [curr_score](const std::string& str)->int {
                    return 0;
                },
                [curr_score](const std::vector<group>& items)->int {
                    return r::fold_left(
                        items | rv::transform(
                            [curr_score](const auto& g)->int {
                                return score_group(g, curr_score + 1);
                            }
                        ),
                        curr_score + 1,
                        std::plus<>()
                    );
                },
            },
            groups
        );
    }

    int count_junk_characters(const group& groups) {
        return std::visit(
            aoc::overload{
                [](const std::string& str)->int {
                    return str.size();
                },
                [](const std::vector<group>& items)->int {
                    auto junk_count = 0;
                    for (const auto& child : items) {
                        junk_count += count_junk_characters(child);
                    }
                    return r::fold_left(
                        items | rv::transform( count_junk_characters ),
                        0,
                        std::plus<>()
                    );
                },
            },
            groups
        );
    }
}

void aoc::y2017::day_09(const std::string& title) {

    auto groups = parse(
        aoc::file_to_string(
            aoc::input_path(2017, 9)
        )
    );

    std::println("--- Day 9: {} ---", title);
    std::println("  part 1: {}", score_group(groups) );
    std::println("  part 2: {}", count_junk_characters(groups));
    
}
