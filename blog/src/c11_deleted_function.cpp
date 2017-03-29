#include <iostream>
#include <memory>
using namespace std;

struct B {
    virtual ~B() = default;
    template<int x>
    void show() {
        cout << "x is " << x << endl;
    }
};

struct D : public B {

};

int main(int argc, char *argv[])
{
    B * p = new D();

    return 0;
}
