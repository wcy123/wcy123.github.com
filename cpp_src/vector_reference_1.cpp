#include <iostream>
#include <vector>
using namespace std;
class Foo
{
  public:
    Foo() {
        value = 100;
    }
    ~Foo(){
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
    v.reserve(10);
    v.emplace_back();
    auto & r1 = v[0];
    cout << "before change r1:" << v << endl;
    v.emplace_back();
    r1.value = 999;
    cout << "after change r1:" << v << endl;
    return 0;
}
