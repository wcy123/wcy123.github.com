#include <iostream>
#include <vector>
using namespace std;
class Foo
{
  public:
    Foo() {
        value = 100;
        cerr <<  __FILE__ << ":" << __LINE__ << ": [" << __FUNCTION__<< "] "
             << "this "  << this << " "
             << endl;
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
    o << endl;
    return o;
}
int main(int argc, char *argv[])
{
    vector<Foo> v;
    v.emplace_back();
    auto & r1 = v[0];
    r1.value = 101;
    cout << v << endl;

    v.emplace_back();
    r1.value = 102;
    cout << v << endl;
    return 0;
}
