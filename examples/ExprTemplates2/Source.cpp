#include <iostream>
#include <cstdint>
#include <string>

using std::cout;
using std::endl;

class SmartString : public std::string {
public:
	using std::string::string;

	void print(std::ostream& out) const {
		out << (std::string const&) (*this);
	}
};

template <typename T> struct choose_ref {
	using type = T;
};

template<> struct choose_ref<SmartString> {
	using type = SmartString const&;
};

template <typename T> using ChooseRef = typename choose_ref<T>::type;

template <typename S1Type, typename S2Type>
class ConcatString {
	using LeftType = ChooseRef<S1Type>;
	using RightType = ChooseRef<S2Type>;
	LeftType s1;
	RightType s2;
public:
	ConcatString(S1Type const& left, S2Type const& right) :
		s1{ left }, s2(right) {}

	uint64_t size(void) const { return s1.size() + s2.size(); }

	char operator[](uint64_t k) const {
		if (k < s1.size()) { return s1[k]; }
		else { return s2[k - s1.size()]; }
	}

	void print(std::ostream& out) const {
		out << s1;
		out << s2;
	}
};



ConcatString<SmartString,SmartString>
operator+(SmartString const& lhs, SmartString const& rhs) {
	return ConcatString<SmartString, SmartString>{ lhs, rhs };
}

template <typename LHSType, typename RHSType>
ConcatString<ConcatString<LHSType, RHSType>, SmartString>
operator+(ConcatString<LHSType, RHSType> const& lhs, SmartString const& rhs) {
	return ConcatString<ConcatString<LHSType, RHSType>, SmartString>{ lhs, rhs };
}

template <typename LHSType, typename RHSType>
ConcatString<SmartString, ConcatString<LHSType, RHSType>>
operator+(SmartString const& lhs, ConcatString<LHSType, RHSType> const& rhs) {
	return ConcatString<SmartString, ConcatString<LHSType, RHSType>>{ lhs, rhs };
}

template <typename LHS1Type, typename RHS1Type, typename LHS2Type, typename RHS2Type>
ConcatString<ConcatString<LHS1Type, RHS1Type>, ConcatString<LHS2Type, RHS2Type>>
operator+(ConcatString<LHS1Type, RHS1Type> const& lhs, 
	ConcatString<LHS2Type, RHS2Type> const& rhs) 
{
	return ConcatString<ConcatString<LHS1Type, RHS1Type>, 
		ConcatString<LHS2Type, RHS2Type>>{ lhs, rhs };
}


template <typename T>
std::ostream& operator<<(std::ostream& out, T const& str) {
	str.print(out);
	return out;
}


/* --- client code here --- */
using string = SmartString;

int main(void) {
	string first = "Craig";
	string last = "Chase";
	string greeting = "Hello ";
	cout << greeting + first + last << endl;
	auto t1 = greeting + first;
	auto t2 = t1 + last;
	cout << t2;

}