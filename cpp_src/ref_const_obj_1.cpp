#include <iostream>
using namespace std;

class Foo
{
  public:
    Foo() {
        cerr <<  __FILE__ << ":" << __LINE__ << ": [" << __FUNCTION__<< "] "
             << "this "  << this << " "
             << endl;
    }
    ~Foo(){
        cerr <<  __FILE__ << ":" << __LINE__ << ": [" << __FUNCTION__<< "] "
             << "this "  << this << " "
             << endl;
    }
};

Foo f1() {
    return Foo();
}

Foo const& test() {
    Foo const& f = f1();
    cerr <<  __FILE__ << ":" << __LINE__ << ": [" << __FUNCTION__<< "] "
         << "&f "  << &f << " "
         << endl;
    return f;
}
int main(int argc, char *argv[])
{
    test();
    return 0;
}
