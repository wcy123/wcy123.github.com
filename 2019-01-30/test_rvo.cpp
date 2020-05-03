#include <iostream>
using namespace std;
int c = 0;

class Foo {
 public:
  explicit Foo();
  Foo(const Foo& other);
  ~Foo();
  int value;
};

ostream& operator<<(ostream& out, const Foo& v) {
  out << "Foo[" << v.value << "@" << (void*)&v << "]";
  return out;
}

Foo::Foo() : value(c++) { cout << "construct: " << *this << endl; }
Foo::Foo(const Foo& other) : value(other.value) {
  cout << "copy from " << other << "to " << *this << endl;
}
Foo::~Foo() { cout << "deconstructor: " << *this << endl; }

Foo build() {
  int mark = 0;
  cout << "&mark " << (void*)&mark << endl  //
      ;
  return Foo();
}

int main(int argc, char* argv[]) {
  cout << "begin block" << endl;
  {
    int begin = 0;
    auto obj = build();
    int end = 0;
    cout << "&begin " << (void*)&begin << endl  //
         << "obj = " << obj << endl             //
         << "&end " << (void*)&end << endl      //
        ;
  }
  cout << "end block" << endl;
  return 0;
}
