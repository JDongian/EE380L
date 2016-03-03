#include <iostream>
#include <cstdint>
#include <string>
#include <map>
#include <functional>
#include "Base.h"
using std::cout;
using std::endl;
using std::string;
using std::map;

std::map<std::string, CreateFun>& getMap(void) {
	static std::map<std::string, CreateFun> mymap;
	return mymap;
}


Base* factory(string type) {
	CreateFun fun = getMap()[type];
	return fun();
}

class Wrap {
	Base* ptr;
public:
	Wrap(Base* p) { ptr = p; }
	~Wrap(void) { delete ptr; }
	void doit(void) { ptr->doit(); }

	Wrap(Wrap const& rhs) {
		ptr = rhs.ptr->clone();
	}
};

void doit(Wrap w) {
	w.doit();
}

int main(void) {
	Wrap w{ factory("D1") };
	doit(w);
	w.doit();

	/* the last case study is objects that appear to change their type*/
#ifdef notdef
	Square x; // remove a corner from a square, it become a triangle
	x.remove_corner(); // but, triangle might not "fit" inside square
	// and the virtual function table point is only set in the constructor


	w.morph(); // but, if we use the wrapper, the wrapper can delegate to
	// the object, the square object can construct a new Triangle object
	// and the wrapper can manage the memory -- deleting the original square
	// and saving a pointer to the new triangle

	// i.e, the wrapper can appear to change type, even though objects cannot

#endif
}


