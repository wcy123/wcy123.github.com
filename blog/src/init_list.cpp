#include <iostream>
using namespace std;
struct Point {
    int x;
    float y;
};
int main(int argc, char *argv[])
{
    auto x = {0, 1.0};
    Point p = x;
    cout<< p.x << endl;
    return 0;
}
