#pragma once

#include <vector>
#include <unordered_map>
#include <optional>
#include <functional>

namespace aoc {

    class input_buffer {
        std::vector<int64_t> buffer_;
        int64_t curr_;
    public:

        input_buffer(const std::vector<int64_t>& inp = {});
        int64_t next();
        void reset();
    };

    using input_fn = std::function<int64_t()>;
    using output_fn = std::function<void(int64_t)>;

    enum icc_event {
        terminated,
        received_input,
        generated_output
    };

    class intcode_computer {
        std::unordered_map<int64_t, int64_t> memory_;
        int64_t program_counter_;
        std::optional<int64_t> output_;

        bool run_one_instruction(const input_fn& inp, const output_fn& out);
        void incr_prog_counter(int64_t incr);

    public:

        intcode_computer(const std::vector<int64_t>& memory);

        void reset(const std::vector<int64_t>& memory);
        void set_output(int64_t v);
        int64_t output() const;
        int64_t current_value() const;
        int64_t current_address() const;
        const int64_t& value(int64_t i) const;
        int64_t& value(int64_t i);
        void jump_to(int64_t address);
        void run(input_buffer& inp);
        void run();
        void run(const input_fn& inp, const output_fn& out);
        icc_event run_until_event(int64_t inp);
    };

}