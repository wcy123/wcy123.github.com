#include <iostream>
using namespace std;

#include <memory>
#include <fcntl.h>
#include <cassert>
#include <unistd.h>
using namespace std;

#include <sys/mman.h>
template <typename T, typename... Args>
std::shared_ptr<T> Singleton(Args &&... args) {
    static std::weak_ptr<T> singleton;
    auto ret = singleton.lock();
    if (ret) {
        return ret;
    }
    ret = std::make_shared<T>(std::forward<Args>(args)...);
    singleton = ret;
    return ret;
}
struct DevMem {
    DevMem() {
        dev_fd = open("/dev/mem", O_RDWR | O_SYNC);
        assert(dev_fd > 0);
    }
    ~DevMem() {
        assert(dev_fd > 0);
        close(dev_fd);
    }
    int dev_fd;
};
struct PhysicalMemory {
    PhysicalMemory(off_t offset, size_t length_v)
        : dev_mem(Singleton<DevMem>()), length(length_v) {
        cout <<  __FILE__ << ":" << __LINE__ << ": [" << __FUNCTION__<< "] "
             << "length "  << hex << "0x" << length << dec << " "
             << "offset "  << hex << "0x" << offset << dec << " "
             << endl;
        map_base = (mmap(NULL, length, PROT_READ | PROT_WRITE, MAP_SHARED,
                      dev_mem->dev_fd, offset));
        assert (map_base != MAP_FAILED);
    }
    ~PhysicalMemory() {
        if (map_base != MAP_FAILED) {
            int r = munmap(map_base, length);
            assert(r == 0);
        }
    }
    void* data() {
        return map_base;
    }
    std::shared_ptr<DevMem> dev_mem;
    size_t length;
    void *map_base;
};

template <class _Tp>
struct MemRegValue {
    MemRegValue(PhysicalMemory *phy_mem, off_t offset)
        : pointer_(reinterpret_cast<pointer_type>(
              static_cast<char *>(phy_mem->data()) + offset)) {
    }
    operator volatile _Tp& () {
        return *pointer_;
    }
    _Tp operator =(_Tp v) {
        return *pointer_ = v;
    }
  private:
    using pointer_type = volatile _Tp *;
    pointer_type pointer_;
};

int main(int argc, char *argv[])
{
    return 0;
}
