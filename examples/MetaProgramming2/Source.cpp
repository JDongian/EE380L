#include <iostream>
#include <cstdint>

using std::cout;
using std::endl;

struct Blah {};
struct Blargh {};

template <typename T>
struct Wrap : public T {
	using T::T;
	Wrap(T const& arg) : T(arg) {}
	using magic = int;
};

template <> struct Wrap<Blah> : Blah {
	using Blah::Blah;
	Wrap(Blah const& arg) : Blah(arg) {}
};

template <typename T>
void foo(T const&) {
	cout << "general\n";
}

template <typename T>
void foo(Wrap<T> const&, typename Wrap<T>::magic v = 0) {
	cout << "specific to wrapped stuff\n";
}

void foo(int const&) {
	cout << "specific to int\n";
}

template <typename T>
Wrap<T> wrap(T const& x) {
	return Wrap<T>(x);
}

void case1(void) {
	foo(42);
	Blah b;
	Blargh argh;
	foo(b);
	foo(wrap(b));

	cout << "and again\n";
	foo(argh);
	foo(wrap(argh));
}




template <typename T>
void bracket(T const&, ...) {
	cout << "general\n";
}


template <typename T, typename X = decltype(std::declval<T>()[0])>
void bracket(T const& x) {
	cout << "specific to types with op[]\n";
}


void doit(int x) {

}

void doit(int x, ...) {

}

int main(void) {
	doit(42);

	//bracket(42);
	//int x[10];
	//bracket(x);
}