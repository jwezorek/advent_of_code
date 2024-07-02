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

    enum op_code {
        hlf,
        tpl,
        inc,
        jmp,
        jie,
        jio
    };

    op_code str_to_op(const std::string& str) {
        static const std::unordered_map<std::string, op_code> op_tbl = {
            {"hlf",hlf},
            {"tpl",tpl},
            {"inc",inc},
            {"jmp",jmp},
            {"jie",jie},
            {"jio",jio}
        };
        return op_tbl.at(str);
    }

    struct instruction {
        op_code op;
        char reg;
        int offset;
    };

    instruction parse_line_of_code(const std::string& line) {
        auto words = aoc::extract_alphanumeric(line);
        auto op = str_to_op(words[0]);
        int sign = line.contains('-') ? -1 : 1;

        if (words.size() == 2) {
            if (op != jmp) {
                auto reg = words[1].front();
                return { op, reg, 0 };
            } else {
                return { jmp, 0, sign * std::stoi(words[1])};
            }
        }

        auto reg = words[1].front();
        return { op, reg, sign * std::stoi(words[2]) };
    }

    struct computer {
    private:
        uint64_t a_register_;
        uint64_t b_register_;
        int prog_counter_;
        std::vector<instruction> program_;

        bool run_one_instruction() {
            auto& comp = *this;
            const auto& instr = program_[prog_counter_];
            switch (instr.op) {
                case hlf:
                    comp[instr.reg] /= 2;
                    ++prog_counter_;
                    break;
                case tpl:
                    comp[instr.reg] *= 3;
                    ++prog_counter_;
                    break;
                case inc:
                    comp[instr.reg] += 1;
                    ++prog_counter_;
                    break;
                case jmp:
                    prog_counter_ += instr.offset;
                    break;
                case jie:
                    prog_counter_ += (comp[instr.reg] % 2 == 0) ? instr.offset : 1;
                    break;
                case jio:
                    prog_counter_ += (comp[instr.reg] == 1) ? instr.offset : 1;
                    break;
            }
            return (prog_counter_ < program_.size());
        }

    public:
        computer(const std::vector<instruction>& program) :
            a_register_{ 0 },
            b_register_{ 0 },
            prog_counter_{ 0 },
            program_{ program }
        {}

        uint64_t operator[](char reg) const {
            return reg == 'a' ? a_register_ : b_register_;
        }

        uint64_t& operator[](char reg) {
            return reg == 'a' ? a_register_ : b_register_;
        }

        void run() {
            bool not_done;
            do {
                not_done = run_one_instruction();
            } while (not_done);
        }
    };
}

void aoc::y2015::day_23(const std::string& title) {

    auto program = aoc::file_to_string_vector(
            aoc::input_path(2015, 23)
        ) | rv::transform(
            parse_line_of_code
        ) | r::to<std::vector>();

    auto comp = computer(program);
    comp.run();

    std::println("--- Day 23: {} ---", title);
    std::println("  part 1: {}",
        comp['b']
    );

    comp = computer(program);
    comp['a'] = 1;
    comp.run();

    std::println("  part 2: {}",
        comp['b']
    );
}