#include <iostream>
#include <memory>
using namespace std;


class Widget{
};
int main(int argc, char *argv[])
{
    unique_ptr<Widget> uw(new Widget());
    Widget* p = uw;
    return 0;
}
