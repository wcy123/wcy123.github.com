#include <time.h>

#include <iostream>
using namespace std;

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
                       buf_[c*width*height*num_of_colors + w*height*num_of_colors + h*num_of_colors + r] = r;
                   }
               }
           }
       }
    }
    ~Video() {
        delete[] buf_;
    }
    float SumOfFirstColumn();
    float SumOfFirstRow();
   private:
    const size_t num_of_channels_;
    const size_t width_;
    const size_t height_;
    const size_t num_of_colors_;
    char * buf_;
};

float Video::SumOfFirstColumn() {
    float ret = 0.0f;
    for (size_t c = 0; c < num_of_channels_; ++c) {
        for (size_t w = 0; w < 1; ++w) {
            for (size_t h = 0; h < height_; ++h) {
                for (size_t r = 0; r < num_of_colors_; ++r) {
                    ret += buf_[c * width_ * height_ * num_of_colors_ + w * height_ * num_of_colors_ + h * num_of_colors_ + r];
                }
            }
        }
    }
    return ret;
}
float Video::SumOfFirstRow() {
    float ret = 0.0f;
    for (size_t c = 0; c < num_of_channels_; ++c) {
        for (size_t w = 0; w < width_; ++w) {
            for (size_t h = 0; h < 1; ++h) {
                for (size_t r = 0; r < num_of_colors_; ++r) {
                    ret += buf_[c * width_ * height_ * num_of_colors_ + w * height_ * num_of_colors_ + h * num_of_colors_ + r];
                }
            }
        }
    }
    return ret;
}

int time_diff(const struct timespec& ts_end, const struct timespec& ts_start) {
    return (ts_start.tv_sec - ts_end.tv_sec) * 1000 -
           (ts_start.tv_nsec - ts_end.tv_nsec) / 1000;
}
int main(int argc, char *argv[])
{
    Video v(10,1920,1080,3);
    struct timespec ts_start;
    struct timespec ts_end;

    clock_gettime(CLOCK_MONOTONIC, &ts_start);
    float r1 = v.SumOfFirstColumn();
    clock_gettime(CLOCK_MONOTONIC, &ts_end);
    cout << __FILE__ << ":" << __LINE__ << ": [" << __FUNCTION__ << "] "
         << "r1 " << r1 << " "
         << " it takes " << time_diff(ts_end, ts_start) << " ms"
         << endl;

    clock_gettime(CLOCK_MONOTONIC, &ts_start);
    float r2 = v.SumOfFirstRow();
    clock_gettime(CLOCK_MONOTONIC, &ts_end);
    cout << __FILE__ << ":" << __LINE__ << ": [" << __FUNCTION__ << "] "
         << "r2 " << r2 << " "
         << " it takes " << time_diff(ts_end, ts_start) << " ms"
         << endl;
return 0;
}
