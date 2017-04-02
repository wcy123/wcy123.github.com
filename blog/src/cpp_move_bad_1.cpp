#include <iostream>
#include <vector>
using namespace std;

using ints = vector<int>;

ostream& operator<<(ostream& out, ints& v){
    for(auto i: v){
        out << " " << i;
    }
    return out;
}

int main(int argc, char *argv[])
{
    ints i1{1,2,3,4};
    cout << "before invoke i2(move(i1)): " << i1 << endl;
    ints i2(std::move(i1));
    cout << "after invoke i2(move(i1)): " << i1 << endl;
    cout << "    i1.size() = " << i1.size() << endl;
    cout << "and i2: " << i2 << endl;
    return 0;
}
