// remove_cv_0.cpp
#include <iostream>
#include <boost/type_index.hpp>
using boost::typeindex::type_id_with_cvr;
using std::cout;
using std::endl;

// primary defination.
template<class T>
struct remove_cv {
    using type = T;
};
// partial specialization
template<class T>
struct remove_cv<T const> {
    using type = T;
};
template<class T>
struct remove_cv<T volatile> {
    using type = T;
};
template<class T>
struct remove_cv<T volatile const> {
    using type = T;
};


int main(int argc, char *argv[])
{
    cout << "remove_cv<int> = " << type_id_with_cvr<remove_cv<int>::type >().pretty_name() << endl;
    cout << "remove_cv<const int> = " << type_id_with_cvr<remove_cv<int>::type >().pretty_name() << endl;
    cout << "remove_cv<int const> = " << type_id_with_cvr<remove_cv<int>::type >().pretty_name() << endl;
    cout << "remove_cv<int volatile> = " << type_id_with_cvr<remove_cv<int>::type >().pretty_name() << endl;
    cout << "remove_cv<int const volatile> = " << type_id_with_cvr<remove_cv<int>::type >().pretty_name() << endl;
    return 0;
}
