#include <iostream>
#include <cstdint>
#include <functional>

template <typename F>
void doit(F f) {
	decltype(f()) x = f();
}

int main(void) {
	doit([](void) { return 42; });
}