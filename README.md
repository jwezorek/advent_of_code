# advent_of_code

advent of code in C++23. Currently has full solutions to 2020, 2021, 2022, and 2023.

some of the earlier solutions were ported from C++17 without ranges or C++17 with ranges-v3 to use C++23 and standard ranges. The main issue I ran into with dropping the ranges-v2 dependency was not having views::concat so I've included [netcan's implementation of concat from here](https://github.com/netcan/recipes/blob/master/cpp/metaproggramming/ranges/Calendar.cpp#L585). Beyond concat I replaced views::remove_if with views::filter, replaced accumulate with fold_left, replaced one usage of partial_sum with a transform_view + a mutable lambda, and excised one usage of views::cycle.

