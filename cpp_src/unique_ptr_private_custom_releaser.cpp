#include <iostream>
#include <memory>
using namespace std;

class Foo {
  private:
    struct MemLock {
        void operator()(Foo* p) {
            cerr <<  __FILE__ << ":" << __LINE__ << ": [" << __FUNCTION__<< "] "
                 << endl;
        }
    };
  public:
    unique_ptr<Foo, MemLock> get() {
        return unique_ptr<Foo, MemLock>(new Foo());
    }
  private:
    void
};

int main(int argc, char *argv[])
{
    Foo f;
    auto x = f.get();
    return 0;
}
