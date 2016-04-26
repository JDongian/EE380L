#include <iostream>
#include <cstdint>
#include <string>

using std::cout;
using std::endl;

struct TRUE {
	static constexpr bool value = true;
};

struct FALSE {
	static constexpr bool value = false;
};

template <typename T>
constexpr auto eval(T) -> decltype(T::value) { return T::value; }

template <bool p> struct foo {
	static std::string msg(void) { return "true"; }
};

template <> struct foo<false> {
	static std::string msg(void) { return "false"; }
};

int main(void) {
	cout << foo<eval(FALSE{})>::msg() << endl;
}