
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
    enum command_op {
        swap_position = 0,
        swap_letter,
        rotate_position,
        rotate,
        reverse,
        move
    };

    bool has_prefix(const std::string& str, const std::string& prefix) {
        if (str.size() < prefix.size()) {
            return false;
        }
        return str.substr(0, prefix.size()) == prefix;
    }

    command_op str_to_command_op(const std::string& str) {

        static const std::array<std::string, 6> prefixes = {
            "swap position",
            "swap letter",
            "rotate based on position of letter",
            "rotate",
            "reverse",
            "move"
        };

        auto iter = r::find_if(
            prefixes,
            [&](auto&& prefix)->bool {
                return has_prefix(str, prefix);
            }
        );

        return static_cast<command_op>(iter - prefixes.begin());
    }

    struct command {
        command_op op;
        int arg1;
        int arg2;
    };

    command parse_swap_position(const std::string& str) {
        auto positions = aoc::extract_numbers(str);
        return {
            swap_position,
            positions[0],
            positions[1]
        };
    }

    command parse_swap_letter(const std::string& str) {
        auto words = aoc::extract_alphabetic(str);        
        return {
            swap_letter,
            words[2].front() - 'a',
            words[5].front() - 'a'
        };
    }

    command parse_rotate_position(const std::string& str) {
        auto words = aoc::extract_alphabetic(str);
        return {
            rotate_position,
            words.back().front() - 'a',
            0
        };
    }

    command parse_rotate(const std::string& str) {
        auto words = aoc::extract_alphanumeric(str);
        return {
            rotate,
            words[1] == "left" ? 0 : 1,
            std::stoi( words[2] )
        };
    }

    command parse_reverse(const std::string& str) {
        auto positions = aoc::extract_numbers(str);
        return {
            reverse,
            positions[0],
            positions[1]
        };
    }

    command parse_move(const std::string& str) {
        auto positions = aoc::extract_numbers(str);
        return {
            move,
            positions[0],
            positions[1]
        };
    }

    using parse_fn = std::function<command(const std::string&)>;

    command parse_command(const std::string& str) {
        auto op = str_to_command_op(str);
        const static std::unordered_map<command_op, parse_fn> op_to_parser = {
            { swap_position , parse_swap_position },
            { swap_letter , parse_swap_letter },
            { rotate_position, parse_rotate_position },
            { rotate , parse_rotate },
            { reverse , parse_reverse },
            { move , parse_move }
        };
        return op_to_parser.at(op)(str);
    }

    std::string do_swap_position(const command& cmd, const std::string& inp) {
        auto out = inp;
        std::swap(out[cmd.arg1], out[cmd.arg2]);
        return out;
    }

    std::string do_swap_letter(const command& cmd, const std::string& inp) {
        auto out = inp;
        auto i = out.find(cmd.arg1 + 'a');
        auto j = out.find(cmd.arg2 + 'a');
        std::swap(out[i], out[j]);
        return out;
    }

    std::string rot(const std::string& inp, bool left, int amnt) {
        int delta = left ? -1 : 1;
        int n = inp.size();
        auto out = inp;
        for (int i = 0; i < inp.size(); ++i) {
            int j = ((i + amnt * delta) + n) % n;
            out[j] = inp[i];
        }
        return out;
    }

    std::string do_rotate_position(const command& cmd, const std::string& inp) {
        auto index = inp.find(cmd.arg1 + 'a');
        if (index == std::string::npos) {
            throw std::runtime_error("Letter not found in string");
        }

        // Calculate the number of rotations
        int rotations = 1 + index + (index >= 4 ? 1 : 0);

        // Rotate the string to the right by the calculated number of steps
        return rot(inp, false, rotations);
    }

    std::string do_rotate(const command& cmd, const std::string& inp) {
        return rot(inp, cmd.arg1 == 0, cmd.arg2);
    }

    std::string do_reverse(const command& cmd, const std::string& inp) {
        auto out = inp;
        int i = cmd.arg1;
        int j = cmd.arg2;
        while (i < j) {
            std::swap(out[i], out[j]);
            ++i;
            --j;
        }
        return out;
    }

    std::string do_move(const command& cmd, const std::string& inp) {
        auto out = inp;
        auto ch = inp.at(cmd.arg1);
        out.erase(out.begin() + cmd.arg1);
        out.insert(out.begin() + cmd.arg2, ch);
        return out;
    }

    using command_fn = std::function<std::string(const command&, const std::string&)>;

    std::string perform_command(const command& cmd, const std::string& inp) {
        const static std::unordered_map<command_op, command_fn> tbl = {
            { swap_position  , do_swap_position },
            { swap_letter    , do_swap_letter },
            { rotate_position, do_rotate_position },
            { rotate ,         do_rotate },
            { reverse ,        do_reverse },
            { move ,           do_move }
        };
        return tbl.at(cmd.op)(cmd, inp);
    }

    std::string scramble(const std::vector<command>& cmds, const std::string& inp) {
        auto str = inp;
        for (const auto& cmd : cmds) {
            str = perform_command(cmd, str);
        }
        return str;
    }

    std::string inverse_rotate_position(const command& cmd, const std::string& inp) {
        auto size = inp.size();

        for (size_t index = 0; index < size; ++index) {

            int rotations = 1 + index + (index >= 4 ? 1 : 0);
            rotations %= size; 

            std::string candidate = rot(inp, true, rotations);

            int verify_rotations = 1 + candidate.find(cmd.arg1 + 'a');
            verify_rotations += (candidate.find(cmd.arg1 + 'a') >= 4 ? 1 : 0);
            verify_rotations %= size;

            std::string verification = rot(candidate, false, verify_rotations);
            if (verification == inp) {
                return candidate; // Found the original string
            }
        }

        throw std::runtime_error("Unable to reverse rotation; input may be invalid.");
    }

    std::string inverse_rotate(const command& cmd, const std::string& inp) {
        bool reverseDirection = cmd.arg1 != 0;
        return rot(inp, reverseDirection, cmd.arg2);
    }

    std::string inverse_move(const command& cmd, const std::string& inp) {
        auto out = inp;
        auto ch = inp.at(cmd.arg2); 
        out.erase(out.begin() + cmd.arg2); 
        out.insert(out.begin() + cmd.arg1, ch); 
        return out;
    }

    std::string perform_inverse_command(const command& cmd, const std::string& inp) {
        const static std::unordered_map<command_op, command_fn> tbl = {
            { swap_position, do_swap_position },
            { swap_letter, do_swap_letter },
            { rotate_position, inverse_rotate_position },
            { rotate, inverse_rotate },
            { reverse, do_reverse },
            { move, inverse_move }
        };
        return tbl.at(cmd.op)(cmd, inp);
    }

    std::string descramble(const std::vector<command>& cmds, const std::string& inp) {
        auto reverse_cmds = cmds;
        r::reverse(reverse_cmds);

        auto str = inp;
        for (const auto& cmd : reverse_cmds) {
            str = perform_inverse_command(cmd, str);
        }

        return str;
    }
}

void aoc::y2016::day_21(const std::string& title) {

    auto inp = aoc::file_to_string_vector(
            aoc::input_path(2016, 21)
        ) | rv::transform(
            parse_command
        ) | r::to<std::vector>();

    std::println("--- Day 21: {} ---", title);
    std::println("  part 1: {}", scramble(inp, "abcdefgh"));
    std::println("  part 2: {}", descramble(inp, "fbgdceah"));
    
}
