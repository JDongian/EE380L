#include <iostream>
#include <cstdint>

using std::cout;
using std::endl;

class String {
	char* data;
	uint32_t length;
public:
	String(void) {
		data = nullptr;
		length = 0;
	}

	String(char const * src) {
		length = 0;
		while (src[length] != 0) {
			length += 1;
		}
		if (length > 0) {
			data = new char[length];
			for (int k = 0; k < length; k += 1) {
				data[k] = src[k];
			}
		}
		else {
			data = nullptr;
		}
	}

	~String(void) { delete[] data; }

	uint32_t size(void) const { return length; }

	char& operator[](uint32_t k) { return data[k]; }
	char operator[](uint32_t k) const { return data[k]; }

	String& operator+=(String const& rhs) {
		if (rhs.size() == 0) { return *this; } // special case for appending empty

		char const* old_data = data;
		uint32_t old_length = length;

		length += rhs.length;
		data = new char[length];
		for (uint32_t k = 0; k < old_length; k += 1) {
			data[k] = old_data[k];
		}

		for (uint32_t k = 0; k < rhs.length; k += 1) {
			data[k + old_length] = rhs.data[k];
		}

		if (old_data) { delete[] old_data; }
	}
};


int main(void) {
	String s1{ "Hello World" };

	for (uint32_t k = 0; k < s1.size(); k += 1) {
		cout << s1[k];
	}
	cout << endl;
}