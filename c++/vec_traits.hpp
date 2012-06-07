
#ifndef VEC_TRAITS_HPP
#define VEC_TRAITS_HPP

#include <cstddef>
#include <type_traits>

#include <boost/mpl/if.hpp>

#include "vec_fwd.hpp"

template <typename T>
struct is_vec : public std::false_type { };

template <typename T>
struct is_vec<const T> : public is_vec<T> { };

template <typename T>
struct is_vec<T &> : public is_vec<T> { };

template <typename T>
struct is_vec<T &&> : public is_vec<T> { };

template <typename T, std::size_t N>
struct is_vec< vec<T, N> > : public std::true_type { };

template <typename T, std::size_t N>
struct is_vec< vec<T &, N> > : public std::true_type { };

namespace detail {
	template <typename Target, std::size_t Size, bool Writable>
	struct vec_traits_base {
		static constexpr std::size_t static_size = Size;
		static constexpr bool writable = Writable;
		typedef Target target_type;
		typedef target_type & reference_type;
		typedef vec<target_type, static_size> target_vec;
		typedef vec<reference_type, static_size> reference_vec;
		typedef typename index_range<0, static_size>::type component_indices;
	};
}

template <typename T>
struct vec_traits : public detail::vec_traits_base<T, 1, true> { };

template <typename T>
struct vec_traits<T&> : public vec_traits<T> { };

template <typename T>
struct vec_traits<T&&> : public vec_traits<const T> { };

template <typename T>
struct vec_traits<const T> : public vec_traits<T> {
	static constexpr bool writable = false;
};

template <typename T, std::size_t N>
struct vec_traits< vec<T, N> > : public detail::vec_traits_base<T, N, true> { };

template <typename T, std::size_t N>
struct vec_traits< const vec<T, N> > : public detail::vec_traits_base<T, N, false> { };

template <typename T, std::size_t N>
struct vec_traits< vec<T &, N> > : public detail::vec_traits_base<T, N, true> { };

template <typename T, std::size_t N>
struct vec_traits< const vec<T &, N> > : public detail::vec_traits_base<T, N, true> { };

struct na { };

template <typename Vec0, typename Vec1>
struct compatible_vecs {
	static constexpr bool value = is_vec<Vec0>::value && is_vec<Vec1>::value
		&& std::is_same<typename vec_traits<Vec0>::target_type, typename vec_traits<Vec1>::target_type>::value
		&& vec_traits<Vec0>::static_size == vec_traits<Vec1>::static_size;
};

template <typename T = na, typename... Args>
struct concatenated_type { };

//! concatenate two values
template <typename T, typename Arg0, typename Arg1>
struct concatenated_type<T, Arg0, Arg1> {
	
	static_assert(
		!std::is_same<T, na>::value
		|| std::is_same<typename vec_traits<Arg0>::target_type, typename vec_traits<Arg1>::target_type>::value,
		"cannot concatenate vectors of different types"
	);
	
	typedef typename boost::mpl::if_c<
		std::is_same<T, na>::value,
		typename boost::mpl::if_c<
			vec_traits<Arg0>::writable && vec_traits<Arg1>::writable,
			typename vec_traits<Arg0>::reference_type,
			typename vec_traits<Arg0>::target_type
		>::type,
		T
	>::type value_type;
	
	typedef vec<value_type, vec_traits<Arg0>::static_size + vec_traits<Arg1>::static_size> type;
	
};

//! "concatenate" a single value
template <typename T, typename Arg>
struct concatenated_type<T, Arg> {
	
	typedef typename boost::mpl::if_c<
		std::is_same<T, na>::value,
		typename boost::mpl::if_c<
			vec_traits<Arg>::writable,
			typename vec_traits<Arg>::reference_type,
			typename vec_traits<Arg>::target_type
		>::type,
		T
	>::type value_type;
	
	typedef typename boost::mpl::if_c<
		is_vec<Arg>::value,
		Arg,
		vec<value_type, 1>
	>::type type;
	
};

//! concatenate any number of values
template <typename T, typename Arg0, typename Arg1, typename... Tail>
struct concatenated_type<T, Arg0, Arg1, Tail...> {
	typedef typename concatenated_type<T, typename concatenated_type<T, Arg0, Arg1>::type, Tail...>::type type;
};

#endif // VEC_TRAITS_HPP
