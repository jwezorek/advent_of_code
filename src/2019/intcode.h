#pragma once

#include <vector>
#include <optional>
#include <functional>

namespace aoc {

    using input_generator = std::function<int(int)>;

    class input_buffer {
        std::vector<int> buffer_;
        input_generator gen_;
        int curr_;
    public:

        input_buffer(const std::vector<int>& inp = {});
        input_buffer(const input_generator& gen);
        int next();
        void reset();
    };

    class intcode_computer {
        std::vector<int> memory_;
        int program_counter_;
        bool show_output_;
        std::optional<int> output_;

        bool run_one_instruction(input_buffer& inp);

    public:

        intcode_computer(const std::vector<int>& memory);

        void reset(const std::vector<int>& memory);
        void set_show_output(bool v);
        void show_output(int val);
        int output() const;
        int current_value() const;
        int current_address() const;
        int value(int i) const;
        int& value(int i);
        void incr_prog_counter(int incr);
        void jump_to(int address);
        void run(input_buffer& inp);
        void run();

    };

}