
#ifndef UTIL_HPP
#define UTIL_HPP

#include <cstddef>
#include <limits>

#include <boost/mpl/set.hpp>
#include <boost/mpl/size.hpp>
#include <boost/mpl/integral_c.hpp>
#include <boost/mpl/max.hpp>
#include <boost/mpl/fold.hpp>
#include <boost/mpl/vector.hpp>
#include <boost/mpl/insert.hpp>

namespace util { namespace inhibit_adl {

template <class T, class U, class /* moldex */ = typename std::enable_if<
	(!std::is_lvalue_reference<T>::value || std::is_lvalue_reference<U>::value)
	&& std::is_convertible<
			typename std::remove_reference<U>::type*,
			typename std::remove_reference<T>::type*
		>::value
	>::type
>
inline constexpr T&& forward(U && u) {
 return static_cast<T &&>(u);
}

} // namespace inhibit_adl

using inhibit_adl::forward;

} // namespace util

template <std::size_t... Indices>
struct indices {
	template <std::size_t Index>
	struct append {
		typedef indices<Indices..., Index> type;
	};
};

template <std::size_t Start, std::size_t End>
struct index_range {
	static_assert(End > Start, "invalid range");
	typedef typename index_range<Start, End - 1>::type::template append<End - 1>::type type;
};
template <std::size_t End>
struct index_range<End, End> {
	typedef indices<> type;
};

template <template <typename...> class Class, typename... Args>
struct apply {
	typedef Class<Args...> type;
};

template <typename T>
struct dummy { };

template <typename T>
constexpr T & undefined(T & in) {
	return (void(std::numeric_limits<int>::max() + 1), in);
}

template <typename T>
constexpr const T & undefined(const T & in) {
	return (void(std::numeric_limits<int>::max() + 1), in);
}

#define AUTO_RETURN(impl) \
	decltype(impl) { return impl; }


/** reference_array - array that can store references */

//! Get the nth element of an argument pack.
template <std::size_t N, typename... Args>
struct nth_of { static_assert(N < sizeof...(Args), "invalid index"); };
template <std::size_t N, typename Arg0, typename... Args>
struct nth_of<N, Arg0, Args...> {
	static_assert(N < sizeof...(Args) + 1, "invalid index");
	typedef typename nth_of<N - 1, Args...>::type type;
};
template <typename Arg0, typename... Args>
struct nth_of<0, Arg0, Args...> {
	typedef Arg0 type;
};

//! Returns the nth element of an argument pack, preserving rvalue references.
template <std::size_t N, typename Arg0, typename... Args, typename Enable = typename std::enable_if<N == 0, void>::type>
constexpr Arg0 && ref_nth(Arg0 && arg0, Args &&...) {
	return util::forward<Arg0>(arg0);
}
template <std::size_t N, typename Arg0, typename... Args, typename Enable = typename std::enable_if<N != 0, void>::type>
constexpr typename nth_of<N - 1, Args...>::type && ref_nth(Arg0 &&, Args &&... args) {
	return ref_nth<N - 1>(util::forward<Args>(args)...);
}

template <std::size_t... Values>
struct maximum {
	static constexpr std::size_t value = boost::mpl::fold<
		typename apply<
			boost::mpl::vector,
			boost::mpl::integral_c<std::size_t, Values>...
		>::type,
		boost::mpl::integral_c<std::size_t, 0>,
		boost::mpl::max<boost::mpl::_1, boost::mpl::_2>
	>::type::value;
};

template <std::size_t... Values>
struct are_unique {
	static constexpr bool value = sizeof...(Values) == boost::mpl::size<
		typename boost::mpl::fold<
			typename apply<
				boost::mpl::vector,
				boost::mpl::integral_c<std::size_t, Values>...
			>::type,
			boost::mpl::set0<>,
			boost::mpl::insert<boost::mpl::_1, boost::mpl::_2>
		>::type
	>::value;
};

template <typename... Args>
struct are_same {
	static constexpr bool value = 1 >= boost::mpl::size<
		typename boost::mpl::fold<
			typename apply<
				boost::mpl::vector,
				Args...
			>::type,
			boost::mpl::set0<>,
			boost::mpl::insert<boost::mpl::_1, boost::mpl::_2>
		>::type
	>::value;
};

namespace operator_detail {
	struct na { };
	template <typename Left, typename Right>
	na operator<<(const Left &, const Right &);
	template <typename Left, typename Right>
	struct has_operator {
		static constexpr bool lshift = !std::is_same<decltype(*(Left*)0 << *(Right*)0), na>::value;
	};
}

template <typename Left, typename Right>
struct lshift_operator_exists { static constexpr bool value = operator_detail::has_operator<Left, Right>::lshift; };

#endif // UTIL_HPP
