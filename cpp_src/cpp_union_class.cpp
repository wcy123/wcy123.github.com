#include <iostream>
using namespace std;

struct A {
    A() {
        cout << "A()" << endl;
    }
    ~A() {
        cout << "~A()" << endl;
    }
};
struct B {
    B() {
        cout << "B()" << endl;
    }
    ~B() {
        cout << "~B()" << endl;
    }
};
union X {
    A a;
    B b;
};
int main(int argc, char *argv[])
{

    X x;
    return 0;
}
