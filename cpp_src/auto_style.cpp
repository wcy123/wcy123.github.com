#include <iostream>
#include <functional>
#include <boost/type_index.hpp>
using namespace std;
class Foo {
  public:
    Foo(int v):i(v) {
        cout <<  __FILE__ << ":" << __LINE__ << ": [" << __PRETTY_FUNCTION__<< "] "
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
             << endl;
        i = -100;
    }
  public:
    int i = 0;
    friend ostream& operator<<(ostream& out, const Foo & obj);
};
int main(int argc, char *argv[])
{
    const auto& f1 = Foo{1};
    Foo f2{1};
    return 0;
}
