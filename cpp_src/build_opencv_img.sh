# c++ -std=c++11 opencv_show_img.cpp `pkg-config --cflags --libs opencv `
set -ex
c++ -ggdb -std=c++11 opencv_show_yuv.cpp `pkg-config --cflags --libs opencv `

#./a.out /Users/wangchunye/d/working/prof/yuv/vdec_6-8__p420_1920x1080.yuv 1920 1080

./a.out /Users/wangchunye/d/working/prof/yuv/vvo-4-9-_p420_1920x1080.yuv 320 180
