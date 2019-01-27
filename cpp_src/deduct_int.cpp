#include <iostream>
using namespace std;

template<int Nx>
struct A {
    static constexpr auto N = Nx;
    int a;
};
template<int Nx>
struct B: public A<Nx> {

};
template <typename A> struct AValue { //
  static constexpr auto x = A::N;
};

int main(int argc, char *argv[])
{
    B<100> x;
    std::cout <<  __FILE__ << ":" << __LINE__ << ": [" << __FUNCTION__<<"]"//
              << "AValue<decltype(x)>::x "  << AValue<decltype(x)>::x << " " //
              << std::endl;
    return 0;
}
