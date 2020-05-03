// life_expansion.cpp
#include <iostream>
#include <functional>
#include <boost/type_index.hpp>
using namespace std;
class Foo {
  public:
    Foo(int v):i(v) {
        cout <<  __FILE__ << ":" << __LINE__ << ": [" << __PRETTY_FUNCTION__<< "] "
             << "this "  << this << " "
             << "i "  << i << " "
             << endl;
    }
    Foo(const Foo& foo):i(foo.i + 1) {
        cout <<  __FILE__ << ":" << __LINE__ << ": [" << __PRETTY_FUNCTION__<< "] "
             << "i "  << i << " "
             << endl;
    }
    ~Foo() {
        cout <<  __FILE__ << ":" << __LINE__ << ": [" << __PRETTY_FUNCTION__<< "] "
             << "i "  << i << " "
             << "this "  << this << " "
             << endl;
        i = -100;
    }
  public:
    int i = 0;
};

Foo calc()
{
    Foo foo(1);
    return foo;
}
void show(const Foo& f){
    printf("%d\nq", f.i);
}
int main(int argc, char *argv[])
{
    printf("before\n");
    {
        const auto& f1 = calc();
        show(f1);
    }
    printf("after\n");
    return 0;
}
