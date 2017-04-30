// is_copy_assignable_0.cpp
#include <iostream>
#include <memory>
#include <mutex>
using std::cout;
using std::endl;


struct true_type {
    static constexpr bool value = true;
};
struct false_type {
    static constexpr bool value = false;
};

// primary defination.
template<typename T>
struct is_copy_assignable : public true_type {
};

// partial specialization.
template<class T, class... P1toN>
struct is_copy_assignable<T,T, P1toN...> : public true_type {
};
template<class T, class U, class... P1toN>
struct is_copy_assignable<T, U, P1toN...> {
    static constexpr bool value = is_copy_assignable<T, P1toN...>::value;
};


int main(int argc, char *argv[])
{
    cout << "is_copy_assignable<unique_ptr<int> > = " <<
            is_copy_assignable<<std::unique_ptr<int> >::value << endl;

    cout << "is_copy_assignable<mutex> = " <<
            is_copy_assignable<<std::mutex>::value << endl;


    cout << "is_copy_assignable<lock_guard> = " <<
            is_copy_assignable<<std::lock_guard<std::mutex> >::value << endl;

    cout << "is_copy_assignable<int > = " <<
            is_copy_assignable<int>::value << endl;

    return 0;
}
