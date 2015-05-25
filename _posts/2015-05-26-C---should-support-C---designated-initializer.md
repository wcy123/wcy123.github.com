---
layout: post
vtitle:  "C++ should support C99 designated initializer"
date:   2015/05/26 00:53:12
categories:
comments: true
---

[Two Core C99 Features that C++11 Lacks][1] mentions "Designated
Initializers and C++".

I think the 'designated initializer' related with potential
optimization. Here I use "gcc/g++" 5.1 as an example.

```shell
g++ (GCC) 5.1.0
Copyright (C) 2015 Free Software Foundation, Inc.
This is free software; see the source for copying conditions.  There is NO
warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

gcc (GCC) 5.1.0
Copyright (C) 2015 Free Software Foundation, Inc.
This is free software; see the source for copying conditions.  There is NO
warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
```

Here is a simple example.

```c
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

struct point {
    int x;
    int y;
};
const struct point a_point = {.x = 0, .y = 0};
int foo() {
    if(a_point.x == 0){
        printf("x == 0");
        return 0;
    }else{
        printf("x == 1");
        return 1;
    }
}
int main(int argc, char *argv[])
{
    return foo();
}
```

We knew at compilation time, `a_point.x` is zero, so we could expected
that `foo` is optimized into a single `printf`.


```
$ gcc -O3 a.c
$ gdb a.out
(gdb) disassemble foo
Dump of assembler code for function foo:
   0x00000000004004f0 <+0>:	sub    $0x8,%rsp
   0x00000000004004f4 <+4>:	mov    $0x4005bc,%edi
   0x00000000004004f9 <+9>:	xor    %eax,%eax
   0x00000000004004fb <+11>:	callq  0x4003a0 <printf@plt>
   0x0000000000400500 <+16>:	xor    %eax,%eax
   0x0000000000400502 <+18>:	add    $0x8,%rsp
   0x0000000000400506 <+22>:	retq   
End of assembler dump.
(gdb) x /s 0x4005bc
0x4005bc:	"x == 0"
```

Very good! `foo` is optmized to print `x == 0` only.

Let's see c++ version.

```c++
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

struct point {
    point(int _x,int _y):x(_x),y(_y){}
    int x;
    int y;
};
const struct point a_point(0,0);
int foo() {
    if(a_point.x == 0){
	printf("x == 0");
	return 0;
    }else{
	printf("x == 1");
	return 1;
    }
}
int main(int argc, char *argv[])
{
    return foo();
}

```

```
$ g++ -O3 a.cc
$ gdb a.out
(gdb) disassemble foo
Dump of assembler code for function _Z3foov:
   0x00000000004005c0 <+0>:	push   %rbx
   0x00000000004005c1 <+1>:	mov    0x200489(%rip),%ebx        # 0x600a50 <_ZL7a_point>
   0x00000000004005c7 <+7>:	test   %ebx,%ebx
   0x00000000004005c9 <+9>:	je     0x4005e0 <_Z3foov+32>
   0x00000000004005cb <+11>:	mov    $0x1,%ebx
   0x00000000004005d0 <+16>:	mov    $0x4006a3,%edi
   0x00000000004005d5 <+21>:	xor    %eax,%eax
   0x00000000004005d7 <+23>:	callq  0x400460 <printf@plt>
   0x00000000004005dc <+28>:	mov    %ebx,%eax
   0x00000000004005de <+30>:	pop    %rbx
   0x00000000004005df <+31>:	retq   
   0x00000000004005e0 <+32>:	mov    $0x40069c,%edi
   0x00000000004005e5 <+37>:	xor    %eax,%eax
   0x00000000004005e7 <+39>:	callq  0x400460 <printf@plt>
   0x00000000004005ec <+44>:	mov    %ebx,%eax
   0x00000000004005ee <+46>:	pop    %rbx
   0x00000000004005ef <+47>:	retq   
End of assembler dump.
```

We can see that `a_point` is not really a compile time constant value.



[1]: http://www.informit.com/guides/content.aspx?g=cplusplus&seqNum=550
