
#include <type_traits>
#include <utility>

namespace detail {

// No need to guve up constexpr for std::forward
template <class T>
constexpr T && forward(typename std::remove_reference<T>::type & t) noexcept {
	return static_cast<T &&>(t);
}
template <class T>
constexpr T && forward(typename std::remove_reference<T>::type && t) noexcept {
	static_assert(!std::is_lvalue_reference<T>::value,
	              "Can not forward rvalue as lvalue.");
	return static_cast<T &&>(t);
}

template <
	typename Left,
	class Op,
	typename = typename std::decay<Op>::type::binary_operator_tag
>
struct bound_operator {
	
	Left && left;
	Op && op;
	
	constexpr bound_operator(Left && left, Op && op)
		: left(forward<Left>(left)), op(forward<Op>(op)) { }
	
};

} // namespace detail

template <typename Left, class Op>
constexpr detail::bound_operator<Left, Op> operator<(Left && left, Op && op) {
	return { detail::forward<Left>(left), detail::forward<Op>(op) };
}

template <typename Left, class Op, typename Right>
constexpr auto operator>(detail::bound_operator<Left, Op> && ref, Right && right)
	-> decltype(ref.op(ref.left, right)) {
	return ref.op(detail::forward<Left>(ref.left), detail::forward<Right>(right));
}


//-------------------------------------------------------------------------


#include <algorithm>

constexpr struct swap_operator {
	
	typedef void binary_operator_tag;
	
	template <typename T>
	void operator()(T & a, T & b) const {
		std::swap(a, b);
	}
	
} swap = swap_operator();


//-------------------------------------------------------------------------


#include <iostream>

int main() {
	
	int a = 0;
	
	int b = 1;
	
	a <swap> b;
	
	std::cout << "a=" << a << " b=" << b << std::endl;
	
	return 0;
}
