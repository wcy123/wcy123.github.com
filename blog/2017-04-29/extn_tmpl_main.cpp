// extn_tmpl_main.cpp
extern int foo_caller1(int a, int b);
extern int foo_caller2(int a, int b);

#ifdef EXTERN_TMPL
#include "extn_tmpl.hpp"
template int foo(int a, int b);
#endif


int main(int argc, char *argv[])
{
    if(argv[1][0] == '1'){
        return foo_caller1(1,1);
    }else if(argv[1][0] == '2'){
        return foo_caller2(1,1);
    }
    return 0;
}
