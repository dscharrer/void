
#ifndef VEC_BASE_HPP
#define VEC_BASE_HPP

#include <cstddef>

#include "reference_array.hpp"

// Base classes that provide storeage and x, y, z and w members

template <typename T, std::size_t N>
class vec_base {
	
	static_assert(N > 4, "the specialisations, they do nothing");
	
protected:
	
	constexpr vec_base() = default;
	constexpr vec_base(const vec_base & other) = default;
	vec_base(vec_base && other) = default;
	
	template <typename X, typename Y, typename Z, typename W, typename... Args>
	constexpr vec_base(X && x, Y && y, Z && z, W && w, Args &&... tail)
		: x(util::forward<X>(x)),
		  y(util::forward<Y>(y)),
		  z(util::forward<Z>(z)),
		  w(util::forward<W>(w)),
		  internal_data(util::forward<Args>(tail)...) { }
	
public:
	
	T & operator[](std::size_t index) {
		return (index == 0) ? x : (index == 1) ? y : (index == 2) ? z : (index == 3) ? w : internal_data[index - 4];
	}
	
	constexpr const T & operator[](std::size_t index) const {
		return (index == 0) ? x : (index == 1) ? y : (index == 2) ? z : (index == 3) ? w : internal_data[index - 4];
	}
	
	T x;
	T y;
	T z;
	T w;
	
private:
	
	reference_array<T, N - 4> internal_data;
	
};

template <typename T>
class vec_base<T, 4> {
	
protected:
	
	constexpr vec_base() = default;
	constexpr vec_base(const vec_base & other) = default;
	vec_base(vec_base && other) = default;
	
	template <typename X, typename Y, typename Z, typename W>
	constexpr vec_base(X && x, Y && y, Z && z, W && w)
		: x(util::forward<X>(x)),
		  y(util::forward<Y>(y)),
		  z(util::forward<Z>(z)),
		  w(util::forward<W>(w)) { }
	
public:
	
	T & operator[](std::size_t index) {
		return (index == 0) ? x : (index == 1) ? y : (index == 2) ? z : (index == 3) ? w : undefined(x);
	}
	
	constexpr const T & operator[](std::size_t index) const {
		return (index == 0) ? x : (index == 1) ? y : (index == 2) ? z : (index == 3) ? w : undefined(x);
	}
	
	T x;
	T y;
	T z;
	T w;
	
};

template <typename T>
class vec_base<T, 3> {
	
protected:
	
	constexpr vec_base() = default;
	constexpr vec_base(const vec_base & other) = default;
	vec_base(vec_base && other) = default;
	
	template <typename X, typename Y, typename Z>
	constexpr vec_base(X && x, Y && y, Z && z)
		: x(util::forward<X>(x)),
		  y(util::forward<Y>(y)),
		  z(util::forward<Z>(z)) { }
	
public:
	
	T & operator[](std::size_t index) {
		return (index == 0) ? x : (index == 1) ? y : (index == 2) ? z : undefined(x);
	}
	
	constexpr const T & operator[](std::size_t index) const {
		return (index == 0) ? x : (index == 1) ? y : (index == 2) ? z : undefined(x);
	}
	
	T x;
	T y;
	T z;
	
};

template <typename T>
class vec_base<T, 2> {
	
protected:
	
	constexpr vec_base() = default;
	constexpr vec_base(const vec_base & other) = default;
	vec_base(vec_base && other) = default;
	
	template <typename X, typename Y>
	constexpr vec_base(X && x, Y && y) : x(util::forward<X>(x)),  y(util::forward<Y>(y)) { }
	
public:
	
	T & operator[](std::size_t index) {
		return (index == 0) ? x : (index == 1) ? y : undefined(x);
	}
	
	constexpr const T & operator[](std::size_t index) const {
		return (index == 0) ? x : (index == 1) ? y : undefined(x);
	}
	
	T x;
	T y;
	
};

template <typename T>
class vec_base<T, 1> {
	
protected:
	
	constexpr vec_base() = default;
	constexpr vec_base(const vec_base & other) = default;
	vec_base(vec_base && other) = default;
	
	template <typename X>
	constexpr vec_base(X && x) : x(util::forward<X>(x)) { }
	
public:
	
	T & operator[](std::size_t index) {
		return (index == 0) ? x : undefined(x);
	}
	
	constexpr const T & operator[](std::size_t index) const {
		return (index == 0) ? x : undefined(x);
	}
	
	operator T &() { return x; }
	constexpr operator const T &() const { return x; }
	
	T x;
	
};

template <typename T>
class vec_base<T, 0> {
	static_assert(sizeof(dummy<T>) == 0, "vec cannot have size 0");
};

#endif // VEC_BASE_HPP
