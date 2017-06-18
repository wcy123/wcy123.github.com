#include <iostream>
#include <type_traits>
#include <boost/type_index.hpp>
using namespace std;

struct A {
    template <typename T>
    void parse_value(const std::string& text, T & value) {
        typedef typename std::enable_if<std::is_integral<T>::value, T>::type XT;
        cout <<  __FILE__ << ":" << __LINE__ << ": [" << __FUNCTION__<< "] "
             << boost::typeindex::type_id_with_cvr<T>().pretty_name()
             << endl;
    }
    template <typename T>
    std::enable_if<std::is_integral<T>::value> parse_value(
        const std::string &text,
        T & value) {
            cout <<  __FILE__ << ":" << __LINE__ << ": [" << __FUNCTION__<< "] "
            << endl;
    }
};
int main(int argc, char *argv[])
{
    A a ;
    typedef std::enable_if<std::is_integral<uintptr_t>::value, uintptr_t>::type T;
    cout <<  __FILE__ << ":" << __LINE__ << ": [" << __FUNCTION__<< "] "
         << "is_integral<uintptr_t> "  << is_integral<uintptr_t>::value << " "
         << " " << boost::typeindex::type_id_with_cvr<T>().pretty_name()
         << endl;

    uintptr_t xx = 0;
    a.parse_value(string("hello"), xx);
    return 0;
}
