#include <iostream>
#include <cstdint>

#include "String.h"

using std::cout;
using std::endl;


void doit(String param) {
	cout << param[1];
}

int main(void) {
	String s1{ "Hello World" };

	s1.operator[](0) = 'J';
	s1[1] = 'a';

	doit(s1);



	for (uint32_t k = 0; k < s1.size(); k += 1) {
		cout << s1[k];
	}
	cout << endl;
}