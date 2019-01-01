// has_type_member_0.cpp
#include <iostream>
#include <type_traits>
using std::cout;
using std::endl;


struct true_type {
    static constexpr bool value = true;
};
struct false_type {
    static constexpr bool value = false;
};

// the imfamous void_t
template<class...>
using void_t = void;


// primary defination.
template<class T, class = void >
struct has_type_member : public false_type {
};

// partial specialization.
template<class T>
struct has_type_member<T, void_t<typename T::type> > : public true_type {
};


int main(int argc, char *argv[])
{
    cout << "has_type_member<int> = " << has_type_member<int>::value << endl;
    cout << "has_type_member<std::enable_if<true,void> > = " << has_type_member<std::enable_if<true,void> >::value << endl;
    return 0;
}
