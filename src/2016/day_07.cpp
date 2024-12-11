
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
    bool is_abba(auto quad) {
        return quad[0] == quad[3] && quad[1] == quad[2] && quad[0] != quad[1];
    }

    bool contains_abba(const std::string& str) {
        auto quads = str | rv::slide(4);
        return r::find_if(quads, [](auto v) {return is_abba(v); }) != quads.end();
    }

    bool supports_tls(const std::vector<std::string>& ip) {
        bool has_abba = false;
        for (size_t i = 0; i < ip.size(); ++i) {
            bool needs_abba = i % 2 == 0;
            bool abba = contains_abba(ip[i]);

            if (needs_abba && abba) {
                has_abba = true;
            }

            if (!needs_abba && abba) {
                return false;
            }
        }
        return has_abba;
    }

    std::vector<std::string> extract_abas(const std::string& str) {
        return str | rv::slide(3) | rv::filter(
            [](auto tri) {
                return tri[0] == tri[2] && tri[0] != tri[1];
            }
        ) | rv::transform(
            [](auto tri)->std::string {
                return tri | r::to<std::string>();
            }
        ) | r::to<std::vector>();
    }

    bool supports_ssl(const std::vector<std::string>& ip) {
        std::vector<std::string> even;
        std::unordered_set<std::string> odd;
        for (size_t i = 0; i < ip.size(); ++i) {
            auto abas = extract_abas(ip[i]);
            if (i % 2 == 0) {
                r::copy(abas, std::back_inserter(even));
            } else {
                r::copy(abas, std::inserter(odd, odd.end()));
            }
        }
        for (auto aba : even) {
            std::string bab = { aba[1], aba[0], aba[1] };
            if (odd.contains(bab)) {
                return true;
            }
        }
        return false;
    }
}

void aoc::y2016::day_07(const std::string& title) {

        auto inp = aoc::file_to_string_vector(
            aoc::input_path(2016, 7)
        ) | rv::transform(
            aoc::extract_alphabetic
        ) | r::to<std::vector>();

    std::println("--- Day 7: {} ---", title);
    std::println( "  part 1: {}", r::count_if(inp, supports_tls) );
    std::println( "  part 2: {}", r::count_if(inp, supports_ssl) );
    
}
