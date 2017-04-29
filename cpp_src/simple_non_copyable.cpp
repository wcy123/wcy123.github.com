#include <iostream>
#include <memory>
using namespace std;


class Foo {
  public:
  private:
    unique_ptr<int> pInt_;
};


int main(int argc, char *argv[])
{
    Foo a;
    Foo b{a};

    return 0;
}
