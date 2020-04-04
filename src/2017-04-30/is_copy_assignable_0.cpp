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

template<typename T>
T declval();


// primary defination.
template<typename T>
struct is_copy_assignable {
  private:
    template<class U, class = decltype( declval<U&>() = declval<U const&>() )>
    static true_type try_assignment(U&& );

    static false_type try_assignment(...);
  public:
    using type = decltype( try_assignment(declval<T>()));
    static constexpr bool value = type::value;
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
