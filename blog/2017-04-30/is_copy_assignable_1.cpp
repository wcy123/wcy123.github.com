// is_copy_assignable_1.cpp
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

template<typename T>
T declval();

// the imfamous void_t
template<class...>
using void_t = void;


// primary defination.
template<typename T, class = void>
struct is_copy_assignable : public false_type {
};

// partial specialization
template<typename T>
struct is_copy_assignable<T, void_t<decltype( declval<T&>() = declval<T const&>() )> >
        : public true_type {
};



int main(int argc, char *argv[])
{
    cout << "is_copy_assignable<unique_ptr<int> > = " <<
            is_copy_assignable<std::unique_ptr<int> >::value << endl;

    cout << "is_copy_assignable<mutex> = " <<
            is_copy_assignable<std::mutex>::value << endl;


    cout << "is_copy_assignable<lock_guard> = " <<
            is_copy_assignable<std::lock_guard<std::mutex> >::value << endl;

    cout << "is_copy_assignable<int > = " <<
            is_copy_assignable<int>::value << endl;

    return 0;
}
