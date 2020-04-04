// is_one_of_0.cpp
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
template<class T, class... P0toN>
struct is_one_of : public false_type {
};

// partial specialization.
template<class T, class... P1toN>
struct is_one_of<T,T, P1toN...> : public true_type {
};
template<class T, class U, class... P1toN>
struct is_one_of<T, U, P1toN...> : public is_one_of<T, P1toN...> {
};


int main(int argc, char *argv[])
{
    cout << "is_one_of<int> = " << is_one_of<int>::value << endl;
    cout << "is_one_of<int,int> = " << is_one_of<int,int>::value << endl;
    cout << "is_one_of<int,float,int> = " << is_one_of<int,float,int>::value << endl;
    cout << "is_one_of<int,float,double> = " << is_one_of<int,float,double>::value << endl;

    return 0;
}
