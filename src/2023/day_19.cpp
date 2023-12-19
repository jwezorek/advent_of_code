#include "days.h"
#include "../util.h"
#include <print>
#include <ranges>
#include <tuple>
#include <algorithm>
#include <unordered_map>
#include <boost/icl/discrete_interval.hpp>
#include <boost/icl/interval_set.hpp>
#include <iostream>

namespace r = std::ranges;
namespace rv = std::ranges::views;
namespace icl = boost::icl;

namespace {

    struct rule {
        char category;
        bool less_than;
        int value;
        std::string dest;

        bool is_default() const {
            return category == '\0';
        }
    };

    struct workflow {
        std::string name;
        std::vector<rule> rules;
    };

    struct part {
        int x;
        int m;
        int a;
        int s;

        int operator[](char ch) const {
            switch (ch) {
                case 'x': return x;
                case 'm': return m;
                case 'a': return a;
                case 's': return s;
            }
            throw std::runtime_error("something is wrong");
        }
    };

    rule parse_rule(const std::string& str) {
        if (!str.contains(':')) {
            return { '\0', false, -1, str };
        }
        auto pieces = aoc::split(str, ':');
        bool less_than = pieces.front().contains('<');
        auto left_pieces = aoc::split(pieces.front(), less_than ? '<' : '>');
        return {
            left_pieces.front().front(),
            less_than,
            std::stoi(left_pieces.back()),
            pieces.back()
        };
    }

    workflow parse_workflow(const std::string& line) {
        auto pieces = aoc::split(line, '{');
        auto rule_pieces = aoc::split(
            pieces[1].substr(0, pieces[1].size() - 1), ',');
        return { pieces.front(),
            rule_pieces | rv::transform(parse_rule) | r::to<std::vector<rule>>()
        };
    }

    part parse_part(const std::string& line) {
        auto values = aoc::extract_numbers(line);
        if (values.size() != 4) {
            throw std::runtime_error("something is wrong");
        }
        return {
            values[0],
            values[1],
            values[2],
            values[3]
        };
    }

    std::vector<workflow> parse_workflows(auto lines) {
        return lines | rv::transform(
            parse_workflow
        ) | r::to<std::vector<workflow>>();
    }

    std::vector<part> parse_parts(auto lines) {
        return lines | rv::transform(
            parse_part
        ) | r::to<std::vector<part>>();
    }

    std::tuple<std::vector<workflow>, std::vector<part>> parse_input(
            const std::vector<std::string>& inp) {
        auto iter = r::find_if(inp, [](auto&& line) {return line.empty(); });
        return {
            parse_workflows(r::subrange(inp.begin(), iter)),
            parse_parts(r::subrange(std::next(iter), inp.end()))
        };
    }

    using workflow_tbl = std::unordered_map<std::string, workflow>;

    workflow_tbl make_workflow_table(const std::vector<workflow>& workflows) {
        return workflows |
            rv::transform(
                [](auto&& wf)->workflow_tbl::value_type {
                    return { wf.name, wf };
                }
            ) | r::to<workflow_tbl>();
    }

    std::string process_rule(const rule& r, const part& p) {
        if (r.is_default()) {
            return r.dest;
        }
        auto part_val = p[r.category];
        if (part_val < r.value && r.less_than) {
            return r.dest;
        } else if (part_val > r.value && (!r.less_than)) {
            return r.dest;
        }
        return {};
    }

    std::string process_workflow(const workflow& wf, const part& p) {
        for (const auto& rule : wf.rules) {
            auto result = process_rule(rule, p);
            if (!result.empty()) {
                return result;
            }
        }
        throw std::runtime_error("something is wrong");
    }

    bool part_is_accepted(const workflow_tbl& workflows, const part& p) {
        std::string wf = "in";
        while (wf != "A" && wf != "R") {
            const auto& curr_workflow = workflows.at(wf);
            wf = process_workflow(curr_workflow, p);
        }
        return wf == "A";
    }

    int score_part(const part& p) {
        return p.x + p.m + p.a + p.s;
    }

    using interval = icl::discrete_interval<int>;
    using interval_set = icl::interval_set<int>;

    const int k_max_val = 4;

    interval make_interval(int from, int to) {
        return interval(from, to, icl::interval_bounds::closed());
    }

    interval less_than(int val) {
        return make_interval(1, val - 1);
    }

    interval greater_than(int val) {
        return make_interval(val + 1, k_max_val);
    }

    interval_set union_of_sets(const interval_set& lhs, const interval_set& rhs) {
        interval_set union_;
        for (const auto& inter : lhs) {
            union_.insert(inter);
        }
        for (const auto& inter : rhs) {
            union_.insert(inter);
        }
        return union_;
    }

    struct part_ranges {
        interval_set x;
        interval_set m;
        interval_set a;
        interval_set s;

        part_ranges(interval_set x, interval_set m, interval_set a, interval_set s) :
            x(x), m(m), a(a), s(s)
        {}

        part_ranges() {
            x.insert(make_interval(1, k_max_val));
            m.insert(make_interval(1, k_max_val));
            a.insert(make_interval(1, k_max_val));
            s.insert(make_interval(1, k_max_val));
        }

        interval_set& operator[](char ch) {
            switch (ch) {
                case 'x': return x;
                case 'm': return m;
                case 'a': return a;
                case 's': return s;
            }
            throw std::runtime_error("something is wrong");
        }

        int64_t size() const {
            return icl::cardinality(x) *
                icl::cardinality(m) *
                icl::cardinality(a) *
                icl::cardinality(s);
        }
    };

    part_ranges empty_ranges() {
        return { {}, {}, {}, {} };
    }

    part_ranges union_of_part_ranges(const part_ranges& lhs, const part_ranges& rhs) {
        return {
            union_of_sets(lhs.x, rhs.x),
            union_of_sets(lhs.m, rhs.m),
            union_of_sets(lhs.a, rhs.a),
            union_of_sets(lhs.s, rhs.s)
        };
    }

    part_ranges intersection_of_part_ranges(const part_ranges& lhs, const part_ranges& rhs) {
        return {
            lhs.x & rhs.x,
            lhs.m & rhs.m,
            lhs.a & rhs.a,
            lhs.s & rhs.s
        };
    }

    std::tuple<part_ranges, part_ranges> split_ranges(const rule& r) {
        interval_set applicable_range;
        interval_set inapplicable_range;
        if (r.less_than) {
            applicable_range = interval_set{ less_than(r.value) };
            inapplicable_range = interval_set{ greater_than(r.value - 1) };
        }
        else {
            applicable_range = interval_set{ greater_than(r.value) };
            inapplicable_range = interval_set{ less_than(r.value + 1) };
        }

        part_ranges applicable;
        part_ranges inapplicable;
        applicable[r.category] = applicable_range;
        inapplicable[r.category] = inapplicable_range;

        return { std::move(applicable), std::move(inapplicable) };
    }

    std::tuple<part_ranges, part_ranges> split_range_on_rule(const part_ranges& input, const rule& rule) {
        if (rule.is_default()) {
            return { input, empty_ranges() };
        }
        auto [applicable, innapplicable] = split_ranges(rule);
        return {
            intersection_of_part_ranges(applicable, input),
            intersection_of_part_ranges(innapplicable, input)
        };
    }

    part_ranges find_accepted_ranges(const part_ranges& input, const workflow_tbl& workflows, const std::string& start) {
        part_ranges output = empty_ranges();
        const auto& curr_workflow = workflows.at(start);
        auto current_range = input;
        for (const auto& rule : curr_workflow.rules) {
            auto [applicable, inapplicable] = split_range_on_rule(current_range, rule);

            part_ranges add_to_output = empty_ranges();
            if (rule.dest == "A") {
                add_to_output = applicable;
            } else if (rule.dest != "R") {
                add_to_output = find_accepted_ranges(applicable, workflows, rule.dest);
            }

            output = union_of_part_ranges(output, add_to_output);
            auto debug = output.size();

            current_range = inapplicable;
        }
        return output;
    }


}

/*------------------------------------------------------------------------------------------------*/


void aoc::y2023::day_19(const std::string& title) {

    auto [workflows, parts] = parse_input(
        aoc::file_to_string_vector(aoc::input_path(2023, 19, "test2"))
    );
    auto tbl = make_workflow_table(workflows);
    std::println("--- Day 19: {} ---\n", title);
    std::println("  part 1: {}",
        r::fold_left(
            parts | rv::filter(
                [&tbl](auto&& p) {
                    return part_is_accepted(tbl, p);
                }
            ) | rv::transform(
                score_part
            ),
            0,
            std::plus<>()
        )
    );

    auto test = find_accepted_ranges(part_ranges(), tbl, "in");

    int64_t x = icl::cardinality(test.x);
    int64_t m = icl::cardinality(test.m);
    int64_t aa = icl::cardinality(test.a);
    int64_t s = icl::cardinality(test.s);
    std::println("{}", x * m * aa * s);

    auto a = make_interval(6, 10);
    auto b = make_interval(12, 13);
    auto d = make_interval(1, 6);
    auto e = make_interval(13, 20);
    auto foo = union_of_sets(interval_set(a), interval_set(b));
    auto bar = union_of_sets(interval_set(d), interval_set(e));

    foo = foo & bar;

    for (auto it = foo.begin(); it != foo.end(); it++) {
        std::cout << it->lower() << ", " << it->upper() << "\n";
    }
}
    