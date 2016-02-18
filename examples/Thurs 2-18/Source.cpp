#include <iostream>
#include <cstdint>

using std::cout;
using std::endl;

class Foo {
public:
	~Foo(void) {
		cout << "cleaned up\n";
	}
};


class Base {
public:
	virtual ~Base(void) = default; 

	virtual void draw(void) = 0;
};

class Derived : public Base {
public:
	Foo resource;
	virtual void draw(void) override {}
};


class SFunObject {
public:
	int operator()(double x, double y) {
		if (x < y) { return -1;  }
		if (y < x) { return 1; }
		return 0;
	}
};

template <typename SomeFun>
void qsort(double x[], int n, SomeFun) {
	if (SomeFun{}(x[0], x[1]) < 0) {
		cout << "whoopie!\n";
	}
}

void myApp(void) {
	SFunObject fun;
	double x[1000];
	int n = 1000;

	qsort(x, n, fun);
}

using PFun = void(*)(void); // pointer to function type (from C)

class LifeForm {
public:
};

using PMemFun = void (LifeForm::*)(void); // pointer to LifeForm member fun

void doThing(void) {
	cout << "I did it\n";
}

class FunObj {
	PFun pfun;
	PMemFun pmfun;
	LifeForm& obj;
public:
	//void operator()(void) {
	//	(*pfun)();
	//}

	void operator()(void) {
		(obj.*pmfun)();
	}

};



int main(void) {
}



