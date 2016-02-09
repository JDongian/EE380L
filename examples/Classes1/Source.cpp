#include <iostream>
#include <cstdint>
#include <vector>

using std::cout;
using std::endl;
using std::vector;

template <typename T>
class checkedVector : public vector<T> {
public:
	using vector<T>::vector;
	T& operator[](int k) {
		if (k < 0 || k >= this->size()) {
			cout << "WTF, dude?\n";
		}
		return vector<T>::operator[](k);
	}
};


void vectorInherit(void) {
	checkedVector<int> x(10);
	x.push_back(42);
	x.pop_back();
	cout << x[10] << endl;
}

class Base1 {
public:
	int x;
	int y;
};

class Base2 {
public:
	int z;
};

class Derived : public Base1, public Base2 {
public:
//	int z;

};

void inheritOffset(void) {
	int off_x = offsetof(Base1, x);
	int off_y = offsetof(Base1, y);
	cout << "Inside a Base1 we have x at offset " << off_x;
	cout << " and y at offset " << off_y;
	cout << endl;

	int off_z = offsetof(Base2, z);
	cout << "Inside a Base2 we have z at offset " << off_z;
	cout << endl;

	off_x = offsetof(Derived, x);
	off_y = offsetof(Derived, y);
	off_z = offsetof(Derived, z);
	cout << "Inside a Base1 we have x at offset " << off_x;
	cout << " and y at offset " << off_y;
	cout << " and z at offset " << off_z;
	cout << endl;
}

int main(void) {
	vectorInherit();
	inheritOffset();
}