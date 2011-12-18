
#include <algorithm>
#include <iostream>
#include <utility>

template <typename T, class Op, bool Enable>
struct op_ref {
	
	T & left;
	const Op & op;
	
	inline op_ref(T & _left, const Op & _op) : left(_left), op(_op) { }
	inline op_ref(const op_ref & o) : left(o.left), op(o.op) { }
	
};

template <typename T, class Op>
inline op_ref<T, Op, Op::is_binary_operator> operator<(T & left, const Op & op) {
	return op_ref<T, Op, Op::is_binary_operator>(left, op);
}

template <typename T, class Op>
inline void operator>(const op_ref<T, Op, Op::is_binary_operator> ref, T & right) {
	ref.op(ref.left, right);
}

struct binary_operator { static const bool is_binary_operator = true; };



static const struct swap_operator : public binary_operator {
	
	template <typename T>
	inline void operator()(T & a, T & b) const {
		std::swap(a, b);
	}
	
} swap = swap_operator();



int main() {
	
	float a = 0;
	
	float b = 1;
	
	a <swap> b;
	
	std::cout << "a=" << a << " b=" << b << std::endl;
	
	return a < b;
}
