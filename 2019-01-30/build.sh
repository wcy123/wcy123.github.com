set -x
g++ -std=c++11 test_rvo.cpp && ./a.out
g++ -fno-elide-constructors -std=c++11 test_rvo.cpp && ./a.out
