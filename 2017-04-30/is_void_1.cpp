// is_void_0.cpp
#include <iostream>
using std::cout;
using std::endl;


struct true_type {
    static constexpr bool value = true;
};
struct false_type {
    static constexpr bool value = false;
};

// primary defination.
template<class T, class U>
struct is_same : public false_type {
};

// partial specialization.
template<class T>
struct is_same<T,T> : public true_type {
};
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


// primary defination.
template<class T>
struct is_void : public is_same<typename remove_cv<T>::type, void> {
};



int main(int argc, char *argv[])
{
    cout << "is_void<int> = " << is_void<int>::value << endl;
    cout << "is_void<void> = " << is_void<void>::value << endl;
    cout << "is_void<void const> = " << is_void<void const>::value << endl;
    cout << "is_void<void volatile> = " << is_void<void volatile>::value << endl;
    cout << "is_void<void const volatile> = " << is_void<void const volatile>::value << endl;
    return 0;
}
