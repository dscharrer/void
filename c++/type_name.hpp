
#include <string>
#include <cxxabi.h>

//! Print the full name of a type, including cv and reference part
template <typename T>
std::string type_name() {
	typedef typename std::remove_reference<T>::type target_type;
	std::string result;
	if(std::is_volatile<target_type>::value) {
		result += "volatile ";
	}
	if(std::is_const<target_type>::value) {
		result += "const ";
	}
	int status;
	char * name = abi::__cxa_demangle(typeid(T).name(), 0, 0, &status);
	result += name;
	free(name);
	if(std::is_rvalue_reference<T>::value) {
		result += " &&";
	}
	if(std::is_lvalue_reference<T>::value) {
		result += " &";
	}
	return result;
}
