#include <iostream>
#include <cstdint>
#include <string>

using std::cout;
using std::endl;

using std::declval;

struct TRUE {
	static constexpr bool value = true;
};

struct FALSE {
	static constexpr bool value = false;
};

template <typename T>
constexpr auto eval(T) -> decltype(T::value) { return T::value; }

template <typename T>
constexpr auto eval(void) -> decltype(T::value) { return T::value; }

template <bool p> struct foo {
	static std::string msg(void) { return "true"; }
};

template <> struct foo<false> {
	static std::string msg(void) { return "false"; }
};

template <typename T, typename test=decltype(declval<T>()[0])>
TRUE has_op_bracket(void) { return TRUE{}; }

template <typename T, typename... not_used>
FALSE has_op_bracket(not_used...) { return FALSE{}; }

template <typename T>
using HasBracket = decltype(has_op_bracket<T>());

template <typename T>
void check_bracket(void) {
	cout << std::boolalpha;
	cout << eval<HasBracket<T>>() << endl;
}

void test1(void) {
	check_bracket<int>();
	check_bracket<int*>();
	check_bracket<std::string>();
}





/* -------------------------------------------------------------------- */

template <typename... T>
struct Tuple;

template <typename T>
struct Tuple<T> {
	T val;
};

template <typename T, typename... OtherArgs>
struct Tuple<T, OtherArgs> : public Tuple<OtherArgs...> {
	T val;
};


int main(void) {
	Tuple<int> x;
	x.val = 42;
	Tuple<int, int> y;
}






