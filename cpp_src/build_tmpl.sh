set -ex
c++ -I/usr/local/include -std=c++11 -Wall -Werror is_same_0.cpp
./a.out | tee is_same_0.out


c++ -I/usr/local/include -std=c++11 -Wall -Werror is_void_0.cpp
./a.out | tee is_void_0.out



c++ -I/usr/local/include -std=c++11 -Wall -Werror remove_cv_0.cpp
./a.out | tee remove_cv_0.out


c++ -I/usr/local/include -std=c++11 -Wall -Werror is_void_1.cpp
./a.out | tee is_void_1.out


c++ -I/usr/local/include -std=c++11 -Wall -Werror remove_cv_1.cpp
./a.out | tee remove_cv_1.out

c++ -I/usr/local/include -std=c++11 -Wall -Werror is_one_of_0.cpp
./a.out | tee is_one_of_0.out


c++ -I/usr/local/include -std=c++11 -Wall -Werror is_copy_assignable_0.cpp
./a.out | tee is_copy_assignable_0.out
