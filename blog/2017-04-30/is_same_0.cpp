// is_same_0.cpp
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


int main(int argc, char *argv[])
{
    cout << "is_same<int,int> = " << is_same<int,int>::value << endl;
    cout << "is_same<int,float> = " << is_same<int,float>::value << endl;
    cout << "is_same<int,const int> = " << is_same<int,const int>::value << endl;
    return 0;
}
