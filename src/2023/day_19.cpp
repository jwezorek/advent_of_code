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

    const int k_max_val = 4000;

    interval make_interval(int from, int to) {
        return interval(from, to, icl::interval_bounds::closed());
    }

    interval less_than(int val) {
        return make_interval(1, val - 1);
    }

    interval greater_than(int val) {
        return make_interval(val + 1, k_max_val);
    }

    struct part_range {
        interval x;
        interval m;
        interval a;
        interval s;

        part_range(interval x, interval m, interval a, interval s) :
            x(x), m(m), a(a), s(s)
        {}

        interval& operator[](char ch) {
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

    part_range empty_ranges() {
        return { {}, {}, {}, {} };
    }

    part_range full_ranges() {
        return { make_interval(1, k_max_val),
            make_interval(1, k_max_val),
            make_interval(1, k_max_val),
            make_interval(1, k_max_val) 
        };
    }

    part_range intersection_of_part_ranges(const part_range& lhs, const part_range& rhs) {
        return {
            lhs.x & rhs.x,
            lhs.m & rhs.m,
            lhs.a & rhs.a,
            lhs.s & rhs.s
        };
    }

    std::tuple<part_range, part_range> split_ranges(const rule& r) {
        interval applicable_range;
        interval inapplicable_range;

        if (r.less_than) {
            applicable_range = less_than(r.value) ;
            inapplicable_range = greater_than(r.value - 1);
        } else {
            applicable_range = greater_than(r.value);
            inapplicable_range = less_than(r.value + 1);
        }

        part_range applicable = full_ranges();
        part_range inapplicable = full_ranges();
        applicable[r.category] = applicable_range;
        inapplicable[r.category] = inapplicable_range;

        return { std::move(applicable), std::move(inapplicable) };
    }

    std::tuple<part_range, part_range> split_range_on_rule(const part_range& input, const rule& rule) {
        if (rule.is_default()) {
            return { input, empty_ranges() };
        }
        auto [applicable, innapplicable] = split_ranges(rule);
        return {
            intersection_of_part_ranges(applicable, input),
            intersection_of_part_ranges(innapplicable, input)
        };
    }

    int64_t count_accepted_ranges(const workflow_tbl& workflows, 
        const std::string& start, const part_range& input = full_ranges()) {
        int64_t output = 0;
        const auto& curr_workflow = workflows.at(start);
        auto current_range = input;

        for (const auto& rule : curr_workflow.rules) {
            auto [applicable, inapplicable] = split_range_on_rule(current_range, rule);

            if (rule.dest == "A") {
                output += applicable.size();
            } else if (rule.dest != "R") {
                output += count_accepted_ranges(workflows, rule.dest, applicable);
            }

            current_range = inapplicable;
        }

        return output;
    }
}

/*------------------------------------------------------------------------------------------------*/


void aoc::y2023::day_19(const std::string& title) {

    auto [wfs, parts] = parse_input(
        aoc::file_to_string_vector(aoc::input_path(2023, 19))
    );
    auto workflows = make_workflow_table(wfs);

    std::println("--- Day 19: {} ---\n", title);
    std::println("  part 1: {}",
        r::fold_left(
            parts | rv::filter(
                [&workflows](auto&& p) {
                    return part_is_accepted(workflows, p);
                }
            ) | rv::transform(
                score_part
            ),
            0,
            std::plus<>()
        )
    );

    std::println("  part 2: {}",
        count_accepted_ranges(workflows, "in")
    );

}
    