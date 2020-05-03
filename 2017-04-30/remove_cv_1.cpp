// remove_cv_0.cpp
#include <iostream>
#include <boost/type_index.hpp>
using boost::typeindex::type_id_with_cvr;
using std::cout;
using std::endl;

// primary defination.
template<class T>
struct remove_const {
    using type = T;
};
// partial specialization
template<class T>
struct remove_const<T const> {
    using type = T;
};

// primary defination
template<class T>
struct remove_volatile{
    using type = T;
};
// partial specialization
template<class T>
struct remove_volatile<T volatile> {
    using type = T;
};

// bang
template<class T>
struct remove_cv {
    using type = typename remove_const<typename remove_volatile<T>::type >::type;
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
