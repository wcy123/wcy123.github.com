#include <iostream>
#include <functional>
#include <boost/type_index.hpp>
using cnamespace std;
class Foo {
  public:
    Foo():i(100) {
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
ostream& operator<<(ostream& out, const Foo & obj)
{
    out << "foo(" << &obj << "," << obj.i << ")";
    return out;
}
void
CreateLambda() {
    Foo foo;
    {
      [foo]() { cout << foo.i << endl; }();
    }
}
int main(int argc, char *argv[])
{
    CreateLambda();
    return 0;
}
