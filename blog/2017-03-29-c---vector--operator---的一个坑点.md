---
layout: post
title:  "c++ vector::operator[] 的一个坑点"
date:   2017/03/29 14:23:22
categories:
comments: true
---


```include
quote cpp blog/src/vector_reference_1.cpp
```

上面的代码输出是

```
before change r1: 100
after change r1: 100 100
```

居然 `r1.value = 999` 没有起作用。如果我们添加代码 `v.reserver(10)`

输出结果是

```
before change r1: 100
after change r1: 999 100
```

为什么呢？

我们增加一些调试信息

```include
quote cpp blog/src/vector_reference_2.cpp
```

输出结果是

```
vector_reference_2.cpp:8: [Foo] this 0x7f9808d00000
 100
vector_reference_2.cpp:8: [Foo] this 0x7f9808d00014
vector_reference_2.cpp:14: [Foo] this 0x7f9808d00010 &other 0x7f9808d00000
vector_reference_2.cpp:21: [~Foo] this 0x7f9808d00000
vector_reference_2.cpp:43: [main] &r1 0x7f9808d00000
 100 100
vector_reference_2.cpp:21: [~Foo] this 0x7f9808d00014
vector_reference_2.cpp:21: [~Foo] this 0x7f9808d00010
```

我们看到， `v.emplace_back()` 会调用拷贝构造函数，而重新从 `0x7f9808d00000`
构造一个新的对象 `0x7f9808d00010`，然后析构掉原来的对象 `0x7f9808d00000`。

而原来得到的引用 `r1` 还引用原来的对象  `0x7f9808d00000` ，一个已经析构掉的对象，类似悬空指针了。

这是因为 vector 变大的时候，发现内存不够了，就需要申请新的内存，然后把原有的对象都 copy 到新的地址上去。

`vector::reserve` 避免了这个问题，但是这是假象，代码依然有 bug ，只不过被 reserve 给隐藏掉了。因为内存够用，所以就不会申请新的内存。
