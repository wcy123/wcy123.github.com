#include <iostream>
#include <functional>
#include <boost/type_index.hpp>
using namespace std;

template<typename T>
void show_type_and_size(T& x) {
    cout << "T = "
         << boost::typeindex::type_id_with_cvr<T>().pretty_name() << ";\n"
         << boost::typeindex::type_id_with_cvr<decltype(x)>().pretty_name() << " x;\n"
         << "sizeof(x) "  << sizeof(x) << "\n"
         << endl;
}

int main(int argc, char *argv[])
{
    auto x = []() { cout << "hello world 1" << endl; };
    auto y = []() { cout << "hello world 2" << endl; };
    show_type_and_size(x);
    show_type_and_size(y);

    function<void(void)> fx = x;
    show_type_and_size(fx);

    return 0;
}
