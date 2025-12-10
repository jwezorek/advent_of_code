
#include "../util/util.h"
#include "y2025.h"
#include <queue>
#include <functional>
#include <print>
#include <ranges>
#include <algorithm>
#include <unordered_set>

#include <CbcModel.hpp>
#include <OsiClpSolverInterface.hpp>
#include <CoinModel.hpp>
#include <CoinPackedMatrix.hpp>

namespace r = std::ranges;
namespace rv = std::ranges::views;

/*------------------------------------------------------------------------------------------------*/

namespace {
    struct machine {
        std::string lights;
        std::vector<std::vector<int>> wiring;
        std::vector<int> joltage;
    };

    std::tuple<std::string, std::string, std::string> chunk_input(const std::string& str) {
        auto pieces = aoc::split(str, ']');
        auto pieces2 = aoc::split(pieces[1], '{');
        return { pieces[0], pieces2[0], pieces2[1] };
    }

    machine parse_machine(const std::string& inp) {
        auto [lights_str, wire_str, jolt_str] = chunk_input(inp);
        return {
            lights_str | rv::drop(1) | r::to<std::string>(),

            aoc::split(wire_str, ')') | rv::transform(
                [](auto&& s) {return aoc::extract_numbers(s); }
            ) | rv::filter(
                [](auto&& v) {return !v.empty(); } 
            ) | r::to<std::vector>(),

            aoc::extract_numbers(jolt_str)
        };
    }

    uint32_t lights_str_to_num(const std::string& str) {
        auto s = str;
        r::replace(s, '.', '0');
        r::replace(s, '#', '1');
        return static_cast<uint32_t>(std::stoul(s, nullptr, 2));
    }

    uint32_t wiring_schematics_to_num(const std::vector<int>& vals, size_t n) {
        uint32_t output = 0;
        for (auto v : vals) {
            auto shift = static_cast<int>(n) - v - 1;
            uint32_t mask = 1 << shift;
            output = output | mask;
        }
        return output;
    }

    struct state {
        uint32_t val;
        int count;
    };

    int fewest_button_presses(const machine& m) {
        auto target = lights_str_to_num(m.lights);
        auto schematics = m.wiring | rv::transform(
                [&](auto&& vals) {
                    return wiring_schematics_to_num(vals, m.lights.size());
                }
            ) | r::to<std::vector>();

        std::unordered_set<uint32_t> visited;
        std::queue<state> queue;
        queue.push({ 0, 0 });

        while (!queue.empty()) {
            auto curr = queue.front();
            queue.pop();

            if (curr.val == target) {
                return curr.count;
            }

            if (visited.contains(curr.val)) {
                continue;
            }
            visited.insert(curr.val);

            for (const auto btn_press : schematics) {
                queue.push({
                    curr.val ^ btn_press,
                    curr.count + 1
                });
            }
            
        }

        return -1;
    }

    int fewest_total_button_presses(const std::vector<machine>& inp) {
        return r::fold_left(
            inp | rv::transform(fewest_button_presses),
            0,
            std::plus<>()
        );
    }

    int64_t solve_with_cbc(const machine& m) {
        int n_cols = static_cast<int>(m.wiring.size());  // vars
        int n_rows = static_cast<int>(m.joltage.size()); // constraints

        OsiClpSolverInterface solver;

        // build the matrix...
        CoinPackedMatrix matrix(true, 0, 0);
        matrix.setDimensions(n_rows, 0);

        for (int c = 0; c < n_cols; ++c) {
            CoinPackedVector col;
            for (int r : m.wiring[c]) {
                if (r < n_rows) {
                    col.insert(r, 1.0); // coefficient is 1.0 (press adds 1 jolt)
                }
            }
            matrix.appendCol(col);
        }

        // Ax = Target. Lower bound = Upper bound = Target.
        std::vector<double> row_lb(n_rows);
        std::vector<double> row_ub(n_rows);
        for (int i = 0; i < n_rows; ++i) {
            row_lb[i] = static_cast<double>(m.joltage[i]);
            row_ub[i] = static_cast<double>(m.joltage[i]);
        }

        // 0 <= x <= Infinity. minimize sum(x).
        std::vector<double> col_lb(n_cols, 0.0);
        std::vector<double> col_ub(n_cols, solver.getInfinity());
        std::vector<double> obj(n_cols, 1.0); // Cost is 1 per press

        solver.loadProblem(matrix, col_lb.data(), col_ub.data(), obj.data(), row_lb.data(), row_ub.data());

        // set integer Constraints
        for (int i = 0; i < n_cols; ++i) {
            solver.setInteger(i);
        }
        solver.messageHandler()->setLogLevel(0);

        CbcModel model(solver);
        model.setLogLevel(0);
        model.branchAndBound();

        if (model.isProvenOptimal()) {
            double obj_val = model.getObjValue();
            return static_cast<int64_t>(std::round(obj_val));
        }

        return 0;
    }

    int64_t fewest_total_joltage_presses(const std::vector<machine>& inp) {
        int64_t sum = 0;
        for (int i = 0; i < inp.size(); ++i) {
            auto fewest = solve_with_cbc(inp[i]);
            sum += fewest;
        }
        return sum;
    }
}

void aoc::y2025::day_10(const std::string& title) {

    auto inp = aoc::file_to_string_vector(
            aoc::input_path(2025, 10)
        ) | rv::transform(
            parse_machine
        ) | r::to<std::vector>();

    std::println("--- Day 10: {} ---", title);
    std::println("  part 1: {}", fewest_total_button_presses(inp) );
    std::println("  part 2: {}", fewest_total_joltage_presses(inp) );
    
}
