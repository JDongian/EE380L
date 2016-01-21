#include <iostream>
#include <type_traits>

using std::cout;
using std::endl;

class Foo {
	Foo(Foo &) {}
};

int main(void) {
	cout << "Hello World\n";

	bool val = std::is_pod<Foo>::value;
	cout << std::boolalpha;
	cout << val << endl;
}