// remove_erase_3.cpp
#include <iostream>
using namespace std;
#include <vector>

class Foo {
  public:
    Foo(int v): value(v) {
    }
    Foo(const Foo& other): value(other.value) {

    }
    Foo& operator =(Foo&& other) {
        cerr <<  __FILE__ << ":" << __LINE__ << ": [" << __FUNCTION__<< "] "
             << "other.value "  << other.value << " "
             << "value "  << value << " "
             << endl;
        value = other.value;
        other.value = 0;
        return *this;
    }
  public:
    int value;
};

using namespace std;
int main(int argc, char *argv[])
{
    vector<Foo> avec={1,2,3,4,6,7,8};
    auto it = remove_if(avec.begin(), avec.end(), [](Foo& a) { return a.value %2 == 0; });
    cout << "before erase " << endl;
    for(auto i : avec){
        cout << "i= " << i.value << endl;
    }

    avec.erase(it, avec.end());

    cout << "after erase " << endl;
    for(auto i : avec){
        cout << "i= " << i.value << endl;
    }
    return 0;
}
