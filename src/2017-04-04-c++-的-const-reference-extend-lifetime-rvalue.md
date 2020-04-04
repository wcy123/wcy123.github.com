---
layout: post
title:  "c++ 的 const reference extend lifetime rvalue"
date:   2017/04/04 13:13:05
categories:
comments: true
---



c++ 中，如果一个函数返回一个对象，那么这个对象是如果返回的呢？

本文用 OSX 下的 llvm 来汇编理解这个定义。

```{.cpp include=blog/2017-04-04/life_expansion.cpp}
```


我们看一下，产生的汇编


```
c++ -O3 -fno-inline -std=c++11 -o a.out life_expansion.cpp
otool -tV a.out | c++filt
```

截取其中的汇编代码

```asm
_main:
        pushq   %rbp
        movq    %rsp, %rbp
        pushq   %rbx
        pushq   %rax
        leaq    0x869(%rip), %rdi ## literal pool for: "before"
        callq   0x100000d10 ## symbol stub for: _puts # 这里调用 printf
        leaq    -0x10(%rbp), %rbx                     # 这里申请临时变量的内存
        movq    %rbx, %rdi
        callq   calc() ## calc()
        movq    %rbx, %rdi
        callq   show(Foo const&) ## show(Foo const&)
        movq    %rbx, %rdi
        callq   Foo::~Foo() ## Foo::~Foo()
        leaq    0x848(%rip), %rdi ## literal pool for: "after"
        callq   0x100000d10 ## symbol stub for: _puts
        xorl    %eax, %eax
        addq    $0x8, %rsp
        popq    %rbx
        popq    %rbp
        retq
        nopw    %cs:(%rax,%rax)
```


```asm
calc():
        pushq   %rbp
        movq    %rsp, %rbp
        pushq   %rbx
        pushq   %rax
        movq    %rdi, %rbx
        movl    $0x1, %esi
        callq   Foo::Foo(int) ## Foo::Foo(int)
        movq    %rbx, %rax
        addq    $0x8, %rsp
        popq    %rbx
        popq    %rbp
        retq
        nopl    (%rax)
```

这里可以看到，对于 `calc` 的返回值，实际分配内存是在调用者的堆栈里申请的。

而如果调用者，用一个 const reference 抓住（引用） 这个变量的话，那么这个临时变量的生命周期会被拉长。

他的生命周期拉长到和 const reference 的生命周期一样。

详见 [http://www.open-std.org/jtc1/sc22/wg21/docs/papers/1993/N0345.pdf]()
