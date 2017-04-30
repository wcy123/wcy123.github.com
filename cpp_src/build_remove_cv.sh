set -ex
c++ -I/usr/local/include -std=c++11 -Wall -Werror remove_cv_0.cpp
./a.out | tee remove_cv.out
