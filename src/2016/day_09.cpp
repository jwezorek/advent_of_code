
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
        std::string str;
        int reps;
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

    bool parse_char(state& s, char ch) {
        if (s.pos == s.end || *s.pos != ch) {
            return false;
        }
        ++s.pos;
        return true;
    }

    std::optional<marker> parse_marker(state& s) {
        auto old_state = s;

        if (!parse_char(s, '(')) {
            return {};
        }
        
        // the input does not have any malformed markers
        // so dont bother validating any of this...

        auto characters = parse_int(s); 
        ++s.pos;

        auto repetitions = parse_int(s); 
        ++s.pos;

        auto start = s.pos;
        s.pos += *characters;

        return marker{ std::string(start, s.pos), *repetitions };
    }

    int64_t size_expanded(const std::string& compressed, bool recursive);

    int64_t size_expanded_marker(const marker& marker, bool recursive) {
        return (!recursive) ?
            marker.reps * marker.str.size() :
            marker.reps * size_expanded(marker.str, true);
    }

    int64_t size_expanded(const std::string& compressed, bool recursive) {

        std::vector<marker> markers;
        state s{ compressed.begin(), compressed.end() };
        int64_t count = 0;

        while (s.pos != s.end) {
            auto marker = parse_marker(s);
            if (marker) {
                markers.push_back(*marker);
                continue;
            }
            count++;
            if (s.pos != s.end) {
                s.pos++;
            }
        }

        return count + r::fold_left(
            markers | rv::transform(
                [recursive](auto&& m) {
                    return size_expanded_marker(m, recursive);
                }
            ),
            0ll,
            std::plus<int64_t>()
        );
    }
}

void aoc::y2016::day_09(const std::string& title) {

    auto inp = aoc::trim(
        aoc::file_to_string(
            aoc::input_path(2016, 9)
        )
    );

    std::println("--- Day 9: {} ---", title);
    std::println("  part 1: {}", size_expanded(inp, false) );
    std::println("  part 2: {}", size_expanded(inp, true));
    
}
