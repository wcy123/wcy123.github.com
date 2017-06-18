#include <cstdlib>
#include <iostream>

using namespace std;

static uintptr_t align(uintptr_t addr, size_t alignment) {
    uintptr_t ret = addr;
    size_t mask = alignment - 1;
    if (addr & mask) {
        ret = (addr + alignment) & ~mask;
    }
    return ret;
}

int main(int argc, char *argv[])
{
    for(int i = 0; i < 32; ++i){
        cerr << __FILE__ << ":" << __LINE__ << ": [" << __FUNCTION__ << "] "
             << "i= " << i << " , align(i, 8)  = " << align(i, 4) << " "
             << endl;
    }
    return 0;
}
