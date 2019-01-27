#include <iostream>
#include <functional>
#include <boost/type_index.hpp>
#include <iostream>
#include <functional>
#include <boost/type_index.hpp>
using namespace std;

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


template<typename T>
void show_type_and_size(T& x) {
    cout << "T = "
         << boost::typeindex::type_id_with_cvr<T>().pretty_name() << ";\n"
         << boost::typeindex::type_id_with_cvr<decltype(x)>().pretty_name() << " x;\n"
         << "sizeof(x) "  << sizeof(x) << "\n"
         << endl;
}

void
CreateLambda() {
    Foo foo;
    auto lambda1 =  [foo](){ cout << foo.i << endl;};

    show_type_and_size(foo);
    show_type_and_size(lambda1);

}
int main(int argc, char *argv[])
{
    CreateLambda();
    return 0;
}
