#include <iostream>
#include <cstdint>
#include <new>



#include "MemHeap.h"



using std::cout;
using std::endl;

class Foo_nonPOD { // a trivial, but non-POD type
public:
	Foo_nonPOD(void) { cout << "Foo default constructor\n"; }
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
	delete p;

	int* parray = new int[10]; // note, since int is POD, nothing special happens when we allocate an array
	delete[] p; 

	Foo_nonPOD* fp = new Foo_nonPOD;
	Foo_nonPOD* fparray = new Foo_nonPOD[10];
	delete fp;
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
	T& operator*(void) { if (!block) throw WTF{ "dereference of invalid SmartPointer" };  return *object; }

	SmartPointer<T>& operator=(T* object) {
		block = new ControlBlock{};
		this->object = object;
		block->ref_count = 1;
		return *this;
	}

	~SmartPointer(void) { destroy(); }
	
	SmartPointer(void) { block = nullptr; object = nullptr; }

	/* copy semantics */
	SmartPointer(SmartPointer<T> const& rhs) { copy(rhs); }
	SmartPointer& operator=(SmartPointer<T> const& rhs) {
		if (this != &rhs) {
			destroy();
			copy(rhs);
		}
		return *this;
	}

	/* move semantics */
	SmarPointer(SmartPointer<T> && rhs) { move(std::move(rhs)); }
	SmartPointer<T>& operator=(SmartPointer<T> && rhs) {
		destroy();
		move(rhs);
	}

private:
	void copy(SmartPointer<T> const& rhs) { // copy constructor, duplicate the pointer -- increment ref_count
		this->object = rhs.object;
		this->block = rhs.block;
		block->ref_count += 1;
	}

	void move(SmartPointer<T> && rhs) {
		block = rhs.block;
		object = rhs.object;
		rhs.block = nullptr;
	}

	void destroy(void) {
		if (!block) { return; } // no block, so exit without fanfare

		block->ref_count -= 1;
		if (block->ref_count == 0) {
			delete object;
			delete block;
		}

	}

};

int main(void) {
	SmartPointer<int> p;
	p = new int;
	*p = 42;
}