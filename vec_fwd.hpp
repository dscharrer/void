
#ifndef VEC_FWD_HPP
#define VEC_FWD_HPP

#include <cstddef>

template <typename T, std::size_t N>
class vec;

typedef vec<bool, 1> bvec1;
typedef vec<bool, 2> bvec2;
typedef vec<bool, 3> bvec3;
typedef vec<bool, 4> bvec4;

typedef vec<int, 1> ivec1;
typedef vec<int, 2> ivec2;
typedef vec<int, 3> ivec3;
typedef vec<int, 4> ivec4;

typedef vec<unsigned, 1> uvec1;
typedef vec<unsigned, 2> uvec2;
typedef vec<unsigned, 3> uvec3;
typedef vec<unsigned, 4> uvec4;

typedef vec<float, 1> vec1;
typedef vec<float, 2> vec2;
typedef vec<float, 3> vec3;
typedef vec<float, 4> vec4;

typedef vec<double, 1> dvec1;
typedef vec<double, 2> dvec2;
typedef vec<double, 3> dvec3;
typedef vec<double, 4> dvec4;

#endif // VEC_FWD_HPP
