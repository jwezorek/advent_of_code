
#include "../util/util.h"
#include "y2016.h"
#include <filesystem>
#include <functional>
#include <print>
#include <ranges>
#include <unordered_set>

namespace r = std::ranges;
namespace rv = std::ranges::views;

/*------------------------------------------------------------------------------------------------*/

namespace {
    struct marker {
        int characters;
        int repititions;
    };

    struct state {
        std::string::const_iterator pos;
        std::string::const_iterator end;
    };

    std::optional<int> parse_int(state& s) {
        std::stringstream ss;
        auto old_state = s;
        bool found_digit = false;
        while (s.pos != s.end && std::isdigit(*s.pos)) {
            found_digit = true;
            ss << *(s.pos++);
        }
        if (found_digit) {
            return std::stoi(ss.str());
        }
        s = old_state;
        return {};
    }

    std::optional<marker> parse_marker(state& s) {
        auto old_state = s;
        if (s.pos == s.end || *s.pos != '(') {
            return {};
        }
        ++s.pos;
        auto characters = parse_int(s);
        if (!characters) {
            s = old_state;
            return {};
        }
        if (s.pos == s.end || *s.pos != 'x') {
            s = old_state;
            return {};
        }
        ++s.pos;
        auto repetitions = parse_int(s);
        if (!repetitions) {
            s = old_state;
            return {};
        }
        if (s.pos == s.end || *s.pos != ')') {
            s = old_state;
            return {};
        }
        ++s.pos;

        return marker{ *characters, *repetitions };
    }

    bool has_markers(const std::string& str) {
        state s{ str.begin(), str.end() };
        while (s.pos != s.end) {
            auto marker = parse_marker(s);
            if (marker) {
                return true;
            }
            s.pos++;
        }
        return false;
    }

    std::string expand(const std::string&, bool);

    std::string expand_marker(state& s, const marker& marker, bool recursive) {

        if (!recursive) {
            auto start = s.pos;
            s.pos = s.pos + marker.characters;
            auto block = std::string(start, s.pos);
            return rv::repeat(block) | rv::take(
                    marker.repititions
                ) | rv::join | r::to<std::string>();
        }

        auto expanded = expand_marker(s, marker, false);
        if (!has_markers(expanded)) {
            return expanded;
        } 

        return expand(expanded, true);
    }

    std::string expand(const std::string& compressed, bool recursive_markers) {
        state s{ compressed.begin(), compressed.end()};
        std::stringstream uncompressed;
        while (s.pos != s.end) {
            auto marker = parse_marker(s);
            if (marker) {
                uncompressed << expand_marker(s, *marker, recursive_markers);
                continue;
            }
            uncompressed << *(s.pos++);
        }
        return uncompressed.str();
    }
}

void aoc::y2016::day_09(const std::string& title) {

    auto inp = aoc::trim(
        aoc::file_to_string(
            aoc::input_path(2016, 9)
        )
    );

    //std::string test = "A(2x2)BCD(2x2)EFG";
   // std::println("{} => {}", test, expand(test));

    std::println("--- Day 9: {} ---", title);
    std::println("  part 1: {}", expand(inp, false).size() );
    //std::println("  part 2: {}", expand(inp, true).size() );
    
}
