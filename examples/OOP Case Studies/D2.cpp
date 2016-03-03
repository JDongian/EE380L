

#include "Base.h"



class D2 : public Base {
public:
	virtual void doit(void) {
		std::cout << "D2\n";
	}
	static Base* makeD2(void) { return new D2;  }

	virtual D2* clone(void) { return new D2{ *this }; }
};

class _InitD2 {
public:
	_InitD2(void) {
		std::map<std::string, CreateFun>& the_real_map = getMap();
		the_real_map["D2"] = &D2::makeD2;
	}
};

static _InitD2 fred;