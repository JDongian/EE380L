#include "Base.h"

class _InitD1 {
public:
	_InitD1(void);
};

class D1 : public Base {
public:
	virtual void doit(void) {
		std::cout << "D1\n";
	}

	static Base* makeD1(void) {
		return new D1;
	}

	virtual Base* clone(void) {
		return new D1{ *this };
	}

	static _InitD1 fred;
};

_InitD1 D1::fred;

_InitD1::_InitD1(void) {
	std::map<std::string, CreateFun>& the_real_map = getMap();
	the_real_map["D1"] = &D1::makeD1;
}

