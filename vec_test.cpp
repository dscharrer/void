
#include <iostream>

#include "vec.hpp"

// ----------------------------------------------------------------------------------------------------------
// test
	
constexpr auto myvec = vec<float, 3>(0.f, 1.f, 2.f);

constexpr auto minus_myvec = -myvec;

constexpr auto myvec_length_stuff = myvec.length_sqr() + length_sqr(myvec) + distance_sqr(myvec, vec<float, 3>(0.f));

constexpr auto myvec_normalized_stuff = myvec.is_normalized() && is_normalized(myvec)
                                        && equal_eps(myvec, myvec) && closer_than(myvec, myvec, 1.f)
                                        && !farther_than(myvec, myvec, 0.f);

constexpr auto my_dot = dot(make_vec(1, 2), make_vec(3, 4));

void foo() {
	(void)myvec_length_stuff;
	(void)myvec_normalized_stuff;
	(void)my_dot;
}

constexpr float myx = myvec.x;
constexpr float my8 = myvec[2];

constexpr auto repeat_vec = vec<float, 20>(42.f);

char data[std::size_t(repeat_vec[10])];

constexpr auto unary_vec = vec<float, 1>(2.f);

constexpr auto my_reference_array = reference_array<float, 3>(1.f, 2.f, 3.f);

constexpr auto fuu = static_cast<const vec<float, 2> &>(vec<float, 2>(1.f)).to<int>();

constexpr auto swizzled = static_cast<const vec<float, 2> &>(vec<float, 2>(1.f)).yx;

constexpr auto shdb = make_vec(1, 2);
constexpr auto sdh = make_vec<float>(1, 2, 3);

constexpr auto my_cross_produc = cross(make_vec(1.f, 2.f, 3.f), make_vec(3.f, 4.f, 5.f));

static constexpr std::size_t f1 = 4;
static constexpr std::size_t f2 = 8;
static constexpr vec<std::size_t, 2> constexpr_vec(f1, f2);
static constexpr std::size_t constexpr_vec2 = constexpr_vec[0];
static constexpr auto constexpr_swizzled = constexpr_vec.swizzle<1>();
char array[constexpr_swizzled[0]];

void cast_test(const vec<float, 3> &) {
	
}

template <typename T>
void print(T & t) {
	std::cout << "       scalar " << t << "     ";
}

template <typename T>
void print(const T & t) {
	std::cout << " const scalar " << t << "     ";
}

template <typename T>
typename std::enable_if<std::is_same<typename std::decay<T>::type, T>::value, void>::type print(T && t) {
	std::cout << "rvalue scalar " << t << "     ";
}

template <typename T, std::size_t N>
void print(vec<T, N> & t) {
	std::cout << "          vec " << t;
}

template <typename T, std::size_t N>
void print(const vec<T, N> & t) {
	std::cout << "    const vec " << t;
}

template <typename T, std::size_t N>
void print(vec<T, N> && t) {
	std::cout << "   rvalue vec " << t;
}

template <typename T, std::size_t N>
void print(vec<T &, N> & t) {
	std::cout << "          ref " << t;
}

template <typename T, std::size_t N>
void print(const vec<T &, N> & t) {
	std::cout << "    const ref " << t;
}

template <typename T, std::size_t N>
void print(vec<T &, N> && t) {
	std::cout << "   rvalue ref " << t;
}

template <typename A, typename B>
void test(A && a, B && b) {
	std::cout << "concatenating ";
	print(util::forward<A>(a));
	std::cout << " and ";
	print(util::forward<B>(b));
	std::cout << " gives ";
	print(concatenate(util::forward<A>(a), util::forward<B>(b)));
	std::cout << '\n';
}

static_assert(is_vec< vec<float, 1> >::value, "bad vec 1");
static_assert(is_vec< vec<float, 2> >::value, "bad vec 2");
static_assert(is_vec< vec<float, 3> >::value, "bad vec 3");
static_assert(is_vec< vec<float, 4> >::value, "bad vec 4");
static_assert(is_vec< vec<float, 5> >::value, "bad vec 5");

static_assert(are_unique<0, 1, 2, 3, 4, 5, 6>::value, "unique 1");
static_assert(!are_unique<0, 1, 2, 3, 4, 5, 1>::value, "unique 1");

void my_func(const float *) { }

void set_vec(float * data, std::size_t n) {
	for(std::size_t i = 0; i < n; i++) {
		data[i] = i;
	}
}

int main() {
	
	{
		
		my_func(vec<float, 5>(1.f).data());
		
		std::cout << myvec << repeat_vec << unary_vec << '\n';
		
		float mine = 1.f;
		float & mine_ref = mine;
		
		vec<float &, 2> refed(mine_ref, mine_ref);
		
		float a = 0.f, b = 2.f;
		
		typedef float & float_ref;
		
		
		std::cout << ref_nth<2>(a, a, b) << '\n';
		
		reference_array<float_ref, 2> refs(a, b);
		
		refs[1] = refs[0];
		refs[0] = 42.f;
		
		std::cout << a << ", " << b << '\n';
		
		
		vec<int, 2> referenced_data(0, 0);
		vec<int &, 2> reference = referenced_data.to<int &>();
		vec<int, 2> vector(1, 0);
		const vec<int, 2> cvec(2, 0);
		vec<int, 2> referenced_data2(3, 0);
		const vec<int &, 2> creference = referenced_data2.to<int &>();
		
		int scalar = 4;
		const int cscalar = 5;
		
		vec<int, 2> rreference_data(6, 0);
		
		test(reference, reference);
		test(reference, creference);
		test(reference, rreference_data.to<int &>());
		test(reference, vector);
		test(reference, cvec);
		test(reference, make_vec(7, 0));
		test(reference, scalar);
		test(reference, cscalar);
		test(reference, 8);
		
		test(creference, reference);
		test(creference, creference);
		test(creference, rreference_data.to<int &>());
		test(creference, vector);
		test(creference, cvec);
		test(creference, make_vec(7, 0));
		test(creference, scalar);
		test(creference, cscalar);
		test(creference, 8);
		
		test(rreference_data.to<int &>(), reference);
		test(rreference_data.to<int &>(), creference);
		test(rreference_data.to<int &>(), rreference_data.to<int &>());
		test(rreference_data.to<int &>(), vector);
		test(rreference_data.to<int &>(), cvec);
		test(rreference_data.to<int &>(), scalar);
		test(rreference_data.to<int &>(), cscalar);
		
		test(vector, reference);
		test(vector, creference);
		test(vector, rreference_data.to<int &>());
		test(vector, vector);
		test(vector, cvec);
		test(vector, make_vec(7, 0));
		test(vector, scalar);
		test(vector, cscalar);
		test(vector, 8);
		
		test(cvec, reference);
		test(cvec, creference);
		test(cvec, rreference_data.to<int &>());
		test(cvec, vector);
		test(cvec, cvec);
		test(cvec, scalar);
		test(cvec, cscalar);
		test(cvec, 8);
		
		test(make_vec(7, 0), reference);
		test(make_vec(7, 0), creference);
		test(make_vec(7, 0), rreference_data.to<int &>());
		test(make_vec(7, 0), vector);
		test(make_vec(7, 0), cvec);
		test(make_vec(7, 0), make_vec(7, 0));
		test(make_vec(7, 0), scalar);
		test(make_vec(7, 0), cscalar);
		test(make_vec(7, 0), 8);
		
		test(scalar, reference);
		test(scalar, creference);
		test(scalar, rreference_data.to<int &>());
		test(scalar, vector);
		test(scalar, cvec);
		test(scalar, make_vec(7, 0));
		test(scalar, scalar);
		test(scalar, cscalar);
		test(scalar, 8);
		
		test(cscalar, reference);
		test(cscalar, creference);
		test(cscalar, rreference_data.to<int &>());
		test(cscalar, vector);
		test(cscalar, cvec);
		test(cscalar, make_vec(7, 0));
		test(cscalar, scalar);
		test(cscalar, cscalar);
		test(cscalar, 8);
		
		test(8, reference);
		test(8, creference);
		test(8, rreference_data.to<int &>());
		test(8, vector);
		test(8, cvec);
		test(8, make_vec(7, 0));
		test(8, scalar);
		test(8, cscalar);
		test(8, 8);
		
		concatenate(make_vec(0));
		
		std::cout << make_vec(0, 1) << '\n';
		print(concatenate(0, 1, 2));
		std::cout << '\n';
		
	}
	
	{
		
		const float hello = 1.f;
		
		vec<float, 1> myvec(hello);
		
		myvec += 1.f;
		
		const vec<float, 1> constvec(3.f);
		
		std::cout << myvec + 1 << ' ' << constvec + 1 << std::endl;
		
		std::cout << myvec << std::endl;
		
		vec<float, 6> avec(0.f, 1.f, 2.f, 3.f, 4.f, 5.f);
		
		cast_test(avec.swizzle<0, 1, 4>());
		
		std::cout << avec << (avec * 2.f) << (2.f * avec) << std::endl;
		
		avec.swizzle<2, 3>() = vec<float, 2>(6, 9);
		
		avec.swizzle<0, 5>().yx = avec.swizzle<4, 0>();
		
		std::cout << avec << ' ' << avec.xy.swizzle<1>() << std::endl;
		
		std::cout << vec<float, 42>(42) << std::endl;
		
		vec<float, 5> fortytwo(42);
		
		fortytwo.swizzle<0, 2, 4>() /= 2;
		
		vec<float, 6> add(0.f, 1.f, 0.f, 1.f, 0.f, 1.f);
		
		std::cout << fortytwo + add.swizzle<0, 1, 2, 3, 4>() << std::endl;
		
		fortytwo *= add.swizzle<1, 2, 3, 4, 5>();
		
		
		swizzle<X, Y, Z, W>(vec<int, 4>(1, 2, 3, 4) + vec<int, 4>(3, 2, 1, 0));
		
		
		std::cout << concatenate(make_vec(1, 2, 3), make_vec(3, 4, 5).yz) << '\n';
		std::cout << concatenate(make_vec(1, 2, 3), 4) << '\n';
		std::cout << concatenate(1, make_vec(2, 3, 4)) << '\n';
		std::cout << concatenate(make_vec(1, 2, 3), 4) << '\n';
		std::cout << concatenate(1, make_vec(2, 3, 4), 5) << '\n';
		std::cout << concatenate(1, 2) << '\n';
		
		std::cout << concatenate(1, 2, 3) << '\n';
		std::cout << concatenate(1, 2, 3, 4) << '\n';
		std::cout << concatenate(1, 2, 3, 4, 5) << '\n';
		
		int a = 0, b = 1;
		concatenate(a, b) = make_vec(2, 3);
		std::cout << a << ' ' << b << '\n';
		
		std::cout << concatenate(1, 2, make_vec(3), make_vec(4, 5), make_vec(6, 7, 8), 9) << '\n';
		
		
		std::cout << vec<float, 2>(vec<float, 1>(.5f), 1) << '\n';
		vec<float, 3>(vec<float, 2>(0), 1);
		vec<float, 3>(vec<float, 2>(0), vec<float, 1>(1));
		
		auto vec2z = make_vec(1.f, 2.f);
		
		std::cout << vec<float, 10>(0.f, vec2z.xy, 3.f, make_vec(4.f, 5.f, 6.f, 7.f), 8.f, 9.f) << '\n';
		
		vec<float, 7>(0, 1, 2, 3, 4, 5, 6);
		
		std::cout << vec2z.length() << ' ' << vec2z.length_sqr() << ' ' << length(vec2z) << ' ' << length_sqr(vec2z) << '\n';
		
		std::cout << (vec2z == vec2z.swizzle(0, 1)) << '\n';
		
		std::cout << vec2z.normalized() << ' ' << normalize(vec2z) << ' ' << distance(vec2z, vec<float, 2>(0)) << '\n';
		
		vec2z.normalize();
		
		std::cout << cos_angle(make_vec(1.f, 0.f), make_vec(0.f, 1.f)) << '\n';
		std::cout << angle(make_vec(1.f, 0.f), make_vec(0.f, 1.f)) << '\n';
		
		vec3 writable_vec;
		set_vec(writable_vec.zyx.data(), writable_vec.size());
		std::cout << writable_vec << '\n';
		
	}
	
	return 0;
}
