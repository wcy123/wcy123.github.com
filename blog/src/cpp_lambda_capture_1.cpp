#include <iostream>
#include <functional>
#include <boost/type_index.hpp>
using namespace std;
class Foo {
  public:
    Foo():i(100) {
        cout <<  __FILE__ << ":" << __LINE__ << ": [" << __PRETTY_FUNCTION__<< "] "
             << "i "  << i << " "
             << endl;
    }
    Foo(const Foo& foo):i(foo.i + 1) {
        cout <<  __FILE__ << ":" << __LINE__ << ": [" << __PRETTY_FUNCTION__<< "] "
             << "i "  << i << " "
             << endl;
    }
    ~Foo() {
        cout <<  __FILE__ << ":" << __LINE__ << ": [" << __PRETTY_FUNCTION__<< "] "
             << "i "  << i << " "
             << endl;
        i = -100;
    }
  public:
    int i = 0;
    friend ostream& operator<<(ostream& out, const Foo & obj);
};
ostream& operator<<(ostream& out, const Foo & obj)
{
    out << "foo(" << &obj << "," << obj.i << ")";
    return out;
}
void
CreateLambda() {
    Foo foo;
    std::function<void(void)> ret1;
    {
        cout << "before create lambda1" << endl;
        auto lambda1 =  [foo](){ cout << foo.i << endl;};
        cout << "after create lambda1" << endl;
        {
            cout << "before create lambda2" << endl;
            auto lambda2 =  lambda1;
            cout << "after create lambda2" << endl;
            {
                cout << "before create lambda3" << endl;
                auto lambda3 =  lambda2;
                cout << "after create lambda3" << endl;
                {
                    cout << "before create lambda3" << endl;
                    auto lambda4 =  lambda3;
                    cout << "after create lambda4" << endl;
                    cout << "before lambda4 out of scope" << endl;
                }
                cout << "after lambda4 out of scope" << endl;
            }
            cout << "after lambda3 out of scope" << endl;
            cout << "before lambda2 out of scope" << endl;
        }
        cout << "after lambda2 out of scope" << endl;
    }
    //return ret1;
}
int main(int argc, char *argv[])
{
    CreateLambda();
    return 0;
}
