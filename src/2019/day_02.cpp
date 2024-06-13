#include "../util.h"
#include "y2019.h"
#include <filesystem>
#include <functional>
#include <print>
#include <ranges>
#include <unordered_map>
#include <tuple>

namespace r = std::ranges;
namespace rv = std::ranges::views;

/*------------------------------------------------------------------------------------------------*/

namespace {
    class intcode_computer {
        std::vector<int> memory_;
        int program_counter_;
    public:

        intcode_computer(const std::vector<int>& memory) :
            memory_(memory),
            program_counter_(0) {

        }

        int current_value() const {
            return memory_[program_counter_];
        }

        int arg(int i) const {
            return memory_[program_counter_ + (i+1)];
        }

        int value(int i) const {
            return memory_[i];
        }

        int& value(int i) {
            return memory_[i];
        }

        void incr_prog_counter(int incr) {
            program_counter_ += incr;
        }
    };

    constexpr int k_terminate_op = 99;
    constexpr int k_add_op = 1;
    constexpr int k_multiply_op = 2;

    void do_add_op(intcode_computer& state) {
        int arg_1 = state.arg(0);
        int arg_2 = state.arg(1);
        int dst = state.arg(2);
        state.value(dst) = state.value(arg_1) + state.value(arg_2);
        state.incr_prog_counter(4);
    }

    void do_multiply_op(intcode_computer& state) {

        int arg_1 = state.arg(0);
        int arg_2 = state.arg(1);
        int dst = state.arg(2);
        state.value(dst) = state.value(arg_1) * state.value(arg_2);
        state.incr_prog_counter(4);
    }

    using op_fn = std::function<void(intcode_computer&)>;

    bool run_one_instruction(intcode_computer& state) {
        static std::unordered_map<int, op_fn> op_codes = {
            {k_add_op, do_add_op},
            {k_multiply_op, do_multiply_op}
        };
        auto val = state.current_value();
        if (val == k_terminate_op) {
            return false;
        }

        if (!op_codes.contains(val)) {
            throw std::runtime_error("illegal op code");
        }
        op_codes.at(val)(state);

        return true;
    }

    int run_program(const std::vector<int>& memory, int noun, int verb) {
        intcode_computer intcode(memory);

        intcode.value(1) = noun;
        intcode.value(2) = verb;

        bool continue_processing = true;
        while (continue_processing) {
            continue_processing = run_one_instruction(intcode);
        }
        return intcode.value(0);
    }

    std::tuple<int, int> find_noun_and_verb(const std::vector<int>& memory, int target) {
        for (auto [noun, verb] : rv::cartesian_product(rv::iota(0, 100), rv::iota(0, 100))) {
            if (run_program(memory, noun, verb) == target) {
                return { noun, verb };
            }
        }
        throw std::runtime_error("target not found");
    }

    int score_noun_and_verb(const std::vector<int>& memory, int target) {
        auto [noun, verb] = find_noun_and_verb(memory, target);
        return 100 * noun + verb;
    }
}

void aoc::y2019::day_02(const std::string& title) {

    auto inp = split(
            aoc::file_to_string(aoc::input_path(2019, 2)), ','
        ) | rv::transform(
            [](auto&& str)->int {
                return std::stoi(str);
            }
        ) | r::to<std::vector>();

    std::println("--- Day 2: {} ---", title);

    std::println("  part 1: {}",
        run_program( inp, 12, 2 )
    );
    std::println("  part 2: {}",
        score_noun_and_verb(inp, 19690720)
    );
}