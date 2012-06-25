
#include <iostream>
#include <utility>
#include <type_traits>
#include <cstdint>

// --------------------------------------------------------------------------

template <typename T, T Value>
struct meta_int : public std::integral_constant<T, Value> {
	
	template <typename TT, typename = typename std::enable_if<std::is_convertible<T, TT>::value>::type>
	constexpr operator TT() const { return TT(Value); }
	
	static constexpr T get() { return Value; }
	
};

#define META { return { }; }

#define META_INT_BINARY_OPERATOR(op) \
	template <typename T0, T0 v0, typename T1, T1 v1> \
	constexpr auto operator op(meta_int<T0, v0>, meta_int<T1, v1>) \
		-> meta_int<decltype(v0 op v1), (v0 op v1)> META
META_INT_BINARY_OPERATOR(+)
META_INT_BINARY_OPERATOR(-)
META_INT_BINARY_OPERATOR(*)
META_INT_BINARY_OPERATOR(/)
META_INT_BINARY_OPERATOR(%)
META_INT_BINARY_OPERATOR(&)
META_INT_BINARY_OPERATOR(|)
META_INT_BINARY_OPERATOR(==)
META_INT_BINARY_OPERATOR(!=)
//META_INT_BINARY_OPERATOR(>)
META_INT_BINARY_OPERATOR(<)
META_INT_BINARY_OPERATOR(>=)
META_INT_BINARY_OPERATOR(<=)
#undef META_INT_BINARY_OPERATOR

template <typename T0, T0 v0, typename T1, T1 v1, typename = typename std::enable_if<(v0 <= v1)>::type>
constexpr auto min(meta_int<T0, v0>, meta_int<T1, v1>) -> meta_int<T0, v0> META
template <typename T0, T0 v0, typename T1, T1 v1, typename = typename std::enable_if<(v0 > v1)>::type>
constexpr auto min(meta_int<T0, v0>, meta_int<T1, v1>) -> meta_int<T1, v1> META

template <typename T0, T0 v0, typename T1, T1 v1, typename = typename std::enable_if<(v0 >= v1)>::type>
constexpr auto max(meta_int<T0, v0>, meta_int<T1, v1>) -> meta_int<T0, v0> META
template <typename T0, T0 v0, typename T1, T1 v1, typename = typename std::enable_if<(v0 < v1)>::type>
constexpr auto max(meta_int<T0, v0>, meta_int<T1, v1>) -> meta_int<T1, v1> META

template <std::size_t Accum, char... Chars>
struct make_meta_int {
	static_assert(sizeof...(Chars) == 0, "bad overload");
	typedef meta_int<std::size_t, Accum> type;
};

template <std::size_t Accum, char C0, char... Chars>
struct make_meta_int<Accum, C0, Chars...> {
	static_assert(C0 >= '0' && C0 <= '9', "invalid digit");
	static_assert((Accum == 0 || Accum * 10 > Accum) && (Accum * 10) / 10 == Accum, "overflow detected");
	static_assert(C0 != 0 || sizeof...(Chars) == 0, "hex and octal formats not supported");
	typedef typename make_meta_int<Accum * 10 + std::size_t(C0 - '0'), Chars...>::type type;
};

// TODO support float and hex and octal literals?
template <char... Chars>
constexpr auto operator "" _m() -> typename make_meta_int<0, Chars...>::type META

#define META_RETURN_IF(Cond, Result) \
	typename std::enable_if<decltype(Cond)::value, decltype(Result)>::type META
	
#define META_RETURN(Result) \
	decltype(Result) META

template <typename T, T... Chars>
struct meta_string;

template <typename T, T... A, T... B>
constexpr auto operator+(meta_string<T, A...>, meta_string<T, B...>) -> meta_string<T, A..., B...> META

template <typename T, T... A, T B>
constexpr auto operator+(meta_string<T, A...>, meta_int<T, B>) -> meta_string<T, A..., B> META

template <typename T, T A, T... B>
constexpr auto operator+(meta_int<T, A>, meta_string<T, B...>) -> meta_string<T, A, B...> META

template <typename T, T... Chars>
struct meta_string {
	
	static constexpr std::size_t _length = sizeof...(Chars);
	
	typedef meta_int<bool, false> False;
	typedef meta_int<bool, true> True;
	
	typedef meta_int<std::size_t, 0> Zero;
	typedef meta_int<std::size_t, 1> One;
	typedef meta_int<std::size_t, _length> End;
	
	static constexpr T _data[_length] = { Chars... };
	
	template <T C>
	struct _append { typedef meta_string<T, Chars..., C> type; };
	
	// ----
	
	static constexpr const T * data() { return _data; }
	
	// ----
	
	static constexpr auto length() -> meta_int<std::size_t, _length> META
	static constexpr auto size() -> META_RETURN(length())
	static constexpr auto empty() -> META_RETURN(size() == 0_m)
	
	static constexpr auto npos() -> meta_int<std::size_t, std::size_t(-1)> META
	
	template <typename Index>
	static constexpr auto at(Index) -> meta_int<T, _data[Index::value]> META
	
	/* TODO
	static constexpr auto head(Zero) -> meta_string<T> META
	template <typename Length>
	static constexpr auto head(Length l) -> META_RETURN_IF(
		l != 0_m,
		head(l - 1_m) + at(l - 1_m)
	)
	*/
	template <std::size_t Length, typename Dummy = void>
	struct _head {
		typedef decltype(typename _head<Length - 1>::type() + at(meta_int<std::size_t, Length - 1>())) type;
	};
	template <typename Dummy>
	struct _head<0, Dummy> {
		typedef meta_string<T> type;
	};
	template <typename Length>
	static constexpr auto head(Length) -> typename _head<Length::value>::type META
	
	/* TODO
	static constexpr auto tail(Zero) -> meta_string<T> META
	template <typename Length>
	static constexpr auto tail(Length l) -> META_RETURN_IF(
		l != 0_m,
		at(length() - l) + tail(l - 1_m)
	)
	*/
	template <std::size_t Length, typename Dummy = void>
	struct _tail {
		typedef decltype(at(meta_int<std::size_t, length() - Length>()) + typename _tail<Length - 1>::type()) type;
	};
	template <typename Dummy>
	struct _tail<0, Dummy> {
		typedef meta_string<T> type;
	};
	template <typename Length>
	static constexpr auto tail(Length) -> typename _tail<Length::value>::type META
	
	// ----
	
	// TODO s + l > length()
	// TODO check 0 <= s < length()
	template <typename Start, typename Length>
	static constexpr auto substr(Start s, Length l = npos()) -> META_RETURN(
		tail(length() - s).head(l)
	)
	
	// TODO s + l > length()
	// TODO check 0 <= s < length()
	template <typename Start, typename Length, typename Replacement>
	static constexpr auto replace(Start s, Length l, Replacement r) -> META_RETURN(
		head(s) + r + tail(length() - s - l)
	)
	
	// TODO s + l > length()
	// TODO check 0 <= s < length()
	template <typename Start, typename Length>
	static constexpr auto erase(Start s, Length l = npos()) -> META_RETURN(
		head(s) + tail(length() - s - l)
	)
	/*
	// TODO OR NOT TODO, THAT IS THE QUESTION. check 0 <= p < length()
	template <typename Pos, typename String>
	static constexpr auto insert(Pos p, String s) -> META_RETURN(
		replace(p, 0_m, s)
	)
	*/
};

template <typename T, T... Chars>
constexpr std::size_t meta_string<T, Chars...>::_length;

template <typename T, T... Chars>
constexpr T meta_string<T, Chars...>::_data[meta_string<T, Chars...>::_length];

template <char... Chars>
std::ostream & operator<<(std::ostream & os, meta_string<char, Chars...> str) {
	os.write(str.data(), str.length());
	return os;
}
template <typename T, T Value>
std::ostream & operator<<(std::ostream & os, meta_int<T, Value>) {
	return os << Value;
}

// --------------------------------------------------------------------------

#include <boost/preprocessor/repetition/repeat.hpp>
#include <boost/preprocessor/punctuation/comma_if.hpp>
#include <boost/preprocessor/config/limits.hpp>

#define S_DECOMPOSE_HELPER(z, n, data) \
	BOOST_PP_COMMA_IF(n) (n < sizeof(data) ? data[n] : '\0')

#define S_DECOMPOSE(STR)  \
	BOOST_PP_REPEAT(BOOST_PP_LIMIT_REPEAT, S_DECOMPOSE_HELPER, STR)

#define S(s) meta_string<char, S_DECOMPOSE(s)>().head(meta_int<std::size_t, sizeof(s) - 1>())

// --------------------------------------------------------------------------

#include "type_name.hpp"

int main() {
	
	auto str = S("Hello") + S(" ") + S("World") + S("!");
	std::cout << type_name(str) << std::endl;
	
	auto modified = str.substr(1_m, 5_m).replace(1_m, 2_m, S("?-?"));
	std::cout << type_name(modified) << std::endl;
	
	std::cout << "\"" << str << "\" \"" << modified
		<< "\" = " << str.length() << " / " << BOOST_PP_LIMIT_REPEAT << std::endl;
	
	return 0;
}
