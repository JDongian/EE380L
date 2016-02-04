#include <iostream>
#include <cstdint>
#include <new>
#include <memory>

#include "MemHeap.h"



using std::cout;
using std::endl;

class Foo_nonPOD { // a trivial, but non-POD type
public:
	Foo_nonPOD(void) {cout << "Foo default constructor\n";}
	~Foo_nonPOD(void) { cout << "Foo destructor\n"; }
};

/* this function is behaving very strangely in VS2015 when compiled/run in debug mode
 * I think the compiler is doing non-standard tricks with new/delete in an effort to help
 * programmers find bugs. Consequently, the override for new/delete doesn't behave 
 * in a sane way.
 * If you're seeing stuff you don't understand, switch to "Release" mode and then
 * run it and see if there's a difference */
void heapDemo1(void) {
	int* p = new int;
	cout << "the pointer is at " << p << endl;
	delete p;

	int* parray = new int[10]; // note, since int is POD, nothing special happens when we allocate an array
	cout << "the pointer is at " << parray << endl;
	delete[] parray;


	Foo_nonPOD* fp = new Foo_nonPOD;
	cout << "the pointer is at " << fp << endl;
	Foo_nonPOD* fparray = new Foo_nonPOD[10];
	delete fp;
	cout << "the pointer is at " << fparray << endl;
	delete[] fparray;
}

struct WTF {
	char const* msg;
	char const* what(void) const { return msg; }
};

template <typename T>
class SmartPointer {
private:
	struct ControlBlock {
		uint64_t ref_count = 0;
	};

private:
	ControlBlock* block;
	T* object;
public:
	T& operator*(void) const {
		if (!block) { throw WTF{ "dereference of uninitialized Smart Pointer" }; }
		return *object;
	}

	SmartPointer(void) { block = nullptr; object = nullptr; }

	SmartPointer<T>& operator=(T* object) {
		block = new ControlBlock{};
		this->object = object;
		block->ref_count = 1;
		return *this;
	}

	~SmartPointer(void) { destroy(); }

	/* copy semantics */
	SmartPointer(SmartPointer<T> const& rhs) { copy(rhs); }
	SmartPointer<T>& operator=(SmartPointer<T> const& rhs) {
		if (this != &rhs) {
			destroy();
			copy(rhs);
		}
		return *this;
	}

	/* move semantics */
	SmartPointer(SmartPointer<T> && rhs) { move(std::move(rhs)); }
	SmartPointer<T>& operator=(SmartPointer<T> && rhs) {
		destroy();
		move(std::move(rhs));
		return *this;
	}

private:
	void copy(SmartPointer<T> const& rhs) {
		object = rhs.object;
		block = rhs.block;
		block->ref_count += 1;
	}

	void move(SmartPointer<T> && rhs) {
		object = rhs.object;
		block = rhs.block;
		rhs.block = nullptr;
	}

	void destroy(void) {
		if (!block) { return; }
		cout << "destroying with ref_count " << block->ref_count << endl;

		block->ref_count -= 1;
		if (block->ref_count == 0) {
			delete object;
			delete block;
		}
	}
};

SmartPointer<Foo_nonPOD> doit(void) {
	SmartPointer<Foo_nonPOD> p;
	p = new Foo_nonPOD{};
	return p;
}

int main(void) {
	std::shared_ptr<Foo_nonPOD> p;
	p = std::make_shared<Foo_nonPOD>();
}