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
struct Tuple<T, OtherArgs...> : public Tuple<OtherArgs...> {
	T val;
};


void test2(void) {
	Tuple<int> x;
	x.val = 42;
	Tuple<int, std::string> y;
	y.val = 0;
	Tuple<std::string>& y_tail = y;
	y_tail.val = "Hello World";
}

template <int index, typename tuple>
struct get_component;

template <typename... tuple_args>
struct get_component<0, Tuple<tuple_args...>> {
	static auto extract(Tuple<tuple_args...>& tuple) -> decltype(tuple.val)& {
		return tuple.val;
	}
};

template <int index, typename FirstArg, typename... tuple_args>
struct get_component<index, Tuple<FirstArg, tuple_args...>> {
	static auto extract(Tuple<FirstArg,tuple_args...>& tuple) 
		-> decltype(get_component < index - 1, Tuple<tuple_args...>(tuple))& {
		return get_component < index - 1, Tuple<tuple_args...>(tuple);
	}
};

int main(void) {
	using T = Tuple<int, std::string>;
	T x;
	get_component<0, T>(x) = 42;
	get_component<1, T>(x) = "Hello World";



}






