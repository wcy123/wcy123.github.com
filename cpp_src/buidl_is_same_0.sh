set -ex
c++ -I/usr/local/include -std=c++11 -Wall -Werror is_same_0.cpp
./a.out | tee is_same_0.out

set -ex
c++ -I/usr/local/include -std=c++11 -Wall -Werror is_void_0.cpp
./a.out | tee is_void_0.out

c++ -I/usr/local/include -std=c++11 -Wall -Werror is_void_1.cpp
./a.out | tee is_void_1.out
