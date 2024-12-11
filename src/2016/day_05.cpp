#include "../util/util.h"
#include "../2015/md5.h"
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
    std::string find_password(const std::string& door_id) {

        std::stringstream output;
        uint64_t key_num = 0;
        int pwd_char_count = 0;

        while (pwd_char_count < 8) {
            std::string key = door_id + std::to_string(key_num++);
            auto hash = md5(key);
            if (hash.substr(0, 5) == "00000") {
                output << hash[5];
                ++pwd_char_count;
            }
        }

        return output.str();
    }

    std::string find_password_with_positions(const std::string& door_id) {

        uint64_t key_num = 0;
        int pwd_char_count = 0;
        std::array<char, 8> pwd = { {0,0,0,0,0,0,0,0} };

        while (pwd_char_count < 8) {
            std::string key = door_id + std::to_string(key_num++);
            auto hash = md5(key);
            if (hash.substr(0, 5) == "00000") {
                auto pos_char = hash[5];
                if (pos_char < '0' || pos_char > '7') {
                    continue;
                }
                int pos = pos_char - '0';
                if (pwd[pos] != 0) {
                    continue;
                }
                pwd[pos] = hash[6];
                pwd_char_count++;
            }
        }

        return pwd | r::to<std::string>();
    }
}

void aoc::y2016::day_05(const std::string& title) {

    auto door_id = aoc::file_to_string(
        aoc::input_path(2016, 5)
    );

    std::println("--- Day 5: {} ---", title);
    std::println("  part 1: {}", find_password(door_id));
    std::println("  part 2: {}", find_password_with_positions(door_id));

}
