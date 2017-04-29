set -x
c++ -std=c++11 -DEXTRA=1 -c -O3 -fno-inline -o extn_tmpl_1.o  extn_tmpl_1.cpp
c++ -std=c++11 -DEXTRA=2 -c -O3 -fno-inline -o extn_tmpl_2.o  extn_tmpl_2.cpp
c++ -std=c++11 -c -O3 -fno-inline -o extn_tmpl_main.o extn_tmpl_main.cpp
nm extn_tmpl_2.o | c++filt
nm extn_tmpl_1.o | c++filt
nm extn_tmpl_main.o | c++filt

ls -l extn_tmpl_1.o  extn_tmpl_2.o

c++  extn_tmpl_main.o extn_tmpl_1.o extn_tmpl_2.o

./a.out 1
echo $?

./a.out 2
echo $?



c++  extn_tmpl_main.o extn_tmpl_2.o extn_tmpl_1.o

./a.out 1
echo $?

./a.out 2
echo $?


c++ -std=c++11 -DEXTERN_TMPL -DEXTRA=1 -c -O3 -fno-inline -o extn_tmpl_1.o  extn_tmpl_1.cpp
c++ -std=c++11 -DEXTERN_TMPL -DEXTRA=2 -c -O3 -fno-inline -o extn_tmpl_2.o  extn_tmpl_2.cpp

c++  extn_tmpl_main.o extn_tmpl_1.o extn_tmpl_2.o

nm extn_tmpl_2.o | c++filt
nm extn_tmpl_1.o | c++filt
nm extn_tmpl_main.o | c++filt

ls -l extn_tmpl_1.o  extn_tmpl_2.o


c++ -std=c++11 -DEXTERN_TMPL -DEXTRA=0 -c -O3 -fno-inline -o extn_tmpl_main.o extn_tmpl_main.cpp || exit 1

nm extn_tmpl_main.o | c++filt

c++  extn_tmpl_main.o extn_tmpl_1.o extn_tmpl_2.o

./a.out 1
echo $?

./a.out 2
echo $?

./a.out 0
echo $?


c++  extn_tmpl_main.o extn_tmpl_2.o extn_tmpl_1.o

./a.out 1
echo $?

./a.out 2
echo $?

./a.out 0
echo $?
