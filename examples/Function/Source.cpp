#include <iostream>
#include <cstdint>
#include <vector>

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
	void doit(void) {
		cout << "I did it (whatever \"it\" is)\n";
	}
};


void doThing(void) {
	cout << "I did it\n";
}

class FunObjBase {

public:
	virtual void operator()(void) = 0;
	uint32_t refs = 0;
	virtual ~FunObjBase(void) = default;
};

class BasicFunObj : public FunObjBase {
	PFun pfun;
public:
	BasicFunObj(PFun p) : pfun(p) {}

	void operator()(void) {
		(*pfun)();
	}

};

template <typename T>
using PMemFun = void (T::*)(void); // pointer to LifeForm member fun



template <typename T>
class MemFunFunObj : public FunObjBase {
	PMemFun<T> pmfun;
	T& obj;
public:
	MemFunFunObj(T& o, PMemFun<T> f) : obj(o) {
		pmfun = f;
	}
	void operator()(void) {
		(obj.*pmfun)();
	}
};

class Fun {
	FunObjBase* ptr;
public:
	void operator()(void) {
		(*ptr)();
	}

	Fun(Fun const & rhs) {
		ptr = rhs.ptr;
		ptr->refs += 1;
	}
	~Fun(void) {
		ptr->refs -= 1;
		if (ptr->refs == 0) { delete ptr; }
	}

	Fun(PFun f) {
		ptr = new BasicFunObj{ f };
		ptr->refs = 1;
	}

	template <typename T>
	Fun(T& obj, PMemFun<T> f) {
		ptr = new MemFunFunObj<T>{ obj, f };
		ptr->refs = 1;
	}
};

void doit(void) {
	cout << "the global doit\n";
}

void simpleFunObj(void) {
	std::vector<Fun> queue;


	LifeForm bug;
	Fun f1{ bug, &LifeForm::doit };
	queue.push_back(f1);

	Fun f2{ &::doit };
	queue.push_back(f2);

	while (!queue.empty()) {
		auto& fun = queue.back();
		fun();
		queue.pop_back();
	}
}



template <typename Fun>
void doSomething(Fun f) {
	f();
}

class Foo2 {
public:
	int x = 42;
	void doit(void) {
		cout << "x is " << x << endl;
	}
};

class Silly {
	Foo2& myFoo;
public:
	Silly(Foo2& f) : myFoo(f) {}

	void operator()(void) {
		myFoo.doit();
		cout << "Hello World\n";
	}
};


int main(void) {
	auto blah = [] (void) { cout << "Hello World\n"; };
	doSomething(blah);

	Foo2 f;
	auto blah2 = [& f](void) { f.doit();  cout << "Hello World\n"; };
	doSomething(blah2);
}

