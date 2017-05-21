set -ex
c++ -O3 -std=c++11 high_dim_array_1.cpp
time ./a.out
c++  -O3 -std=c++11 high_dim_array_2.cpp
time ./a.out

c++ -O3 -std=c++11 high_dim_array_3.cpp
./a.out

c++ -O3 -std=c++11 high_dim_array_4.cpp
./a.out
