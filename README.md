# advent_of_code

advent of code in C++23. Currently has full solutions to 2022 and 2023.

I ported my 2022 solution over from C++17 + ranges-v3 to use C++23 and standard ranges. The main issue I ran into was not having views::concat so I've included [netcan's implementation of concat from here](https://github.com/netcan/recipes/blob/master/cpp/metaproggramming/ranges/Calendar.cpp#L585). Beyond concat I had to change views::remove_if to views::filter, replaced one usage of partial_sum with a transform_view + a mutable lambda, and excised one usage of views::cycle.

I plan to bring my AoC 2019 solutions into here next (and finish 2019) and then I may do some earlier years.
