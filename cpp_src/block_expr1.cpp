#include <iostream>
using namespace std;

int foo(int a){
    return a + 100;
}

int main(int argc, char *argv[])
{
    auto x = ({int a = 100; foo(a);});
    return 0;
}
