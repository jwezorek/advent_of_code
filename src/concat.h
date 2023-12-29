#include <ranges>

namespace aoc {

    namespace detail {

        namespace r = std::ranges;
        namespace v = std::views;

        template <typename T, typename... Rest>
        struct get_first_type_t {
            using Type = T;
        };

        template <typename... Pack>
        using get_first_type = get_first_type_t<Pack...>::Type;

        template <typename... Ranges>
        struct expanded_iter;

        template <typename R, typename... Ranges>
        struct expanded_iter<R, Ranges...> {
            using elt_t = r::range_value_t<R>;
            using iter_t = r::iterator_t<R>;
            R& r;
            iter_t iter;
            expanded_iter<Ranges...> rest_iter;
            expanded_iter(R& r, Ranges&... rest) :r(r), rest_iter(rest...) {
                iter = r.begin();
            }
            bool is_end() const {
                if (iter == r.end()) {
                    return rest_iter.is_end();
                }
                else {
                    return false;
                }
            }

            auto current() {
                if constexpr (sizeof...(Ranges) != 0) {
                    if (iter == r.end()) {
                        return rest_iter.current();
                    }
                }
                return *iter;
            }

            void next() {
                if (iter == r.end()) {
                    rest_iter.next();
                }
                else {
                    ++iter;
                }
            }
        };

        template<>
        struct expanded_iter<> {
            bool is_end() const { return true; }
            void next() {}
        };

        template <typename... Views>
        class concat_iter {
            expanded_iter<Views...> exp_iter;
        public:
            using EltT = r::range_value_t<get_first_type<Views...>>;

            concat_iter(Views&... views) :exp_iter(views...) { }
            concat_iter(const concat_iter&) = delete;
            concat_iter(concat_iter&& other) = default;
            concat_iter& operator=(const concat_iter&) = delete;
            concat_iter& operator=(concat_iter&&) = delete;

            bool is_end() const {
                return exp_iter.is_end();
            }
            EltT operator*() {
                return exp_iter.current();
            }
            concat_iter& operator++() {
                exp_iter.next();
                return *this;
            }
        private:
        };

        struct EndIter {
        };

        template <typename... Rs>
        bool operator==(const concat_iter<Rs...>& iter, EndIter) {
            return iter.is_end();
        }

        template <typename... Rs>
        class concat_view {
            using IterT = concat_iter<decltype(v::all(std::forward<Rs>(std::declval<Rs&>())))...>;
            std::tuple<decltype(v::all(std::forward<Rs>(std::declval<Rs&>())))...> views;
        public:
            concat_view(Rs&&... ranges) :views(v::all(std::forward<Rs>(ranges))...) {
            }

            IterT begin() {
                return std::make_from_tuple<IterT>(views);
            }
            EndIter end() { return {}; };

        private:
        };
    }

    template <typename... Rs>
        requires (std::ranges::range<Rs> && ...)
    detail::concat_view<Rs...> concat(Rs&&... rs) {
        detail::concat_view<Rs...> v(std::forward<Rs>(rs)...);
        return v;
    }
}