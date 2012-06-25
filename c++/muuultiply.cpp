
#include <iostream>
#include <utility>
#include <type_traits>
#include <cstdint>

namespace detail {

template <class C, std::size_t c>
struct plus {
	
	C && v;
	
	constexpr plus(C && v) : v(std::forward<C>(v)) { }
	
	constexpr plus<C, c + 1> operator *() const {
		return plus<C, c + 1>(std::forward<C>(v));
	}
	
};

} // namespace detail

#define AUTO_RETURN(expr) decltype(expr) { return expr; }

template <std::size_t c>
void multiply(int, int) { }

template <typename A, typename B, typename = typename std::decay<A>::type::multi_tag>
constexpr auto operator *(A && a, B && b)
	-> AUTO_RETURN(multiply<1>(std::forward<A>(a), std::forward<B>(b)))


template <typename A, typename B, std::size_t c>
constexpr auto operator *(A && a, const detail::plus<B, c> & p)
	-> AUTO_RETURN(multiply<c + 1>(std::forward<A>(a), std::forward<B>(p.v)))

template <typename C, typename = typename std::decay<C>::type::multi_tag>
constexpr detail::plus<C, 1> operator *(C && c) { return detail::plus<C, 1>(std::forward<C>(c)); }

// -----------------------------------------------------------------------

struct C {
	
	typedef void multi_tag;
	
	C(const char * c) : c(c) { }
	const char * c;
	
	C() = delete;
	
	// noncopyable
	C(const C &) = delete;
	C & operator=(const C &) = delete;
	
};

template <std::size_t exp>
void multiply(const C & a, const C & b) {
	std::cout << a.c << ' ' << '*' << '^' << exp << ' ' << b.c  << std::endl;
}

// --------------------------------------------------------------------------

C operator "" _c(const char * c, std::size_t) {
	return { c };
}

// --------------------------------------------------------------------------

int main() {
	
	C a("a"), b("b");
	a * b;
	a ** b;
	a *** b;
	a **** b;
	"d"_c ***** "e"_c;
	"f"_c ************************************************************************* "g"_c;
	
	return 0;
}
