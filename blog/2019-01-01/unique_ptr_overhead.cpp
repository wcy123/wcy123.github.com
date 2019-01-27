#include <memory>
using namespace std;

struct Foo{
    Foo();
    ~Foo();
};

void UniquePtr() {
    auto p = make_unique<Foo>();
}

void RawPtr() {
    auto p = new Foo();
    delete p;
}
