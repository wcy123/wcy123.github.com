#include <iostream>
#include <functional>
#include <boost/type_index.hpp>
using namespace std;

#define SHOW_TYPE_AND_SIZE(expr) if(1){             \
        cout << #expr << ":\n"; show_type_and_size(expr);   \
}while(0)


template<typename T>
void show_type_and_size(T&& x) {
    cout << "T = "
         << boost::typeindex::type_id_with_cvr<T>().pretty_name() << ";\n"
         << boost::typeindex::type_id_with_cvr<decltype(x)>().pretty_name() << " x;\n"
         << "sizeof(x) "  << sizeof(x) << "\n"
         << endl;
}
int foo() {
    return 100;
}
int main(int argc, char *argv[])
{
    int x = 100;
    int&& rf = foo();
    SHOW_TYPE_AND_SIZE(100);
    SHOW_TYPE_AND_SIZE(rf);
    SHOW_TYPE_AND_SIZE(foo());
    SHOW_TYPE_AND_SIZE(x);
    return 0;
}
