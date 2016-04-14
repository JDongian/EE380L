#include <iostream>
#include <cstdint>
#include <string>

using std::cout;
using std::endl;

class SmartString : public std::string {
public:
	using std::string::string;
};

/* implements the const String concept
 * -- has a size() method and an op[] returning char */
class StringConcatProxy {
	std::string s1;
	std::string s2;
public:
	StringConcatProxy(std::string const& pref, std::string const& suff) :
		s1(pref), s2(suff) {}

	uint64_t size(void) const {
		return s1.size() + s2.size();
	}

	char const& operator[](uint64_t k) const {
		if (k < s1.size()) { return s1[k]; }
		else { return s1[k - s1.size()]; }
	}

	void print(std::ostream& out) const {
		out << s1;
		out << s2;
	}
};

std::ostream& operator<<(std::ostream& out, StringConcatProxy const& str) {
	str.print(out);
	return out;
}

StringConcatProxy operator+(SmartString const& lhs, SmartString const& rhs) {
	return StringConcatProxy(lhs, rhs);
}

using string = SmartString;

int main(void) {
	string first = "Craig";
	string last = "Chase";
	cout << first + last << endl;
}