#include <iostream>
#include <stdint.h>
#include <typeinfo>

using std::cout;
using std::endl;

template <typename T>
void doit(T const & x) {
	cout << "lvalue fun\n";
}

template <typename T>
void doit(T && x) {
	cout << "rvalue fun\n";
}

class Foo {};

int main(void) {
	Foo f;
	Foo const&& y = (Foo const&&) f;
	doit(y);

}

