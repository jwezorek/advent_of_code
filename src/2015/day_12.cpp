#include "../util/util.h"
#include "y2015.h"
#include <filesystem>
#include <functional>
#include <print>
#include <ranges>
#include <unordered_set>
#define BOOST_JSON_NO_LIB 
#define BOOST_CONTAINER_NO_LIB

//TODO: use jlohman/json

/* #include <boost/json/src.hpp>

namespace r = std::ranges;
namespace rv = std::ranges::views;



using json = boost::json::value;

namespace {

    int recursive_sum_of_numbers_in_non_red_objects(const json& json) {

        if (json.is_number()) {
            auto num = json.as_int64();
            return static_cast<int>(num);
        }

        if (json.is_array()) {
            const auto& ary = json.as_array();
            return r::fold_left(
                ary | rv::transform(recursive_sum_of_numbers_in_non_red_objects),
                0,
                std::plus<>()
            );
        }

        if (json.is_object()) {
            const auto& obj = json.as_object();
            int red_properties = r::count_if(
                    obj,
                    [](auto&& kvp) {
                        return kvp.value().is_string() && kvp.value().as_string() == "red";
                    }
                );
            if (red_properties > 0) {
                return 0;
            }
            return r::fold_left(
                obj | rv::transform(
                    [](auto&& kvp) {
                        return recursive_sum_of_numbers_in_non_red_objects(kvp.value());
                    }
                ),
                0,
                std::plus<>()
            );
        }

        return 0;
    }

}

*/

void aoc::y2015::day_12(const std::string& title) {

    auto inp = aoc::file_to_string(aoc::input_path(2015, 12));
    /*
    std::println("--- Day 12: {} ---", title);
    std::println("  part 1: {}",
        r::fold_left(
            aoc::extract_numbers(inp, true),
            0,
            std::plus<>()
        )
    );
    
    std::println("  part 2: {}",
        recursive_sum_of_numbers_in_non_red_objects(
            boost::json::parse(inp)
        )
    );
    */
}