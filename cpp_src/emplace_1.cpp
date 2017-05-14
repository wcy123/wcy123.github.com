#include <iostream>
#include <vector>
using namespace std;

class Foo {
  public:
    Foo(int i) {
        cerr <<  __FILE__ << ":" << __LINE__ << ": [" << __FUNCTION__<< "] "
             << "it takes long time to construct foo object."
             << endl;
    }
    Foo(const Foo& other) = delete;
    Foo(Foo&& other) {
        cerr <<  __FILE__ << ":" << __LINE__ << ": [" << __FUNCTION__<< "] "
             << "it takes some time to move the object"
             << endl;
    }
    Foo operator=(const Foo& other) = delete;
};


int main(int argc, char *argv[])
{
    vector<Foo> vec;
    vec.reserve(4);
    vec.push_back(Foo(1));
    vec.emplace_back(1);
    vec.emplace_back(Foo(1));
    return 0;
}
