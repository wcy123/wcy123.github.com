#include <iostream>
#include <vector>
#include <string>
using namespace std;

vector<string> fun5() { return 10u; }

int main(int argc, char *argv[])
{
    auto vs = fun5();
    cout << vs.size() << endl;
}
