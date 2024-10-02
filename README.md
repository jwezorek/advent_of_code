# advent_of_code

advent of code in C++23. Currently has full solutions for all years except 2016.

Some of years were ported from C++17 without ranges or C++17 with ranges-v3 to use C++23 and standard ranges. The main issue I ran into with dropping the ranges-v3 dependency was not having views::concat so I've included [netcan's implementation of concat from here](https://github.com/netcan/recipes/blob/master/cpp/metaproggramming/ranges/Calendar.cpp#L585).

