#include <iostream>
#include <type_traits>

using std::cout;
using std::endl;

class Complex {
public:
	double real;
	double imag;

	Complex(Complex const &) = default;
	Complex(void) = default;
	~Complex(void) = default;
	Complex& operator=(Complex const &) = default;
};


int main(void) {
	cout << "Hello World\n";

	bool val = std::is_pod<Complex>::value;
	cout << std::boolalpha;
	cout << val << endl;

	Complex val1{ 1.0, 0.0 };
}