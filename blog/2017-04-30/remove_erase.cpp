// remove_erase.cpp
#include <iostream>
#include <vector>
using namespace std;
int main(int argc, char *argv[])
{
    vector<int> avec={1,2,3,4,6,7,8};
    avec.erase(remove_if(avec.begin(), avec.end(), [](int a) { return a %2 == 0; }), avec.end());

    for(auto i : avec){
        cout << "i= " << i << endl;
    }
    return 0;
}
