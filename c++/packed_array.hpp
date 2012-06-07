
#ifndef PACKED_ARRAY_HPP
#define PACKED_ARRAY_HPP

#include <cstddef>
#include <type_traits>

/*! Like std::array, but:
 * - constexpr
 * - convertible to a pointer to the data
 */
template <typename T, std::size_t N>
class packed_array {
	
	T data[N];
	
public:
	
	template <typename... Args, typename = typename std::enable_if<sizeof...(Args) == N, void>::type>
	constexpr packed_array(const Args &... args) : data{ args... } { }
	
	operator T *() {
		return data;
	}
	
	constexpr operator const T *() const {
		return data;
	}
	
	T & operator[](std::size_t index) {
		return data[index];
	}
	
	constexpr const T & operator[](std::size_t index) const {
		return data[index];
	}
	
};

#endif // PACKED_ARRAY_HPP
