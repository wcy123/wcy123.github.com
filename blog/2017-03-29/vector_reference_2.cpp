#include <iostream>
#include <vector>
using namespace std;
class Foo
{
  public:
    Foo() {
        cerr <<  __FILE__ << ":" << __LINE__ << ": [" << __FUNCTION__<< "] "
             << "this "  << this << " "
             << endl;
        value = 100;
    }
    Foo(const Foo& other) {
        cerr <<  __FILE__ << ":" << __LINE__ << ": [" << __FUNCTION__<< "] "
             << "this "  << this << " "
             << "&other "  << &other << " "
             << endl;
        value = 100;
    }
    ~Foo(){
        cerr <<  __FILE__ << ":" << __LINE__ << ": [" << __FUNCTION__<< "] "
             << "this "  << this << " "
             << endl;
        value = 0;
    }
    int value;
};
ostream& operator<<(ostream& o, const vector<Foo>& c)
{
    for(auto & i: c){
        o << " " << i.value;
    }
    return o;
}
int main(int argc, char *argv[])
{
    vector<Foo> v;
    v.emplace_back();
    auto & r1 = v[0];
    cout << v << endl;
    v.emplace_back();
    r1.value = 999;
    cerr <<  __FILE__ << ":" << __LINE__ << ": [" << __FUNCTION__<< "] "
         << "&r1 "  << &r1 << " "
         << endl;
    cout << v << endl;
    return 0;
}
