#pragma once

#include <vector>
#include <optional>
#include <functional>

namespace aoc {

    using input_generator = std::function<int(int)>;

    class input_buffer {
        std::vector<int> buffer_;
        int curr_;
    public:

        input_buffer(const std::vector<int>& inp = {});
        int next();
        void reset();
    };

    using input_fn = std::function<int()>;
    using output_fn = std::function<void(int)>;

    enum icc_event {
        terminated,
        received_input,
        generated_output
    };

    class intcode_computer {
        std::vector<int> memory_;
        int program_counter_;
        std::optional<int> output_;

        bool run_one_instruction(const input_fn& inp, const output_fn& out);
        void incr_prog_counter(int incr);

    public:

        intcode_computer(const std::vector<int>& memory);

        void reset(const std::vector<int>& memory);
        void set_output(int v);
        int output() const;
        int current_value() const;
        int current_address() const;
        int value(int i) const;
        int& value(int i);
        void jump_to(int address);
        void run(input_buffer& inp);
        void run();
        void run(const input_fn& inp, const output_fn& out);
        icc_event run_until_event(int inp);
    };

}