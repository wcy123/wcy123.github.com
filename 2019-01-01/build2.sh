set -xe;

(g++ -DNOEXCEPT= -DCOPY_CTOR -std=c++11 vector_data_race.cpp && ./a.out 2>&1)| tee vector_data_race1.out
(g++ -DNOEXCEPT= -DMOVE_CTOR -std=c++11 vector_data_race.cpp && ./a.out 2>&1) | tee vector_data_race2.out
(g++ -DNOEXCEPT= -DCOPY_CTOR -DMOVE_CTOR -std=c++11 vector_data_race.cpp && ./a.out 2>&1) | tee vector_data_race3.out
(g++ -DNOEXCEPT=noexcept -DCOPY_CTOR -DMOVE_CTOR -std=c++11 vector_data_race.cpp && ./a.out 2>&1) | tee vector_data_race4.out
