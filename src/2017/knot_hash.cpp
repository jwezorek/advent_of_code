#include "knot_hash.h"
#include <iterator>
#include <ranges>
#include <sstream>
#include <algorithm>

namespace r = std::ranges;
namespace rv = std::ranges::views;

/*------------------------------------------------------------------------------------------------*/

namespace {
    using state = aoc::knot_hash_state;

    int incr(int i, size_t sz, int amnt = 1) {
        return (i + amnt) % sz;
    }

    int decr(int i, size_t sz, int amnt = 1) {
        amnt = amnt % sz;
        return (i - amnt + sz) % sz;
    }

    void reverse_span(std::vector<int>& list, int start, int sz) {
        int i = start;
        int j = incr(start, list.size(), sz - 1);
        for (int k = 0; k < sz / 2; ++k) {
            std::swap(list[i], list[j]);
            i = incr(i, list.size());
            j = decr(j, list.size());
        }
    }

    void perform_knot_hash_step(std::vector<int>& list, state& state, int len) {
        reverse_span(list, state.pos, len);
        state.pos = incr(state.pos, list.size(), len + state.skip_size);
        state.skip_size++;
    }

    std::vector<int> process_input(const std::string& str) {
        auto lengths = str | rv::transform(
            [](char ch)->int {
                return ch;
            }
        ) | r::to<std::vector>();
        const static std::array<int, 5> suffix = { {17, 31, 73, 47, 23} };
        r::copy(suffix, std::back_inserter(lengths));
        return lengths;
    }

    std::vector<int> make_dense(const std::vector<int>& sparse) {
        return sparse | rv::chunk(16) | rv::transform(
            [](auto rng)->int {
                return r::fold_left(
                    rng, 0, [](int lhs, int rhs) { return lhs ^ rhs; }
                );
            }
        ) | r::to<std::vector>();
    }

    std::string to_hex(int v) {
        std::stringstream ss;
        ss << std::hex << v;
        auto str = ss.str();
        if (str.size() == 1) {
            str = std::string{ '0' } + str;
        }
        return str;
    }
}

std::vector<int> aoc::knot_hash_to_values(const std::string& inp) {
    auto sparse = rv::iota(0, 256) | r::to<std::vector>();
    knot_hash_state state;
    auto lengths = process_input(inp);
    for (int i = 0; i < 64; ++i) {
        aoc::knot_hash_iteration(sparse, state, lengths);
    }
    return make_dense(sparse);
}

std::string aoc::knot_hash(const std::string& inp) {
    return knot_hash_to_values(inp) | rv::transform(
        to_hex
    ) | rv::join | r::to<std::string>();
}

void aoc::knot_hash_iteration(std::vector<int>& list, knot_hash_state& state, const std::vector<int>& lengths) {
    for (const auto len : lengths) {
        perform_knot_hash_step(list, state, len);
    }
}

aoc::knot_hash_state::knot_hash_state() : pos(0), skip_size(0)
{
}
