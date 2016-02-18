#include <iostream>
#include <stdint.h>
#include <typeinfo>
#include <memory>
#include <string>

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
	virtual 
		void doit(void) { cout << "Base\n"; }


};

class Base2 {
public:
	int y;
	virtual void doit(void) { cout << "Base2\n"; }
};

class Derived : public Base, public Base2 {
public:
	virtual void doit(void) { cout << "whoa?\n";  }
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
	Base2& b2 = d;
	b1.doit();
	b2.doit();

	whatAmI(&d);

	Base& bobj = d;
	Derived& dobj = static_cast<Derived&>(bobj);
//	Derived& dobj2 = dynamic_cast<Derived&>(bobj);

}


#ifdef notdef 
class VisibleString {
public:
	VisibleString(void) {
		cout << "string being made (default)\n";
	}
	
	VisibleString(const char*) {
		cout << "string being made (promotion from char*)\n";
	}

	~VisibleString(void) {
		cout << "string being destroyed\n";
	}
};

class Empty {
public:
	virtual void doit(void) { cout << "empty class\n"; }
};

class Simple : public Empty {
public:
//	std::string data;
	VisibleString data;
	Simple(void) : data{ "hello world\n" } {}
};

int main(void) {
    std::shared_ptr<Empty> ptr{ new Simple{}};
}

#endif 