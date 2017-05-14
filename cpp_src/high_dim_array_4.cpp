#include <iostream>
using namespace std;
template <size_t dims, >
struct ArrayIndex {
    ArrayIndex(size_t size, const ArrayIndex<dims - 1>& next)
        : size_(size), next_(next) {}
    const ArrayIndex<dims - 1>& operator[](size_t index) {
        index* next_.GetSize();
    }
    size_t GetSize() const {
        return size_t;
    }
    const size_t size_;
    const ArrayIndex<dims-1> next_;
};

class Video {
  public:
   Video(size_t num_of_channels, size_t width, size_t height,
         size_t num_of_colors)
       : num_of_channels_(num_of_channels),
         width_(width),
         height_(height),
         num_of_colors_(num_of_colors) {
       buf_ = new char[num_of_channels * width * height * num_of_colors];
       for (size_t c = 0; c < num_of_channels; ++c) {
           for (size_t w = 0; w < width; ++w) {
               for (size_t h = 0; h < height; ++h) {
                   for (size_t r = 0; r < num_of_colors; ++r) {
                       (*this)(c,w,h,r) = 0.0;
                   }
               }
           }
       }
    }

    char& operator()(size_t c, size_t w, size_t h, size_t r) {
        return buf_[c * width_ * height_ * num_of_colors_ +
                    w * height_ * num_of_colors_ + h * num_of_colors_ + r];
    }

    ~Video() {
        delete[] buf_;
    }

   private:
    const size_t num_of_channels_;
    const size_t width_;
    const size_t height_;
    const size_t num_of_colors_;
    char * buf_;
};


int main(int argc, char *argv[])
{
    Video v(10,1920,1080,3);
    return 0;
}
