
#include "bitset_converter.hpp"

#include <iostream>

typedef bitset_converter<>
	::add::map<0, 0>
	::add::map<1, 1>
	::add::map<2, 2>
	::add::map<3, 3>
	::add::map<4, 4>
	::add::map<5, 5>
	::add::map<6, 6>
	::add::map<7, 7>
	::add::map<8, 8>
	::add::map<9, 9>
	::add::map<10, 10>
	::add::map<11, 11>
	::add::map<12, 12>
	::add::map<13, 13>
	::add::map<14, 14>
	::add::map<15, 15>
	::add::map<16, 16>
	::add::map<17, 17>
	::add::map<18, 18>
	::add::map<19, 19>
	::add::map<20, 20>
	identity_converter;

static void test2(size_t i) {
	
	size_t out = identity_converter::convert(i);
	
	std::cout << i << " = " << std::bitset<64>(i) << " -> " << out << " = " << std::bitset<64>(out) << std::endl;
	
}

volatile size_t i;

int main() {
	
	test2(0);
	test2(1);
	test2(2);
	test2(3);
	test2(4);
	test2(5);
	test2(6);
	test2(7);
	test2(8);
	
	
	return identity_converter::convert(i);
}

