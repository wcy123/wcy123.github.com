#include <iostream>
using namespace std;

#include <iostream>
#include <chrono>
#include <thread>
class Foo {
  public:
    Foo(int i) {
        cout <<  __FILE__ << ":" << __LINE__ << ": [" << __FUNCTION__<< "] "
             << "it takes long time to construct foo object."
             << endl;
    }
    Foo(const Foo& other) = delete;
    Foo(Foo&& other) {
        cout <<  __FILE__ << ":" << __LINE__ << ": [" << __FUNCTION__<< "] "
             << "it takes some time to move the object"
             << endl;
    }
    Foo operator=(const Foo& other) = delete;

    ~Foo() {
        cout <<  __FILE__ << ":" << __LINE__ << ": [" << __FUNCTION__<< "] "
             << endl;
    }
};

void independentThread()
{
    Foo foo(1);
    std::cout << "Starting concurrent thread.\n";
    std::this_thread::sleep_for(std::chrono::seconds(2));
    std::cout << "Exiting concurrent thread.\n";
}

void threadCaller()
{
    std::cout << "Starting thread caller.\n";
    std::thread t(independentThread);
    t.detach();
    std::this_thread::sleep_for(std::chrono::seconds(1));
    std::cout << "Exiting thread caller.\n";
}

int main()
{
    threadCaller();
    std::cout << "main thread caller.\n";
    std::this_thread::sleep_for(std::chrono::seconds(1));
    std::cout << "main exit.\n";
}
