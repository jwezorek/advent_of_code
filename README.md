# advent_of_code

advent of code in C++23. Currently has full solutions to 2022 and 2023.

I ported my 2022 solution over from C++17 + ranges-v3 to use C++23 and standard ranges. The main issue I ran into was not having views::concat so I've included [netcan's implementation of concat from here](https://github.com/netcan/recipes/blob/master/cpp/metaproggramming/ranges/Calendar.cpp#L585). Beyond concat I replaced views::remove_if with views::filter, replaced accumulate with fold_left, replaced one usage of partial_sum with a transform_view + a mutable lambda, and excised one usage of views::cycle.

I also migrated a partially completed AoC 2021 into this project and finished that year. Fewer issues with migration because it had no dependency on ranges-v3.
