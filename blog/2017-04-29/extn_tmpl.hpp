// extn_tmpl.hpp
#pragma once

template<typename T>
T foo(T a , T b ){
    return a + b + EXTRA;
}
#ifdef EXTERN_TMPL
extern template int foo<int>(int a, int b);
#endif
