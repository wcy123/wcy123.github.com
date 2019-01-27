#include <vector>
#include <iostream>
using namespace std;

class Foo {
  public:
    explicit Foo(int i) : i_{i}{
        std::cerr <<  __FILE__ << ":" << __LINE__ << ": [" << __PRETTY_FUNCTION__<<"]"//
                  << "this "  << (void*)this << " " //
                  << "i "  << i_ << " " //
                  << std::endl;
    }
#ifdef COPY_CTOR
    Foo(const Foo& other) : i_ {other.i_} {
        std::cerr <<  __FILE__ << ":" << __LINE__ << ": [" << __PRETTY_FUNCTION__<<"]"//
                  << "this "  << (void*)this << " " //
                  << "other.i "  << i_ << " " //
                  << std::endl;
    }
#endif
#ifdef MOVE_CTOR
    Foo(Foo&& other) NOEXCEPT : i_ {other.i_}  {
        std::cerr <<  __FILE__ << ":" << __LINE__ << ": [" << __PRETTY_FUNCTION__<<"]"//
                  << "this "  << (void*)this << " " //
                  << "other.i "  << i_ << " " //
                  << std::endl;
    }
#endif
    ~Foo() {
        std::cerr <<  __FILE__ << ":" << __LINE__ << ": [" << __PRETTY_FUNCTION__<<"]"//
                  << "this "  << (void*)this << " " //
                  << "i "  << i_ << " " //
                  << std::endl;
    }
    int i_;
};

int main(int argc, char *argv[])
{
    vector<Foo> v;
    v.emplace_back(0);
    Foo& v0 = v[0];
    cout << "before push: v.capacity() = " << v.capacity() << endl;
    cout << "v.size() = " << v.size() << endl;
    v.emplace_back(1);
    cout << "after push: v.capacity() = " << v.capacity() << endl;
    cout << "&v0 is dangling pointer: " << (void*)&v0 << endl;
    return 0;
}
