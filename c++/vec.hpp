
#ifndef VEC_HPP
#define VEC_HPP

#include <cmath>
#include <limits>
#include <type_traits>

#include "util.hpp"
#include "packed_array.hpp"
#include "vec_fwd.hpp"
#include "vec_base.hpp"
#include "vec_traits.hpp"
#include "swizzle.hpp"

// TODO add vector iterators?
// TODO rgba and stpq swizzlers?

// Free functions used by the vector

namespace detail {
	template <std::size_t N, typename T, std::size_t... Indices>
	constexpr vec<T, N> repeat(const T & repeated, indices<Indices...>) {
		return { (&repeated)[Indices - Indices]... };
	}
}

//! Create an N-ary vector where all components have the same value.
template <std::size_t N, typename T>
constexpr vec<T, N> repeat(const T & repeated) {
	return detail::repeat<N>(repeated, typename index_range<0, N>::type());
}

namespace detail {
	template <typename T, typename Vec0, std::size_t... Indices0, typename Vec1, std::size_t... Indices1>
	constexpr vec<T, sizeof...(Indices0) + sizeof...(Indices1)> concatenate(Vec0 && v0, Vec1 && v1,
		                                                                      indices<Indices0...>, indices<Indices1...>) {
		return { v0[Indices0]..., v1[Indices1]... }; // TODO forward?
	}
}

//! concatenate two vectors
template <
	typename T = na,
	typename Vec0,
	typename Vec1,
	typename Result = typename concatenated_type<T, Vec0 &&, Vec1 &&>::type
>
constexpr typename std::enable_if<
	is_vec<Vec0>::value && is_vec<Vec1>::value,
	Result
>::type concatenate(Vec0 && v0, Vec1 && v1) {
	return detail::concatenate<typename Result::value_type>(util::forward<Vec0>(v0), util::forward<Vec1>(v1),
		typename vec_traits<Vec0>::component_indices(),
		typename vec_traits<Vec1>::component_indices());
}

namespace detail {
	template <typename T, typename Vec, std::size_t... Indices, typename Scalar>
	constexpr vec<T, sizeof...(Indices) + 1> concatenate(Vec && v, indices<Indices...>, Scalar && s) {
		return { v[Indices]..., util::forward<Scalar>(s) }; // TODO forward v?
	}
}

//! concatenate a vector and a scalar
template <
	typename T = na,
	typename Vec,
	typename Scalar,
	typename Result = typename concatenated_type<T, Vec &&, Scalar &&>::type
>
constexpr typename std::enable_if<
	is_vec<Vec>::value && !is_vec<Scalar>::value,
	Result
>::type concatenate(Vec && v, Scalar && s) {
	return detail::concatenate<typename Result::value_type>(util::forward<Vec>(v),
		typename vec_traits<Vec>::component_indices(), util::forward<Scalar>(s));
}

namespace detail {
	template <typename T, typename Scalar, typename Vec, std::size_t... Indices>
	constexpr vec<T, 1 + sizeof...(Indices)> concatenate(Scalar && s, Vec && v, indices<Indices...>) {
		return { util::forward<Scalar>(s), v[Indices]... }; // TODO forward v?
	}
}

//! concatenate a scalar and a vector
template <
	typename T = na,
	typename Scalar,
	typename Vec,
	typename Result = typename concatenated_type<T, Scalar &&, Vec &&>::type
>
constexpr typename std::enable_if<
	is_vec<Vec>::value && !is_vec<Scalar>::value,
	Result
>::type concatenate(Scalar && s, Vec && v) {
	return detail::concatenate<typename Result::value_type>(util::forward<Scalar>(s), util::forward<Vec>(v),
		typename vec_traits<Vec>::component_indices());
}

//! concatenate two scalars
template <
	typename T = na,
	typename Scalar0,
	typename Scalar1,
	typename Result = typename concatenated_type<T, Scalar0 &&, Scalar1 &&>::type
>
constexpr typename std::enable_if<
	!is_vec<Scalar0>::value && !is_vec<Scalar1>::value,
	Result
>::type concatenate(Scalar0 && s0, Scalar1 && s1) {
	return { util::forward<Scalar0>(s0), util::forward<Scalar1>(s1) };
}

//! "concatenate" a single value
template <typename T = na, typename Arg, typename Result = typename concatenated_type<T, Arg &&>::type>
constexpr Result concatenate(Arg && v) {
	return util::forward<Arg>(v);
}

//! Concatenate any number of vectors and/or scalars.
template <typename T = na, typename T0, typename T1, typename... Tail>
constexpr typename concatenated_type<T, T0 &&, T1 &&, Tail &&...>::type concatenate(T0 && t0, T1 && t1, Tail &&... tail) {
	return concatenate<T>(concatenate<T>(util::forward<T0>(t0), util::forward<T1>(t1)), util::forward<Tail>(tail)...);
}

template <typename Arg>
constexpr Arg sum(const Arg & arg) {
	return arg;
}

template <typename Arg0, typename... Args>
constexpr Arg0 sum(const Arg0 & arg0, const Args &... args) {
	return arg0 + sum(args...);
}


// Array that writes changes back to the vector

template <typename T, std::size_t N>
class writeback_array : public packed_array<typename std::remove_reference<T>::type, N> {
	
	typedef packed_array<typename std::remove_reference<T>::type, N> base_type;
	typedef vec<T, N> vec_type;
	
	vec_type * vector;
	
	template <std::size_t... Indices>
	static constexpr base_type make_array(vec_type & v, indices<Indices...>) {
		return { v[Indices]... };
	}
	
public:
	
	writeback_array(vec_type & vector)
		: base_type(make_array(vector, typename vec_traits<vec_type>::component_indices())), vector(&vector) { }
	
	~writeback_array() {
		for(std::size_t i = 0; i < vector->size(); i++) {
			(*vector)[i] = base_type::operator[](i);
		}
	}
	
};


// Actual vec implementation

//! The ultimate vector class.
template <typename T, std::size_t N>
class vec : public vec_base<T, N> {
	
	typedef vec_base<T, N> base_type;
	
public:
	
	typedef T value_type;
	static constexpr std::size_t static_size = N;
	
	typedef typename std::remove_reference<value_type>::type target_type;
	typedef target_type & reference_type;
	
	typedef vec<target_type, static_size> target_vec;
	typedef vec<reference_type, static_size> reference_vec;
	
	static constexpr bool is_ref = std::is_same<value_type, reference_type>::value;
	
	typedef typename index_range<0, static_size>::type component_indices;
	
	// Constructors
	
	//! Default 
	constexpr vec() = default;
	constexpr vec(const vec & other) = default;
	vec(vec && other) = default;
	
	//! Component constructor.
	template <typename... Args, typename = typename std::enable_if<sizeof...(Args) == N, void>::type>
	constexpr vec(Args &&... args) : base_type(util::forward<Args>(args)...) { }
	
	//! Repeat constructor.
	template <typename Dummy = void, typename = typename std::enable_if<(static_size > 1), Dummy>::type>
	explicit constexpr vec(const T & repeated)
		: base_type(repeat<static_size, value_type>(repeated)) { }
	
	//! Concatenating constructor.
	template <typename Arg0, typename... Args, typename = typename std::enable_if<
		   (sizeof...(Args) > 0 && sizeof...(Args) + 1 < static_size),
	void>::type>
	constexpr vec(Arg0 && arg0, Args &&... args)
		: base_type(concatenate<value_type>(util::forward<Arg0>(arg0), util::forward<Args>(args)...)) { }
	
	// Accessors
	
	value_type & operator[](std::size_t index) {
		return base_type::operator[](index);
	}
	
	constexpr const value_type & operator[](std::size_t index) const {
		return base_type::operator[](index);
	}
	
	constexpr std::size_t size() { return static_size; }
	
private:
	
	template <std::size_t... Indices>
	constexpr const packed_array<target_type, static_size> data(indices<Indices...>) const {
		return { operator[](Indices)... };
	}
	
public:
	
	constexpr const packed_array<target_type, static_size> data() const {
		return data(component_indices());
	}
	
	writeback_array<value_type, static_size> data() {
		return { *this };
	}
	
	// Type conversion
	
private:
	
	template <typename OT, std::size_t... Indices>
	vec<OT, static_size> to(indices<Indices...>) {
		return { static_cast<OT>(operator[](Indices))... };
	}
	
	template <typename OT, std::size_t... Indices>
	constexpr vec<OT, static_size> to(indices<Indices...>) const {
		return { static_cast<OT>(operator[](Indices))... };
	}
	
public:
	
	template <typename OT>
	vec<OT, static_size> to() { return to<OT>(component_indices()); }
	
	template <typename OT>
	constexpr vec<OT, static_size> to() const { return to<OT>(component_indices()); }
	
	constexpr operator const target_vec() const { return to<target_type>(); }
	
	operator reference_vec() { return to<reference_type>(); }
	
	// Swizzling
	
	//! Unchecked dynamic swizzle.
	template <typename... Indices, typename = typename std::enable_if<sizeof...(Indices) != 0, void>::type>
	constexpr const vec<value_type, sizeof...(Indices)> swizzle(Indices... indices) const {
		return { operator[](indices)... };
	}
	
	//! Unchecked dynamic swizzle.
	template <typename... Indices, typename = typename std::enable_if<sizeof...(Indices) != 0, void>::type>
	vec<reference_type, sizeof...(Indices)> swizzle(Indices... indices) {
		return { operator[](indices)... };
	}
	
	//! Checked static swizzle.
	template <std::size_t... Indices, typename = typename std::enable_if<sizeof...(Indices) != 0, void>::type>
	constexpr const vec<value_type, sizeof...(Indices)> swizzle() const {
		static_assert(maximum<Indices...>::value < static_size, "swizzle index out of range");
		return swizzle(Indices...);
	}
	
	//! Checked static swizzle (unique indices).
	template <std::size_t... Indices>
	typename std::enable_if<
		sizeof...(Indices) != 0 && are_unique<Indices...>::value,
		vec<reference_type, sizeof...(Indices)>
	>::type swizzle() {
		static_assert(maximum<Indices...>::value < static_size, "swizzle index out of range");
		return swizzle(Indices...);
	}
	
	//! Checked static swizzle (repeated indices).
	template <std::size_t... Indices>
	typename std::enable_if<
		sizeof...(Indices) != 0 && !are_unique<Indices...>::value,
		const vec<target_type, sizeof...(Indices)>
	>::type swizzle() {
		static_assert(maximum<Indices...>::value < static_size, "swizzle index out of range");
		return swizzle(Indices...);
	}
	
	// Operators
	
private:
	
	void assign(const target_vec &, indices<>) const { };
	
	template <std::size_t Index0, std::size_t... Indices>
	void assign(const target_vec & o, indices<Index0, Indices...>) {
		operator[](Index0) = o[Index0];
		assign(o, indices<Indices...>());
	}
	
public:
	
	vec & operator=(const target_vec & o) {
		return is_ref ? assign(target_vec(o), component_indices()) : assign(o, component_indices()), *this;
	}
	
	vec & operator=(const reference_vec & o) {
		return assign(target_vec(o), component_indices()), *this;
	}
	
private:
	
	template <typename Vec>
	constexpr bool compare(const Vec &, indices<>) const {
		return true;
	}
	
	template <typename Vec, std::size_t Index0, std::size_t... Indices>
	constexpr bool compare(const Vec & o, indices<Index0, Indices...>) const {
		return (operator[](Index0) == o[Index0]) && compare(o, indices<Indices...>());
	}
	
public:
	
	constexpr bool operator==(const target_vec & o) const {
		return compare(o, component_indices());
	}
	
	constexpr bool operator==(const reference_vec & o) const {
		return compare(o, component_indices());
	}
	
	constexpr bool operator!=(const target_vec & o) const {
		return !compare(o, component_indices());
	}
	
	constexpr bool operator!=(const reference_vec & o) const {
		return !compare(o, component_indices());
	}
	
private:
	
	template <std::size_t... Indices>
	constexpr target_vec negate(indices<Indices...>) const {
		return { -operator[](Indices)... };
	}
	
public:
	
	constexpr target_vec operator-() const {
		return negate(component_indices());
	}
	
#define VEC_ASSIGN_OPERATOR_SCALAR(Op) \
	vec & operator Op##=(const target_type & right) { \
		return *this = (*this Op right); \
	}
	
#define VEC_ASSIGN_OPERATOR_VECTOR(Op) \
	vec & operator Op##=(const target_vec & right) { \
		return *this = (*this Op right); \
	} \
	template <typename Dummy = void, typename = typename std::enable_if<is_ref, Dummy>::type> \
	const vec & operator Op##=(const target_vec & right) const { \
		return *this = (*this Op right); \
	}
	
#define VEC1_ASSIGN_OPERATOR_SCALAR(Op) \
	template <typename Dummy = void, typename = typename std::enable_if<(static_size == 1), Dummy>::type> \
	vec & operator Op##=(const target_type & right) { \
		return *this = vec(*this Op right); \
	}
	
	VEC_ASSIGN_OPERATOR_SCALAR(*)
	VEC_ASSIGN_OPERATOR_SCALAR(/)
	
	VEC_ASSIGN_OPERATOR_VECTOR(*)
	VEC_ASSIGN_OPERATOR_VECTOR(/)
	VEC_ASSIGN_OPERATOR_VECTOR(+)
	VEC_ASSIGN_OPERATOR_VECTOR(-)
	
	VEC1_ASSIGN_OPERATOR_SCALAR(+)
	VEC1_ASSIGN_OPERATOR_SCALAR(-)
	VEC1_ASSIGN_OPERATOR_SCALAR(%)
	
#undef VEC_ASSIGN_OPERATOR_SCALAR
#undef VEC_ASSIGN_OPERATOR_VECTOR
#undef VEC1_ASSIGN_OPERATOR_SCALAR
	
	// Common vector functions
	
	void normalize() {
		*this /= length();
	}
	
	constexpr target_vec normalized() const {
		return *this / length();
	}
	
	constexpr bool is_normalized() const {
		return length_sqr() == target_type(1);
	}
	
private:
	
	template <std::size_t... Indices>
	constexpr target_type length_sqr(indices<Indices...>) {
		return sum(operator[](Indices) * operator[](Indices)...);
	}
	
public:
	
	constexpr target_type length_sqr() const {
		return length_sqr(component_indices());
	}
	
	constexpr target_type length() const {
		return std::sqrt(length_sqr()); // TODO sqrt is not constexpr
	}
	
};


// Operators

#define VEC_OPEARTOR_SCALAR(Name, Op) \
	namespace detail { \
		template <typename Vec, std::size_t... Indices> \
		constexpr typename vec_traits<Vec>::target_vec \
			Name(const Vec & left, const typename vec_traits<Vec>::target_type & right, \
			indices<Indices...>) { \
			return { (left[Indices] Op right)... }; \
		} \
		template <typename Vec, std::size_t... Indices> \
		constexpr typename vec_traits<Vec>::target_vec \
			Name(const typename vec_traits<Vec>::target_type & left, const Vec & right, \
			indices<Indices...>) { \
			return { (left Op right[Indices])... }; \
		} \
	} \
	template <typename Vec> \
	constexpr typename std::enable_if<is_vec<Vec>::value, typename vec_traits<Vec>::target_vec>::type \
	operator Op(const Vec & left, const typename vec_traits<Vec>::target_type & right) { \
		return detail::Name(left, right, typename vec_traits<Vec>::component_indices()); \
	} \
	template <typename Vec> \
	constexpr typename std::enable_if<is_vec<Vec>::value, typename vec_traits<Vec>::target_vec>::type \
	operator Op(const typename vec_traits<Vec>::target_type & left, const Vec & right) { \
		return detail::Name(left, right, typename vec_traits<Vec>::component_indices()); \
	}

#define VEC_OPEARTOR_VECTOR(Name, Op) \
	namespace detail { \
		template <typename Vec0, typename Vec1, std::size_t... Indices> \
		constexpr typename vec_traits<Vec0>::target_vec Name(const Vec0 & left, const Vec1 & right, indices<Indices...>) { \
			return { (left[Indices] Op right[Indices])... }; \
		} \
	} \
	template <typename Vec0, typename Vec1> \
	constexpr typename std::enable_if<compatible_vecs<Vec0, Vec1>::value, typename vec_traits<Vec0>::target_vec>::type \
	operator Op(const Vec0 & left, const Vec1 & right) { \
		return detail::Name(left, right, typename vec_traits<Vec0>::component_indices()); \
	}

VEC_OPEARTOR_SCALAR(multiply, *)
VEC_OPEARTOR_SCALAR(divide, /)

VEC_OPEARTOR_VECTOR(multiply, *)
VEC_OPEARTOR_VECTOR(divide, /)
VEC_OPEARTOR_VECTOR(add, +)
VEC_OPEARTOR_VECTOR(subtract, -)

#undef VEC_OPEARTOR_SCALAR
#undef VEC_OPEARTOR_VECTOR


// Stream operator

template <typename Stream, typename T, std::size_t N>
typename std::enable_if<
	   lshift_operator_exists<Stream, const typename std::decay<T>::type>::value
	&& lshift_operator_exists<Stream, const char>::value
	&& lshift_operator_exists<Stream, const char *>::value,
	Stream &
>::type operator<<(Stream & os, const vec<T, N> & vec) {
	os << '[';
	for(std::size_t i = 0; i < vec.static_size; i++) {
		if(i) {
			os << ", ";
		}
		os << vec[i];
	}
	return os << ']';
}


// Free functions

//! Convert a vector from one base type to another.
template <typename OT, typename IT, std::size_t N>
constexpr vec<OT, N> vector_cast(const vec<IT, N> & v) {
	return v.template to<OT>();
}

//! Helper function to construct vectors without explicitely specifying type and size.
template <typename Arg0, typename... Args, typename = typename std::enable_if<
	are_same<typename std::decay<Arg0>::type, typename std::decay<Args>::type...>::value,
	void
>::type>
constexpr vec<typename std::decay<Arg0>::type, sizeof...(Args) + 1> make_vec(Arg0 && arg0, Args &&... args) {
	return { util::forward<Arg0>(arg0), util::forward<Args>(args)... };
}

//! Helper function to construct vectors without explicitely specifying and size.
template <typename T, typename Arg0, typename... Args>
constexpr vec<T, sizeof...(Args) + 1> make_vec(Arg0 && arg0, Args &&... args) {
	return { util::forward<Arg0>(arg0), util::forward<Args>(args)... };
}

enum swizzle_index {
	X = 0,
	Y = 1,
	Z = 2,
	W = 3
};

template <
	std::size_t... Indices,
	typename Vec,
	typename = typename std::enable_if<is_vec<Vec>::value, void>::type
>
constexpr auto swizzle(Vec && v) -> AUTO_RETURN(v.template swizzle<Indices...>())

template <
	typename... Indices,
	typename Vec,
	typename = typename std::enable_if<is_vec<Vec>::value, void>::type
>
constexpr auto swizzle(Vec && v, Indices... indices) -> AUTO_RETURN(v.swizzle(indices...))

template <typename Vec, typename = typename std::enable_if<is_vec<Vec>::value, void>::type>
constexpr auto length_sqr(const Vec & v) -> AUTO_RETURN(v.length_sqr())

template <typename Vec, typename = typename std::enable_if<is_vec<Vec>::value, void>::type>
constexpr auto length(const Vec & v) -> AUTO_RETURN(v.length())

template <typename Vec, typename = typename std::enable_if<is_vec<Vec>::value, void>::type>
constexpr auto normalize(const Vec & v) -> AUTO_RETURN(v.normalized())

template <typename Vec, typename = typename std::enable_if<is_vec<Vec>::value, void>::type>
constexpr auto is_normalized(const Vec & v) -> AUTO_RETURN(v.is_normalized())

template <
	typename Vec0,
	typename Vec1,
	typename = typename std::enable_if<compatible_vecs<Vec0, Vec1>::value, void>::type
>
constexpr auto distance_sqr(const Vec0 & v0, const Vec1 & v1) -> AUTO_RETURN(length_sqr(v1 - v0))

template <
	typename Vec0,
	typename Vec1,
	typename = typename std::enable_if<compatible_vecs<Vec0, Vec1>::value, void>::type
>
constexpr auto distance(const Vec0 & v0, const Vec1 & v1) -> AUTO_RETURN(length(v1 - v0))

template <
	typename Vec0,
	typename Vec1,
	typename = typename std::enable_if<compatible_vecs<Vec0, Vec1>::value, void>::type
>
constexpr bool equal_eps(const Vec0 & v0, const Vec1 & v1, const
	typename vec_traits<Vec0>::target_type & eps = std::numeric_limits<typename vec_traits<Vec0>::target_type>::epsilon()) {
	return distance_sqr(v0, v1) < (eps * eps);
}

template <
	typename Vec0,
	typename Vec1,
	typename = typename std::enable_if<compatible_vecs<Vec0, Vec1>::value, void>::type
>
constexpr bool closer_than(const Vec0 & v0, const Vec1 & v1, const typename vec_traits<Vec0>::target_type & threshold) {
	return distance_sqr(v0, v1) < (threshold * threshold);
}

template <
	typename Vec0,
	typename Vec1,
	typename = typename std::enable_if<compatible_vecs<Vec0, Vec1>::value, void>::type
>
constexpr bool farther_than(const Vec0 & v0, const Vec1 & v1, const typename vec_traits<Vec0>::target_type & threshold) {
	return distance_sqr(v0, v1) > (threshold * threshold);
}

namespace detail {
	template <typename Vec0, typename Vec1, std::size_t... Indices>
	constexpr typename vec_traits<Vec0>::target_type dot(const Vec0 & v0, const Vec1 & v1, indices<Indices...>) {
		return sum(v0[Indices] * v1[Indices]...);
	}
}

template <
	typename Vec0,
	typename Vec1,
	typename = typename std::enable_if<compatible_vecs<Vec0, Vec1>::value, void>::type
>
constexpr typename vec_traits<Vec0>::target_type dot(const Vec0 & v0, const Vec1 & v1) {
	return detail::dot(v0, v1, typename vec_traits<Vec0>::component_indices());
}

template <
	typename Vec0,
	typename Vec1,
	typename = typename std::enable_if<compatible_vecs<Vec0, Vec1>::value, void>::type
>
constexpr typename vec_traits<Vec0>::target_type cos_angle(const Vec0 & v0, const Vec1 & v1) {
	return dot(v0, v1) / std::sqrt(length_sqr(v0) * length_sqr(v1));
}

template <
	typename Vec0,
	typename Vec1,
	typename = typename std::enable_if<compatible_vecs<Vec0, Vec1>::value, void>::type
>
constexpr auto angle(const Vec0 & v0, const Vec1 & v1) -> AUTO_RETURN(std::acos(cos_angle(v0, v1)))

template <
	typename Vec0,
	typename Vec1,
	typename = typename std::enable_if<
		compatible_vecs<Vec0, Vec1>::value
		&& vec_traits<Vec0>::static_size == 3,
		void
	>::type
>
constexpr typename vec_traits<Vec0>::target_vec cross(const Vec0 & a, const Vec1 & b) {
	return { a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x };
}

#endif // VEC_HPP
