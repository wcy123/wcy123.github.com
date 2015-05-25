---
layout: post
title:  "C++ virtual function"
date:   2015/05/20 09:47:56
categories: 
comments: true
---

How C++ virtual function is implemented? Compilers have their own
implementations. I am interested to see the implementation of `gcc`.


```shell
$ gcc --version
gcc (Debian 4.7.2-5) 4.7.2
Copyright (C) 2012 Free Software Foundation, Inc.
This is free software; see the source for copying conditions.  There is NO
warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
```

This is a simple example.

```c
class B {
public:
    virtual int foo1() = 0;
    virtual int foo2() = 0;
    virtual int foo3() = 0;
};

int bar(B * obj)
{
    return obj->foo3();
}
```


Here is the output of the assemble code.

```
$ gcc -O3 -S -c -o - vf.cc
        .file   "vf.cc"
        .text
        .p2align 4,,15
        .globl  _Z3barP1B
        .type   _Z3barP1B, @function
_Z3barP1B:
.LFB0:
        .cfi_startproc
        movq    (%rdi), %rax
        movq    16(%rax), %rax
        jmp     *%rax
        .cfi_endproc
.LFE0:
        .size   _Z3barP1B, .-_Z3barP1B
        .ident  "GCC: (Debian 4.7.2-5) 4.7.2"
        .section        .note.GNU-stack,"",@progbits
```


The interesting part is

```asm
        movq    (%rdi), %rax
        movq    16(%rax), %rax
        jmp     *%rax
```

We know `%rdi` is the first function argument, and it is `this`
pointer. I guess the very first 8 bytes it is pointed to are the
pointer of `vtable`, so firstly, we load the `vtable` into
`%rax`. `gcc` knows that `foo3` is the third virtual function, so that
the offset of `foo3` is `16`, and we load the virtual function pointer
`foo3` into `%rax`. Finally, jumping to the virtual function.

