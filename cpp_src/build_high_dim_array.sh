set -ex
c++ -std=c++11 high_dim_array_1.cpp
time ./a.out
c++ -std=c++11 high_dim_array_2.cpp
time ./a.out
