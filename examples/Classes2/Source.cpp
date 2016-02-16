#include <iostream>
#include <stdint.h>
#include <typeinfo>

using std::cout;
using std::endl;

class Foo {};

void doit(Foo const & x) {
	cout << "lvalue fun\n";
}

void doit(Foo  &&  x) {
	cout << "rvalue fun\n";
}


void an_asside_rvalue_refs(void) {
	Foo f;

	using F = Foo const&&;
	using T = std::remove_reference<F>::type;
	if (std::is_const<T>::value) { cout << "it's a constant\n";  }
	else { cout << "not a constant\n";  }
	doit(static_cast<T&&>(f));


	Foo const&& x = (Foo const&&)f;
	doit((Foo&& )x);
}

class Shape {
public:
	void draw(void) { cout << "generic shape\n"; }

};

class Circle : public Shape {
public:
	int circularity; // stuff that makes a circle a circle (and not generic)
	void draw(void) { cout << "circle\n"; }
};

class Square : public Shape {
public:
	int squareness; // stuff that makes a square a square 
	void draw(void) { cout << "square\n"; }
};

void shapefun(Shape s) {
	s.draw();
}

int main(void) {
//	an_asside_rvalue_refs();

	Shape s;
	Circle c;
	Square r;

	s.draw();
	c.draw();
	r.draw();

	shapefun(s);
	shapefun(c);
	shapefun(r);
}

