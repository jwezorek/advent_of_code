#include "../util/util.h"
#include "y2015.h"
#include <filesystem>
#include <functional>
#include <print>
#include <ranges>
#include <unordered_set>
#include <map>


namespace r = std::ranges;
namespace rv = std::ranges::views;

/*------------------------------------------------------------------------------------------------*/

namespace {
    using sue_properties = std::map<std::string, int>;
    struct some_aunt_sue {
        int index;
        sue_properties props;
    };
    using sue_list = std::vector<some_aunt_sue>;

    some_aunt_sue str_to_sue_spec(const std::string& str) {
        auto words = aoc::extract_alphanumeric(str);
        int index = std::stoi(words[1]);
        sue_properties sue;
        for (auto pair : words | rv::drop(2) | rv::chunk(2)) {
            sue[pair[0]] = std::stoi(pair[1]);
        }
        return { index, sue };
    }

    bool is_matching_sue(const sue_properties& unknown_sue, const some_aunt_sue& sue, bool part_1) {
        if (part_1) {
            for (auto [prop, val] : unknown_sue) {
                if (sue.props.contains(prop) && sue.props.at(prop) != val) {
                    return false;
                }
            }
            return true;
        }

        auto new_unknown_sue = unknown_sue;
        new_unknown_sue.erase("cats");
        new_unknown_sue.erase("trees"); 
        new_unknown_sue.erase("pomeranians");
        new_unknown_sue.erase("goldfish");

        if (!is_matching_sue(new_unknown_sue, sue, true)) {
            return false;
        }
        const auto& props = sue.props;
        if ((props.contains("cats") && props.at("cats") <= unknown_sue.at("cats")) ||
                (props.contains("trees") && props.at("trees") <= unknown_sue.at("trees"))) {
            return false;
        }

        if ((props.contains("pomeranians") && props.at("pomeranians") >= 
                    unknown_sue.at("pomeranians")) ||
                (props.contains("goldfish") && props.at("goldfish") >= 
                    unknown_sue.at("goldfish"))) {
            return false;
        }

        return true;
    }

    int matching_sue(const sue_list& sues, const sue_properties& unknown_sue, bool part_1) {
        for (const auto& sue : sues) {
            if (is_matching_sue(unknown_sue, sue, part_1)) {
                return sue.index;
            }
        }
        return -1;
    }

}

void aoc::y2015::day_16(const std::string& title) {

    auto sues = aoc::file_to_string_vector(
            aoc::input_path(2015, 16)
        ) | rv::transform(
            str_to_sue_spec
        ) | r::to<sue_list>();

    auto unknown_sue = str_to_sue_spec(
            "Sue 0: children: 3, cats : 7, samoyeds : 2, pomeranians : 3, "
            "akitas : 0, vizslas : 0, goldfish : 5, trees : 3, cars : 2, perfumes : 1"
        ).props;

    std::println("--- Day 16: {} ---", title);
    std::println("  part 1: {}",
        matching_sue(sues, unknown_sue, true)
    );
    std::println("  part 2: {}",
        matching_sue(sues, unknown_sue, false)
    );
}