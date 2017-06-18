#include <iostream>
#include <type_traits>
#include <regex>
using namespace std;



template<typename T>
struct function_traits;

template<typename R, typename ...Args>
struct function_traits<std::function<R(Args...)>>
{
    static const size_t nargs = sizeof...(Args);

    typedef R result_type;

    template <size_t i>
    struct arg
    {
        typedef typename std::tuple_element<i, std::tuple<Args...>>::type type;
    };
};


template<typename T, class Void = void>
struct Parser {
    static const string re();
    static T parse(const string & text);
};


struct Parser<T, typename enable_if<is_integral<T>::value>::type> {
    static const string re() {
        return "100";
    }
    static T parse(const string & text) {
        return 0;
    }
};

int main(int argc, char *argv[])
{
    string x = Parser<int>::re();
    cout << x << endl;
    return 0;
}
