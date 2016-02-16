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

void shapes(void) {

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

class Base {
public:
	int x;
	//virtual 
		void doit(void) { cout << "Base\n"; }

	//virtual 
		void doit2(void) { cout << "Base doit2\n"; }

	virtual 
		~Base(void) {}
};

class Base2 {
public:
	int y;
	virtual void doit(void) { cout << "Base2\n"; }
};

class Derived : public Base, Base2 {
public:
	virtual void fun(void) { cout << "Derived\n"; }
};


void whatAmI(Base* obj) {
	cout << "my type is : " << typeid(*obj).name() << endl;
}

int main(void) {
	cout << "Base is " << sizeof(Base) << " bytes\n";

	Base b;
	b.doit();

	Derived d;
	Base& b1 = d;
	Base& b2 = d;
	b1.doit();
	b2.doit();

	whatAmI(&d);
}

