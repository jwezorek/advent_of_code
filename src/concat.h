#include <ranges>
#include <variant>

namespace aoc {
    namespace ranges = std::ranges;

    template<ranges::viewable_range... Rngs>
        requires (sizeof...(Rngs) > 1)
    struct concat_view {
        static constexpr size_t nRngs = sizeof...(Rngs);
        using RNGs = std::tuple<Rngs...>;
        concat_view() = default;
        explicit concat_view(Rngs... rngs) : rngs{ std::move(rngs)... }
        { }

        struct ConcatIterator {
            using difference_type = std::ptrdiff_t;
            using value_type = std::common_type_t<ranges::range_reference_t<Rngs>...>;

            ConcatIterator() = default;
            ConcatIterator(RNGs* rngs) : rngs(rngs)
            {
                its.template emplace<0>(ranges::begin(std::get<0>(*rngs)));
            }

            ConcatIterator& operator++() {
                // TODO: check empty range, skip it
                std::visit([&](auto&& iter) {
                    constexpr size_t idx = iter.value;
                    if ((iter.iterator = ranges::next(iter.iterator))
                        == ranges::end(std::get<idx>(*rngs))) {
                        if constexpr (idx + 1 < nRngs) {
                            its.template emplace<idx + 1>(
                                ranges::begin(std::get<idx + 1>(*rngs))
                            );
                        }
                    }
                    }, its);
                return *this;
            }
            ConcatIterator operator++(int) {
                ConcatIterator tmp(*this);
                ++*this;
                return tmp;
            }
            using reference = std::common_reference_t<ranges::range_reference_t<Rngs>...>;
            reference operator*() const {
                return std::visit([](auto&& iter) -> reference {
                    return *iter.iterator;
                    }, its);
            }

            friend bool operator==(const ConcatIterator& iter, std::default_sentinel_t) {
                return iter.its.index() == nRngs - 1 &&
                    (std::get<nRngs - 1>(iter.its).iterator ==
                        ranges::end(std::get<nRngs - 1>(*iter.rngs)));
            }

            friend bool operator==(const ConcatIterator& lhs, const ConcatIterator& rhs) = default;

        private:
            template<size_t N, typename Rng>
            struct IteratorWithIndex : std::integral_constant<size_t, N> {
                IteratorWithIndex() = default;
                IteratorWithIndex(ranges::iterator_t<Rng> iterator) :
                    iterator(std::move(iterator)) {}
                ranges::iterator_t<Rng> iterator;
                friend bool operator==(const IteratorWithIndex& lhs, const IteratorWithIndex& rhs) = default;
            };

            template<size_t ...Is>
            static constexpr auto iteratorVariantGenerator(std::index_sequence<Is...>)
                -> std::variant<IteratorWithIndex<Is, std::tuple_element_t<Is, RNGs>>...>;

            decltype(iteratorVariantGenerator(std::make_index_sequence<nRngs>{})) its;
            RNGs* rngs{};
        };

        ConcatIterator begin() { return { &this->rngs }; }

        std::default_sentinel_t end() { return {}; }

    private:
        RNGs rngs;
    };

    inline constexpr auto concat = [] <ranges::viewable_range... Rngs> (Rngs&&... rngs) {
        return concat_view{ std::forward<Rngs>(rngs)... };
    };


}