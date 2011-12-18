
#ifndef REFERENCE_ARRAY_HPP
#define REFERENCE_ARRAY_HPP

#include <cstddef>
#include <type_traits>

#include "util.hpp"

/*!
 * Array that can store references.
 * Componenents are not guaranteed to be stored at consecutive memory locations.
 * Empty arrays are not allowed.
 */
template <typename T, std::size_t N>
class reference_array : public reference_array<T, N - 1> {
	
	typedef reference_array<T, N - 1> parent_type;
	
	T data;
	
	template <
		typename Arg0,
		typename... Args,
		typename Enable = typename std::enable_if<(sizeof...(Args) >= N), void>::type
	>
	constexpr reference_array(Arg0 && arg0, Args &&... args)
		: parent_type(util::forward<Arg0>(arg0), util::forward<Args>(args)...),
		  data(ref_nth<N - 1>(util::forward<Arg0>(arg0), util::forward<Args>(args)...)) { }
	
public:
	
	typedef T value_type;
	static constexpr std::size_t static_size = N;
	
	constexpr reference_array() = default;
	constexpr reference_array(const reference_array & o) = default;
	reference_array(reference_array && o) = default;
	
	template <
		typename... Args,
		typename Enable = typename std::enable_if<sizeof...(Args) == static_size, void>::type
	>
	constexpr reference_array(Args &&... args)
		: parent_type(util::forward<Args>(args)...),
		  data(ref_nth<static_size - 1>(util::forward<Args>(args)...)) { }
	
	T & operator[](std::size_t index) {
		return (index == static_size - 1) ? data : parent_type::operator[](index);
	}
	
	constexpr const T & operator[](std::size_t index) const {
		return (index == static_size - 1) ? data : parent_type::operator[](index);
	}
	
	template <typename OT, std::size_t ON>
	friend class reference_array;
};

template <typename T>
class reference_array<T, 1> {
	
	T data;
	
	template <
		typename Arg0,
		typename... Args,
		typename Enable = typename std::enable_if<(sizeof...(Args) >= 1), void>::type
	>
	constexpr reference_array(Arg0 && arg0, Args &&...)
		: data(util::forward<Arg0>(arg0)) { }
	
public:
	
	typedef T value_type;
	static constexpr std::size_t static_size = 1;
	
	constexpr reference_array() = default;
	constexpr reference_array(const reference_array & o) = default;
	reference_array(reference_array && o) = default;
	
	template <typename U>
	constexpr reference_array(U && data) : data(util::forward<U>(data)) { }
	
	T & operator[](std::size_t index) {
		return (index == 0) ? data : undefined(data);
	}
	
	constexpr const T & operator[](std::size_t index) const {
		return (index == 0) ? data : undefined(data);
	}
	
	template <typename OT, std::size_t ON>
	friend class reference_array;
};

template <typename T>
class reference_array<T, 0> {
	static_assert(sizeof(dummy<T>) == 0, "reference_array cannot have size zero");
};

#endif // REFERENCE_ARRAY_HPP
