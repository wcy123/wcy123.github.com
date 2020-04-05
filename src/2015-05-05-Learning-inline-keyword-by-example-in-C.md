# Learning inline keyword by example in C


C also has `inline` keyword, the sementic is not as same as
C++. [Inline Functions In C](http://www.greenend.org.uk/rjk/tech/inline.html)
has a good explaination. In this blog, I am going to do dome exercises
to make it more concrete.

## static inline functions

This is very simple. Let's start with it.

```c
// in foo.h
typedef void (*func_t)(const char * msg, void * f);
static inline void print_me(const char * msg, void * f)
{
   printf("%s: pointer is %p\n",msg,f);
}
```

```c
// in main.c
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

extern void m1();
extern void m2();
int main(int argc, char *argv[])
{
    m1();
    m2();
    return 0;
}
```

```c
// in a1.c
#include <stdio.h>
#include "foo.h"

void m1()
{
    foo("from a1", NULL);
}
```

```c
16M// in a2.c
#include <stdio.h>
#include "foo.h"

void m2()
{
    foo("from a2", NULL);
}
```

```console
% gcc -O3 -c -o a1.o a1.c
% gcc -O3 -c -o a2.o a2.c
% gcc -O3 -c -o main.o main.c
% gcc -o a.out a1.o a2.o main.o
% ./a.out
from a1: pointer is (nil)
from a1: pointer is (nil)
% objdump -d a1.o
a1.o:     file format elf64-x86-64


Disassembly of section .text:

0000000000000000 <m1>:
   0:	31 d2                	xor    %edx,%edx
   2:	be 00 00 00 00       	mov    $0x0,%esi
   7:	bf 00 00 00 00       	mov    $0x0,%edi
   c:	31 c0                	xor    %eax,%eax
   e:	e9 00 00 00 00       	jmpq   13 <m1+0x13>
% nm a1.o
0000000000000000 T m1
                 U printf

```

With optimzation, we can see `a1.o` does not have local symbol `foo`
defined, and there is no function call `foo`, i.e. it is inlined.

```console
% gcc -O0 -c -o a1.o a1.c
% gcc -O0 -c -o a2.o a2.c
% gcc -O0 -c -o main.o main.c
% gcc -o a.out a1.o a2.o main.o
% ./a.out
from a1: pointer is (nil)
from a1: pointer is (nil)
% objdump -d a1.o

a1.o:     file format elf64-x86-64


Disassembly of section .text:

0000000000000000 <foo>:
   0:	55                   	push   %rbp
   1:	48 89 e5             	mov    %rsp,%rbp
   4:	48 83 ec 10          	sub    $0x10,%rsp
   8:	48 89 7d f8          	mov    %rdi,-0x8(%rbp)
   c:	48 89 75 f0          	mov    %rsi,-0x10(%rbp)
  10:	48 8b 55 f0          	mov    -0x10(%rbp),%rdx
  14:	48 8b 45 f8          	mov    -0x8(%rbp),%rax
  18:	48 89 c6             	mov    %rax,%rsi
  1b:	bf 00 00 00 00       	mov    $0x0,%edi
  20:	b8 00 00 00 00       	mov    $0x0,%eax
  25:	e8 00 00 00 00       	callq  2a <foo%0x2a>
  2a:	c9                   	leaveq
  2b:	c3                   	retq

000000000000002c <m1>:
  2c:	55                   	push   %rbp
  2d:	48 89 e5             	mov    %rsp,%rbp
  30:	be 00 00 00 00       	mov    $0x0,%esi
  35:	bf 00 00 00 00       	mov    $0x0,%edi
  3a:	e8 c1 ff ff ff       	callq  0 <foo>
  3f:	5d                   	pop    %rbp
  40:	c3                   	retq
% nm a1.o
0000000000000000 t foo
000000000000002c T m1
                 U printf

```

On the other hand, if we use `-O0` to disable optimization, we can see
`foo` is defined, and `foo` is invoked, i.e. it is not inlined.

### how to force it to inline?

if we change the `foo.h` as following,

```c
//in foo.h
...
static inline __attribute__((always_inline))
void foo(const char * msg, void * f)
...
```

`__attribute__((always_inline))` force to inline the function, even
when optimization is disabled.

```console
sh build.sh
% gcc -O0 -c -o a1.o a1.c
% gcc -O0 -c -o a2.o a2.c
% gcc -O0 -c -o main.o main.c
% gcc -o a.out a1.o a2.o main.o
% ./a.out
from a1: pointer is (nil)
from a1: pointer is (nil)
% objdump -d a1.o

a1.o:     file format elf64-x86-64


Disassembly of section .text:

0000000000000000 <m1>:
   0:	55                   	push   %rbp
   1:	48 89 e5             	mov    %rsp,%rbp
   4:	48 83 ec 10          	sub    $0x10,%rsp
   8:	48 c7 45 f8 00 00 00 	movq   $0x0,-0x8(%rbp)
   f:	00
  10:	48 c7 45 f0 00 00 00 	movq   $0x0,-0x10(%rbp)
  17:	00
  18:	48 8b 55 f0          	mov    -0x10(%rbp),%rdx
  1c:	48 8b 45 f8          	mov    -0x8(%rbp),%rax
  20:	48 89 c6             	mov    %rax,%rsi
  23:	bf 00 00 00 00       	mov    $0x0,%edi
  28:	b8 00 00 00 00       	mov    $0x0,%eax
  2d:	e8 00 00 00 00       	callq  32 <m1+0x32>
  32:	c9                   	leaveq
  33:	c3                   	retq
% nm a1.o
0000000000000000 T m1
                 U printf

```

### inline functions are not always inlined

```c
// in a1.c
#include <stdio.h>
#include "foo.h"

void m1()
{
    foo("from a1", (void*) foo);
}
```

```c
// in a2.c
#include <stdio.h>
#include "foo.h"

void m2()
{
    foo("from a2", (void*) foo);
}
```

```console
sh build.sh
% gcc -O3 -c -o a1.o a1.c
% gcc -O3 -c -o a2.o a2.c
% gcc -O3 -c -o main.o main.c
% gcc -o a.out a1.o a2.o main.o
% ./a.out
from a1: pointer is 0x400530
from a1: pointer is 0x400570
% objdump -d a1.o

a1.o:     file format elf64-x86-64


Disassembly of section .text:

0000000000000000 <foo>:
   0:	48 89 f2             	mov    %rsi,%rdx
   3:	31 c0                	xor    %eax,%eax
   5:	48 89 fe             	mov    %rdi,%rsi
   8:	bf 00 00 00 00       	mov    $0x0,%edi
   d:	e9 00 00 00 00       	jmpq   12 <foo+0x12>
  12:	66 66 66 66 66 2e 0f 	data16 data16 data16 data16 nopw %cs:0x0(%rax,%rax,1)
  19:	1f 84 00 00 00 00 00

0000000000000020 <m1>:
  20:	ba 00 00 00 00       	mov    $0x0,%edx
  25:	be 00 00 00 00       	mov    $0x0,%esi
  2a:	bf 00 00 00 00       	mov    $0x0,%edi
  2f:	31 c0                	xor    %eax,%eax
  31:	e9 00 00 00 00       	jmpq   36 <m1+0x16>
%  nm a1.o
0000000000000000 t foo
0000000000000020 T m1
                 U printf
```

we can see that the function `foo` is inlined, but the `foo` object
code is emitted by compiler, because the address of the function `foo`
is used.

## Pitfall of static inline function

As same as other static function, every transform unit has its own
implementation, so that `foo` is the local function. `t` in `nm`
output indicates that `foo` is a private symbol.

The address of function `foo` is `0x400530` and `0x400570` for `a1.o`
and `a2.o` respectively.

## non-external inline function.


```c
#pragma once
// in foo.h
inline __attribute__((always_inline))
void foo(const char * msg, void * f)
{
   printf("%s: pointer is %p\n",msg,f);
}
```

```c
// in a1.c
#include <stdio.h>
#include "foo.h"

void m1()
{
    foo("from a1", (void*) NULL);
}
```

```c
// in a2.c
#include <stdio.h>
#include "foo.h"

void m2()
{
    foo("from a1", (void*) NULL);
}
```

```console
% gcc -O3 -c -o a1.o a1.c
% gcc -O3 -c -o a2.o a2.c
% gcc -O3 -c -o main.o main.c
% gcc -o a.out a1.o a2.o main.o
a2.o: In function `foo':
a2.c:(.text+0x0): multiple definition of `foo'
a1.o:a1.c:(.text+0x0): first defined here
collect2: error: ld returned 1 exit status
% nm a1.o
0000000000000000 T foo
0000000000000020 T m1
                 U printf
% nm a2.o
0000000000000000 T foo
0000000000000020 T m1
                 U printf
```

We can see both `a1.o` and `a2.o` emits function definition of `foo`,
so that the linker complains that `multile definition`.

It means that the non-external inline function can only be used in one
transform unit. This is rather rather limited use, it could be replace
with `static inline` functions mentioned above.



## extern inline functions

```c
#pragma once
// in foo.h
extern inline
void foo(const char * msg, void * f)
{
   printf("%s: pointer is %p\n",msg,f);
}
```

```c
// in a1.c
#include <stdio.h>
#include "foo.h"

void m1()
{
    foo("from a1", (void*) NULL);
}
```

```c
// in a2.c
#include <stdio.h>
#include "foo.h"

void m2()
{
    foo("from a1", (void*) NULL);
}
```

```console
% gcc -O3 -c -o a1.o a1.c
% gcc -O3 -c -o a2.o a2.c
% gcc -O3 -c -o main.o main.c
% gcc -o a.out a1.o a2.o main.o
% ./a.out
from a1: pointer is (nil)
from a1: pointer is (nil)
% objdump -d a1.o

a1.o:     file format elf64-x86-64


Disassembly of section .text:

0000000000000000 <m1>:
   0:	31 d2                	xor    %edx,%edx
   2:	be 00 00 00 00       	mov    $0x0,%esi
   7:	bf 00 00 00 00       	mov    $0x0,%edi
   c:	31 c0                	xor    %eax,%eax
   e:	e9 00 00 00 00       	jmpq   13 <m1+0x13>
% nm a1.o
0000000000000000 T m1
                 U printf
```

We can see `foo` is inlined, because of `-O3`, and no code object is
emitted.

But with the same source code but different compilation options,
i.e. `-O0`, which disable inline optimization, we've got a linking
error as below.


```console
% gcc -O0 -c -o a1.o a1.c
% gcc -O0 -c -o a2.o a2.c
% gcc -O0 -c -o main.o main.c
% gcc -o a.out a1.o a2.o main.o
a1.o: In function `m1':
a1.c:(.text+0xf): undefined reference to `foo'
a2.o: In function `m2':
a2.c:(.text+0xf): undefined reference to `foo'
collect2: error: ld returned 1 exit status
% objdump -d a1.o

a1.o:     file format elf64-x86-64


Disassembly of section .text:

0000000000000000 <m1>:
   0:	55                   	push   %rbp
   1:	48 89 e5             	mov    %rsp,%rbp
   4:	be 00 00 00 00       	mov    $0x0,%esi
   9:	bf 00 00 00 00       	mov    $0x0,%edi
   e:	e8 00 00 00 00       	callq  13 <m1+0x13>
  13:	5d                   	pop    %rbp
  14:	c3                   	retq
% nm a1.o
                 U foo
0000000000000000 T m1
```

Because `foo` is not inlined, and both `a1.o` and `a2.o` refer to and
undefined reference to `foo`, but there is no code object for `foo` is
emitted, so that there is the linking error, `undefined reference`.

Stand-alone object code is never emitted.

In order to fix the above error, we use non-external inline function
in `a1.c`

```c
// in a1.c
#include <stdio.h>
#include "foo.h"

void m1()
{
    foo("from a1", (void*) NULL);
}

inline void foo(const char * msg, void * f);
```

```console
% gcc -O0 -c -o a1.o a1.c
% gcc -O0 -c -o a2.o a2.c
% gcc -O0 -c -o main.o main.c
% gcc -o a.out a1.o a2.o main.o
% ./a.out
from a1: pointer is (nil)
from a1: pointer is (nil)
% nm a1.o
0000000000000000 T foo
000000000000002c T m1
                 U printf

% nm a2.o
                 U foo
0000000000000000 T m2
```

We can see in `a2.o`, it is as same as before, `foo` is an external
symbol. But in `a1.c` it is a public symbol, and `foo` should only be
emitted by `a1.o`, otherwise it would have linking error `multiple definition`.


### function address

If we change `a1.c` and `a2.c` as below, to print the address of function `foo`.

```c
// in a1.c
#include <stdio.h>
#include "foo.h"

void m1()
{
    foo("from a1", (void*) foo);
}

inline void foo(const char * msg, void * f);

// in a2.c
#include <stdio.h>
#include "foo.h"

void m2()
{
    foo("from a1", (void*) foo);
}
```



```console
% gcc -O0 -c -o a1.o a1.c
% gcc -O0 -c -o a2.o a2.c
% gcc -O0 -c -o main.o main.c
%  gcc -o a.out a1.o a2.o main.o
%  ./a.out
from a1: pointer is 0x400506
from a1: pointer is 0x400506
%  nm a1.o
0000000000000000 T foo
000000000000002c T m1
                 U printf
%  nm a2.o
                 U foo
0000000000000000 T m2
```

We see the address of function `foo` is unique, i.e. `0x400506`.

### what if inline functions have with difinitions?

```c
// in a1.c
#include <stdio.h>
extern inline
void foo(const char * msg, void * f)
{
   printf("foo in a1.c %s: pointer is %p\n",msg,f);
}

void m1()
{
    foo("from a1", (void*) foo);
}

inline void foo(const char * msg, void * f);
// in a2.c
#include <stdio.h>
extern inline
void foo(const char * msg, void * f)
{
   printf("foo in a2.c %s: pointer is %p\n",msg,f);
}
void m2()
{
    foo("from a2", (void*) foo);
}
```


```console
%  gcc -O0 -c -o a1.o a1.c
%  gcc -O0 -c -o a2.o a2.c
%  gcc -O0 -c -o main.o main.c
%  gcc -o a.out a1.o a2.o main.o
%  ./a.out
foo in a1.c from a1: pointer is 0x400506
foo in a1.c from a2: pointer is 0x400506
%  nm a1.o
0000000000000000 T foo
000000000000002c T m1
                 U printf
%  nm a2.o
                 U foo
0000000000000000 T m2
```

We notice that we didn't include the common definition of `foo` from
`foo.h`, instead, `a1.c` and `a2.c` has its own definitions.

Because of `-O0`, the function is not inlined, so that only `foo`
defined in `a1.c` is used.

But if we compile it with `-O3`.

```console
%  gcc -O3 -c -o a1.o a1.c
%  gcc -O3 -c -o a2.o a2.c
%  gcc -O3 -c -o main.o main.c
%  gcc -o a.out a1.o a2.o main.o
%  ./a.out
foo in a1.c from a1: pointer is 0x400530
foo in a2.c from a2: pointer is 0x400530
%  nm a1.o
0000000000000000 T foo
0000000000000020 T m1
                 U printf
%  nm a2.o
                 U foo
0000000000000000 T m2
                 U printf
```

Because `foo` is inlined, `a2.c` uses the definition in `a2.c`, not
the `foo` in `a1.c`.

Don't do it in practice.

But in practice it might happen. For example, you modify `foo` in
`foo.h`, but you only compile `a1.c` and forgot to re-compile
`a2.c`. If the function is inlined, `a2.c` still use the old
definition of `foo`.
