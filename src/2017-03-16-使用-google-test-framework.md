# 使用 google test framework



本文参考 [http://www.ibm.com/developerworks/aix/library/au-googletestingframework.html]()


# 下载 google test

如果使用 cmake

```cmake
include(ExternalProject)
ExternalProject_Add(
  googletest
  GIT_REPOSITORY "https://github.com/google/googletest"
)
include_directories("${SOURCE_DIR}/googletest/include")
link_directories("${BINARY_DIR}/googlemock/gtest/")
add_executable(test1
  ${CMAKE_CURRENT_LIST_DIR}/test1.cpp)
target_link_libraries(test1 gtest gtest_main)
```

test1.cpp 的内容

```c++
#include <iostream>
#include <vector>
#include <algorithm>
#include <iterator>
#include <functional>
#include <deque>
#include <cassert>
#include <cmath>
using namespace std;

#include "gtest/gtest.h"


double square_root (const double x)
{
    return sqrt(x);
}
TEST (SQUARE_ROOT_TEST, PositiveNos) {
    ASSERT_NEAR (18.0, square_root (324.0), 1.0e-4);
    ASSERT_NEAR (25.4, square_root (645.16), 1.0e-4);
    ASSERT_NEAR (50.3321, square_root (2533.310224),1.0e-4);
}
```

可用的宏

 - `ASSERT_FLOAT_EQ`
 - `ASSERT_DOUBLE_EQ`
 - `EXPECT_NEAR`
 - `EXPECT_EQ`
 - `ASSERT_DEATH`
 - `ASSERT_EXIT`


# todo

understand fixtures
