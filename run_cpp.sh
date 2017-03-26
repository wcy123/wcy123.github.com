file=$1

c++ -std=c++11 $1 -o /tmp/a.out &&  /tmp/a.out >${file%.cpp}.out 2>&1
