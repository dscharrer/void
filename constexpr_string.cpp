
#include <iostream>
#include <type_traits>
#include <typeinfo>

template <typename Type, Type... values>
struct meta_array;

template <typename Type, Type start, Type end, bool valid = (end > start)>
struct meta_sequence {
	typedef typename meta_sequence<Type, start, end - 1>::type::template append<end - 1>::type type;
};

template <typename Type, Type start, Type end>
struct meta_sequence<Type, start, end, false> {
	typedef meta_array<Type> type;
};


template <size_t N>
constexpr size_t cstrlen(const char (&arg)[N], size_t i = 0) {
	return arg[i] ? cstrlen(arg, i + 1) + 1 : 0;
}


template <size_t N, typename Type = char>
struct array;

template <typename array, size_t N = array::size, size_t I = 0>
struct array_helper {
	
	typedef array_helper<array, N, (I + 1)> next;
	
	typedef ::array<array::size - N, typename array::value_type> substr_type;
	
	static constexpr array set(const typename array::value_type (&str)[array::size], size_t i, typename array::value_type c) {
		return (I == i) ?
			array(str, typename meta_sequence<size_t, 0, I>::type(), c,
			      str, typename meta_sequence<size_t, I + 1, array::size>::type())
			: next::set(str, i, c);
	}
	
	static constexpr substr_type substr(const typename array::value_type (&str)[array::size], size_t i) {
		return (I == i) ?
			substr_type(str, typename meta_sequence<size_t, I, I + substr_type::size>::type())
			: next::substr(str, i);
	}
	
};

template <typename array, size_t N>
struct array_helper<array, N, N> {
	
	typedef ::array<array::size - N, typename array::value_type> substr_type;
	
	static constexpr array set(const typename array::value_type (&str)[array::size], size_t, typename array::value_type) {
		return array(str, typename array::indices());
	}
	
	static constexpr substr_type substr(const typename array::value_type (&str)[array::size], size_t) {
		return substr_type(str, meta_array<size_t>());
	}
	
};

template <size_t N, typename Type>
struct array {
	
	typedef typename meta_sequence<size_t, 0, N>::type indices;
	
	static constexpr size_t size = N;
	
	typedef Type value_type;
	typedef Type data_type[(N == 0) ? 1 : N];
	
	data_type data;
	
	constexpr array() : data{} { }
	
	template <Type... bytes>
	constexpr array(meta_array<Type, bytes...>) : data{ bytes... } { }
	
	template <size_t M, size_t... I>
	constexpr array(const Type (&str)[M], meta_array<size_t, I...>) : data{ str[I]... } { }
	template <size_t M, size_t... I>
	constexpr array(const Type (&str)[M], meta_array<size_t, I...>, Type c) : data{ str[I]..., c } { }
	template <size_t M, size_t... I>
	constexpr array(Type c, const Type (&str)[M], meta_array<size_t, I...>) : data{ c, str[I]... } { }
	template <size_t M1, size_t... I1, size_t M2, size_t... I2>
	constexpr array(const Type (&str1)[M1], meta_array<size_t, I1...>, const Type (&str2)[M2], meta_array<size_t, I2...>) : data{ str1[I1]..., str2[I2]... } { }
	template <size_t M1, size_t... I1, size_t M2, size_t... I2>
	constexpr array(const Type (&str1)[M1], meta_array<size_t, I1...>, Type c, const Type (&str2)[M2], meta_array<size_t, I2...>) : data{ str1[I1]..., c, str2[I2]... } { }
	
	template <size_t M>
	static constexpr array create(const Type (&str)[M]) { return array(str, indices()); }
	
	constexpr Type operator[](size_t i) { return data[i]; }
	
	constexpr array<N + 1, Type> operator+(Type c) {
		return array<N + 1, Type>(data, indices(), c);
	}
	
	template <size_t M>
	constexpr array<N + M - 1, Type> operator+(const Type (&str)[M]) {
		return array<N + M - 1,Type>(data, indices(), str, typename array<M - 1>::indices());
	}
	
	template <size_t M>
	constexpr array<N + M, Type> operator+(const array<M, Type> & o) {
		return array<N + M, Type>(data, indices(), o.data, typename array<M>::indices());
	}
	
	template <size_t start, size_t end = size>
	constexpr array<end - start, Type> substr() {
		return array<end - start, Type>(data, typename meta_sequence<size_t, start, end>::type());
	}
	
	template <size_t length>
	constexpr array<length, Type> tail() {
		return substr<size - length, size>();
	}
	
	template <size_t length>
	constexpr array<length, Type> substr(size_t start) {
		return array_helper<array, size - length>::substr(data, start);
	}
	
	template <size_t i>
	constexpr array set(Type c) {
		return array(data, typename meta_sequence<size_t, 0, i>::type(), c,
		             data, typename meta_sequence<size_t, i + 1, size>::type());
	}
	
	constexpr array set(size_t i, Type c) {
		return array_helper<array>::set(data, i, c);
	}
	
	constexpr operator const data_type &() {
		return data;
	}
	
	constexpr array swap(size_t i, size_t j) {
		return array(data, indices()).set(i, data[j]).set(j, data[i]);
	}
	
};



template <typename Type, size_t N>
constexpr array<N - 1, Type> cstr(const Type (&str)[N]) {
	return array<N - 1, Type>::create(str);
}

template <typename Type, size_t N>
constexpr array<N + 1, Type> operator+(Type c, const array<N, Type> & a) {
	return array<N + 1, Type>(c, a.data, typename array<N>::indices());
}

template <typename Type, size_t N, size_t M>
constexpr array<N - 1 + M, Type> operator+(const Type (&str)[N], const array<M, Type> & a) {
	return array<N - 1 + M, Type>(str, typename array<N - 1>::indices(), a.data, typename array<M>::indices());
}

// TODO use boost::mpl::list instead?
template <typename Type, Type... values>
struct meta_array {
	
	typedef Type value_type;
	
	static constexpr size_t length = sizeof...(values);
	
	constexpr meta_array() = default;
	
	template <Type c>
	struct append {
		typedef meta_array<Type, values..., c> type;
	};
	
	template <Type c>
	struct prepend {
		typedef meta_array<Type, c, values...> type;
	};
	
	static constexpr array<length, Type> data() {
		return array<length, Type>(meta_array());
	}
	
};

template <typename Type, size_t N>
constexpr array<N, Type> reverse(const array<N, Type> & input, size_t i = 0) {
	return (i < N / 2) ?
		reverse(input.swap(i, N - i - 1), i + 1)
		: input;
}


template <size_t I, size_t End, typename Operator, typename Input, typename Accumulator> 
struct iterate_helper {
	
	static constexpr auto apply(const Input & input, const Accumulator & acc)
		-> decltype(iterate_helper<I + 1, End, Operator, Input, decltype(Operator::apply(input, acc, I))>
		   ::apply(input, Operator::apply(input, acc, I))) {
		return iterate_helper<I + 1, End, Operator, Input, decltype(Operator::apply(input, acc, I))>
		       ::apply(input, Operator::apply(input, acc, I));
	}
	
};

template <size_t End, typename Operator, typename Input, typename Accumulator> 
struct iterate_helper<End, End, Operator, Input, Accumulator> {
	static constexpr Accumulator apply(const Input &, const Accumulator & acc) { return acc; }
};

template <typename Operator, typename Input, size_t N = Input::size>
constexpr auto iterate(const Input & input, const decltype(Operator::init()) & init = Operator::init())
 -> decltype(iterate_helper<0, N, Operator, Input, decltype(Operator::init())>::apply(input, init)) {
	return iterate_helper<0, N, Operator, Input, decltype(Operator::init())>::apply(input, init);
}



struct hexifier {
	
	static constexpr array<0> init() {
		return array<0>();
	}
	
	static constexpr char hexdigit(unsigned char hexvalue) {
		return (hexvalue >= 10) ? (hexvalue - 10 + 'a') : (hexvalue + '0');
	}
	
	template <size_t N, size_t M>
	static constexpr array<M + 6> apply(const array<N> & input, const array<M> & acc, size_t i) {
		return acc + ", 0x" + hexdigit(input[i] >> 4) + hexdigit(input[i] & 0xf);
	}
	
};

constexpr auto hexified = iterate<hexifier>(cstr("Test B_\0")).substr<2>();




template <size_t N, size_t total = N>
struct number_helper {
	static constexpr size_t count = number_helper<N / 10, N>::count + 1;
	static constexpr array<count> encode() {
		return number_helper<N / 10, N>::encode() + ('0' + (N % 10));
	}
};

template <size_t total>
struct number_helper<0, total> {
	static constexpr size_t count = 0;
	static constexpr array<count> encode() {
		return array<0>();
	}
};

template<>
struct number_helper<0, 0> {
	static constexpr size_t count = 1;
	static constexpr array<count> encode() {
		return cstr("0");
	}
};

template <size_t N>
constexpr array<number_helper<N>::count> encode_number() {
	return number_helper<N>::encode();
}

constexpr auto numbers = encode_number<42>() + " + " + encode_number<0>() + " + " + encode_number<32478>() + '\0';




template <class T>
struct strip_type {
	typedef T type;
};
template <class T>
struct strip_type<T &> {
	typedef typename strip_type<T>::type type;
};
template <class T>
struct strip_type<const T> {
	typedef typename strip_type<T>::type type;
};

template <class Data, size_t i = sizeof(Data::data) - 1>
struct array_to_meta_array {
	
	static constexpr char entry = Data::data[i - 1];
	
	typedef typename array_to_meta_array<Data, i - 1>::type::template append<entry>::type type;
};
template <class Data>
struct array_to_meta_array<Data, 0> {
	typedef meta_array<typename strip_type<decltype(*Data::data)>::type> type;
};


struct DataHolder {
	static constexpr char data[] = "ABCDEFG...";
};
typedef array_to_meta_array<DataHolder>::type::append<'!'>::type my_meta_array;

struct DataHolder2 {
	static constexpr char data[] = (my_meta_array::data() + " OK?" + '\0').data;
};
typedef array_to_meta_array<DataHolder2>::type mixing_arrays;




template <int N>
struct foo {
  static void print() { std::cout << N << std::endl; }
};

constexpr auto helloword = ">|" + cstr("Hello").set(1, '3') + " " + cstr("World!").set<1>('0');

constexpr auto reversed = reverse(helloword);

constexpr char constr[helloword.size] = (helloword.substr<1, helloword.size - 1>() + "?" + '\0').data;

constexpr char mydata[] = (helloword + '\0').data;

int main() {
	
	std::cout << typeid(constr).name() << ' ' << helloword[helloword.size - 1] << std::endl;
	
	std::cout << constr << std::endl;
	
	std::cout << meta_array<char, 'W', 'T', 'F', '.', '\0'>::data() << std::endl;
	
	
	std::cout << helloword.tail<6>() + '\0' << std::endl;
	
	std::cout << helloword.substr<5>(2) + '\0' << std::endl;
	
	std::cout << reversed + '\0' << std::endl;
	
	std::cout << hexified + '\0' << std::endl;
	
	std::cout << numbers + '\0' << std::endl;
	
	std::cout << my_meta_array::data() + '\0' << std::endl;
	
	std::cout << mixing_arrays::data() + '\0' << std::endl;
	
	std::cout << mydata << std::endl;
	
	foo<cstrlen("foobar")>::print();
	
	return 0;
}
