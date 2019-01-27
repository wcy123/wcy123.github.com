#include <iostream>

using namespace std;

struct pixel_parallel{
};



int main(int argc, char *argv[])
{
    std::cout <<  __FILE__ << ":" << __LINE__ << ": [" << __FUNCTION__<<"]"//
              << "CHANNEL_PARALLEL::value "  << typeid(pixel_parallel).name() << " " //
              << endl;
    return 0;
}
