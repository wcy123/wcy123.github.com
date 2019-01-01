#include <iostream>
#include <boost/type_index.hpp>
using namespace std;

#define S(expr) do{ cerr << #expr << ":"  ; expr; } while(0)

template<typename T>
void foo1(T x) {
    cerr << "T = "
         << boost::typeindex::type_id_with_cvr<T>().pretty_name() << ";"
         << boost::typeindex::type_id_with_cvr<decltype(x)>().pretty_name() << " x;"
         << endl;
}

template<typename T>
void foo2(T& x) {
    cerr << "T = "
         << boost::typeindex::type_id_with_cvr<T>().pretty_name() << ";"
         << boost::typeindex::type_id_with_cvr<decltype(x)>().pretty_name() << " x;"
         << endl;
}

template<typename T>
void foo3(const T& x) {
    cerr << "T = "
         << boost::typeindex::type_id_with_cvr<T>().pretty_name() << ";"
         << boost::typeindex::type_id_with_cvr<decltype(x)>().pretty_name() << " x;"
         << endl;
}

class Widget {
  public:
    Widget() {}
};

int main(int argc, char *argv[])
{
    Widget w1;
    Widget& rw = w1;
    const Widget cw;
    const Widget& crw1 = w1;
    const Widget& crw2 = cw;
    Widget w2 = cw;
    // Widget& w3 = cw;

    S(foo1(w1));
    S(foo2(w1));
    S(foo3(w1));

    cerr << endl;

    S(foo1(rw));
    S(foo2(rw));
    S(foo3(rw));

    cerr << endl;

    S(foo1(cw));
    S(foo2(cw));
    S(foo3(cw));

    cerr << endl;

    S(foo1(crw1));
    S(foo2(crw1));
    S(foo3(crw1));


    return 0;
}
