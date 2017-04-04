#include <iostream>
#include <vector>

using namespace std;

template<typename T>
ostream& operator<<(ostream& o, const vector<T>& c)
{
    for(auto & i: c){
        o << " " << i;
    }
    return o;
}


int main(int argc, char *argv[])
{
    vector<int> v1{};
    vector<int> v2{1,2};
    vector<int> v3(3,10);
    vector<int> v4({3,10});
    vector<int> v5{{3,10}};
    cout << "v1 =" << v1 << endl;
    cout << "v2 =" << v2 << endl;
    cout << "v3 =" << v3 << endl;
    cout << "v4 =" << v4 << endl;
    cout << "v5 =" << v5 << endl;
    return 0;
}
