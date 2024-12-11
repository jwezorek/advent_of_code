
#include "../util/util.h"
#include "y2016.h"
#include <filesystem>
#include <functional>
#include <print>
#include <ranges>
#include <unordered_map>

namespace r = std::ranges;
namespace rv = std::ranges::views;

/*------------------------------------------------------------------------------------------------*/

namespace {
    struct room {
        std::vector<std::string> encrypted_name;
        int sector_id;
        std::string checksum;
    };

    room parse_room(const std::string& str) {
        auto parts = aoc::extract_alphanumeric(str);
        auto n = parts.size();
        return {
            parts | rv::take(n - 2) | r::to<std::vector>(),
            std::stoi(parts[n - 2]),
            parts[n - 1]
        };
    }

    bool is_real_room(const room& room) {
        std::unordered_map<char, int> count_tbl;
        auto letters = room.encrypted_name | rv::join | r::to<std::string>();
        for (auto letter : letters) {
            ++count_tbl[letter];
        }

        auto keys = count_tbl | rv::keys | r::to<std::string>();
        r::sort(keys);
        r::sort(
            keys,
            [&](const auto& lhs, const auto& rhs)->bool {
                return count_tbl[lhs] > count_tbl[rhs];
            }
        );
        keys = keys | rv::take(5) | r::to<std::string>();
        r::sort(keys);

        auto checksum = room.checksum;
        r::sort(checksum);

        return keys == checksum;
    }

    std::string decrypt(const std::string& str, int key) {
        std::stringstream ss;
        for (char ch : str) {
            char decrypted = static_cast<char>(
                    (static_cast<int>(ch - 'a') + key) % 26 + 'a'
                );
            ss << decrypted;
        }
        return ss.str();
    }

    bool is_northole_storage_room(const room& room) {
        for (const auto& word : room.encrypted_name) {
            if (decrypt(word, room.sector_id) == "northpole") {
                return true;
            }
        }
        return false;
    }

    int find_northpole_storage_room(const std::vector<room>& rooms) {
        for (const auto& room : rooms | rv::filter(is_real_room)) {
            if (is_northole_storage_room(room)) {
                return room.sector_id;
            }
        }
        return -1;
    }
}

void aoc::y2016::day_04(const std::string& title) {

    auto inp = aoc::file_to_string_vector(
            aoc::input_path(2016, 4)
        ) | rv::transform(
            parse_room
        ) | r::to<std::vector>();

    std::println("--- Day 4: {} ---", title);
    std::println("  part 1: {}",
        r::fold_left(
            inp | rv::transform(
                [](auto&& room) {
                    if (is_real_room(room)) {
                        return room.sector_id;
                    }
                    return 0;
                }
            ),
            0,
            std::plus<>()
        )
    );
    std::println("  part 2: {}", find_northpole_storage_room(inp));
}
