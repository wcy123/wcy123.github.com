#include <iostream>
#include <functional>
#include <memory>
using namespace std;


struct Foo {

    Foo() {
        i = 0;
        cerr <<  __FILE__ << ":" << __LINE__ << ": [" << __FUNCTION__<< "] "
             << endl;
    }
    Foo(const Foo& other){
        i = other.i;
        cerr <<  __FILE__ << ":" << __LINE__ << ": [" << __FUNCTION__<< "] "
             << endl;
    }
    ~Foo() {
        i = -100;
        cerr <<  __FILE__ << ":" << __LINE__ << ": [" << __FUNCTION__<< "] "
             << endl;
    }
    int i ;
};
using op = std::function<void(int)>;

op  CreateOp() {
    shared_ptr<Foo> f = make_shared<Foo>();
    f->i = 100;
    return [f] (int x) {
        cerr <<  __FILE__ << ":" << __LINE__ << ": [" << __FUNCTION__<< "] "
             << "x "  << x << " "
             << "f.i "  << f->i<< " "
             << endl;

    };
}
struct B {
    B() {
        action = CreateOp();x
    }
    void do_it () {
        action(10);
    }
    op action;
};
int main(int argc, char *argv[])
{
    {
        B * b = new B();
        b->do_it();
        cerr <<  __FILE__ << ":" << __LINE__ << ": [" << __FUNCTION__<< "] "
             << endl;
    }
    cerr <<  __FILE__ << ":" << __LINE__ << ": [" << __FUNCTION__<< "] "
         << endl;
    return 0;
}
