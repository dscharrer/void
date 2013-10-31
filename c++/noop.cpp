
/*
 * C++11 replacement for MSVC's __noop
 *
 * Consider you have a macro PRINT_DEBUG(...) that should print the arguments in
 * debug builds but should not add *any* overhead in release builds. Or to be precise
 * the arguments should not be evaluated in release builds.
 *
 * A simple implementation would be:
 *  #ifdef DEBUG
 *  #define PRINT_DEBUG(...) printf(__VA_ARGS__)
 *  #else
 *  #define PRINT_DEBUG(...) // discard arguments
 *  #endif
 *
 * This fulfills the requirement, but now the compiler doesn't see the arguments at
 * all during release mode. If any variables or functions were only used such debug
 * statements, the compiler may now warn that they aren't used in release builds.
 * IDEs may not parse the arguments either and could fail to adjust them when renaming
 * symbols.
 *
 * MSVC provides a solution to this via the __noop intrinsic. detail tells the compiler
 * to parse the arguments but not generate any code:
 *  #define PRINT_DEBUG(...) __noop(__VA_ARGS__) // discard arguments
 *
 * However GCC doesn't implement anything equivalent. Not a big problem: we can implement
 * our own in C++11 using a macro that never gets called:
 *  #define PRINT_DEBUG(...) noop(__VA_ARGS__) // discard arguments
 *
 * The following implementation has been tested with GCC and Clang.
 */

namespace detail {
	inline void consume() { }
	template <typename T0, typename... T>
	void consume(const T0 & t0, const T &... t) { (void)t0; consume(t...); }
}

#define noop(...) \
 (void)([&](){detail::consume(__VA_ARGS__);})

// test code:

static int a = 0;

static int d() { return 0; }

void test(int b) {
	
	int c = 0;
	
	noop(a + b, c, d());
	
}

int main() {
	test(0);
	return 0;
}
