// remove_erase_2.cpp
#include <iostream>
#include <vector>
using namespace std;
int main(int argc, char *argv[])
{
    vector<int> avec={1,2,3,4,6,7,8};
    auto it = remove_if(avec.begin(), avec.end(), [](int a) { return a %2 == 0; });
    cout << "before erase " << endl;
    for(auto i : avec){
        cout << "i= " << i << endl;
    }

    avec.erase(it, avec.end());

    cout << "after erase " << endl;
    for(auto i : avec){
        cout << "i= " << i << endl;
    }
    return 0;
}
