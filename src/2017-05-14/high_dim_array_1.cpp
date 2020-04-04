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
       buf_ = new char***[num_of_channels];
       for (size_t c = 0; c < num_of_channels; ++c) {
           buf_[c] = new char**[width];
           for (size_t w = 0; w < width; ++w) {
               buf_[c][w] = new char*[height];
               for (size_t h = 0; h < height; ++h) {
                   buf_[c][w][h] = new char[num_of_colors];
                   for (size_t r = 0; r < num_of_colors; ++r) {
                       buf_[c][w][h][r] = 0.0;
                   }
               }
           }
       }
    }
    ~Video() {
        for (size_t c = 0; c < num_of_channels_; ++c) {
            for (size_t w = 0; w < width_; ++w) {
                for (size_t h = 0; h < height_; ++h) {
                    delete[] buf_[c][w][h];
                }
                delete[] buf_[c][w];
            }
            delete[] buf_[c];
        }
        delete[] buf_;
    }

   private:
    const size_t num_of_channels_;
    const size_t width_;
    const size_t height_;
    const size_t num_of_colors_;
    char **** buf_;
};


int main(int argc, char *argv[])
{
    Video v(10,1920,1080,3);
    return 0;
}
