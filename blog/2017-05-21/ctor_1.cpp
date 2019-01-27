#include <iostream>
using namespace std;

class foo {
  public:
    foo() {
        cerr <<  __FILE__ << ":" << __LINE__ << ": [" << __FUNCTION__<< "] "
             << endl;
        data = 100;
    }
    ~foo() {
        cerr <<  __FILE__ << ":" << __LINE__ << ": [" << __FUNCTION__<< "] "
             << endl;
        data = 0;
    }
    int data;
};

class bar {
  public:
    bar():f_() {
    }
    ~bar() {
        cerr <<  __FILE__ << ":" << __LINE__ << ": [" << __FUNCTION__<< "] "
             << "f_.data "  << f_.data << " "
             << endl;
    }
    foo f_;
};

int main(int argc, char *argv[])
{
    {
        bar b;
    }
    return 0;
}
