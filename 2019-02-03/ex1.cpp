#include <iostream>
using namespace std;
class Object {
  public:
   ~Object();
};
Object::~Object() {
  cout << "Deconstructor is invoked." << endl;
  throw invalid_argument("another exception");
}
void foo() {
    try {
        Object obj;
        throw invalid_argument("only for testing");
    }catch(const exception & e) {
        cout << "catch exception: " << e.what() << endl;
    }
}
int main(int argc, char *argv[])
{
    foo();
    return 0;
}
