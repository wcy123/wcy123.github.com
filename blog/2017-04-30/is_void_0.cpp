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
template<class T>
struct is_void : public false_type {
};

// partial specialization.
template<>
struct is_void<void> : public true_type {
};
template<>
struct is_void<void const> : public true_type {
};
template<>
struct is_void<void const volatile> : public true_type {
};
template<>
struct is_void<void volatile> : public true_type {
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
