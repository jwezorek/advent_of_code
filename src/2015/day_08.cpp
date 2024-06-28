#include "../util.h"
#include "y2015.h"
#include <filesystem>
#include <functional>
#include <print>
#include <ranges>
#include <unordered_set>

namespace r = std::ranges;
namespace rv = std::ranges::views;

/*------------------------------------------------------------------------------------------------*/

namespace {
    std::string unquote(const std::string& s) {
        if (s.empty() || s.size() == 1) {
            return s;
        }
        if (s.front() != '"' || s.back() != '"') {
            return s;
        }
        return s.substr(1, s.size() - 2);
    }

    std::string unescape(const std::string& inp) {
        auto str = unquote(inp) + "   ";
        std::stringstream out;
        for (int i = 0; i < inp.size()-2; ++i) {
            const char* quad = &str[i];
            if (quad[0] != '\\') {
                out << quad[0];
                continue;
            }
            if (quad[1] == '\\' || quad[1] == '"') {
                out << quad[1];
                i += 1;
            } else  if (quad[1] == 'x') {
                auto hex = std::string({ quad[2],quad[3] });
                int val = std::stoul(hex, nullptr, 16);
                out << static_cast<char>(val);
                i += 3;
            }
        }
        return out.str();
    }

    std::string escape(const std::string& inp) {
        std::stringstream out;
        out << '"';
        for (auto ch : inp) {
            if (ch == '"') {
                out << '\\';
                out << '"';
                continue;
            } else if (ch == '\\') {
                out << '\\';
                out << '\\';
                continue;
            }
            out << ch;
        }
        out << '"';
        return out.str();
    }
}

void aoc::y2015::day_08(const std::string& title) {

    auto inp = aoc::file_to_string_vector(aoc::input_path(2015, 8));

    std::println("--- Day 8: {} ---", title);
    std::println("  part 1: {}",
        r::fold_left(
            inp | rv::transform(
                [](const std::string& str)->int{
                    return str.size() - unescape(str).size();
                }
            ),
            0,
            std::plus<>()
        )
    );
    std::println("  part 2: {}",
        r::fold_left(
            inp | rv::transform(
                [](const std::string& str)->int {
                    return escape(str).size() - str.size();
                }
            ),
            0,
            std::plus<>()
        )
    );
}