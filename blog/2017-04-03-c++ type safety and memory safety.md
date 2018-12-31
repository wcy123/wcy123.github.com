---
layout: post
title:  "c++ type safety and memory safety"
date:   2017/04/03 19:42:36
categories:
comments: true
draft: true
---


笔记 [youtube](https://www.youtube.com/watch?v=hEx5DNLWGgA)


* type safety ：不要使用 union ，用 variant

* bound safety ：不要越界访问

* lifetime safety： 不要访问已经析构了对象


# type safety

* 不要使用 `reinterpret_cast`
* 不要使用 `static_cast` 做 downcasts ，用 `dynamic_cast` 。其实这个都应该尽量不用
* 不要使用 `const_cast`
* 不用使用 C style cast ， 例如 `(int) X`;
* 不要使用没有初始化过的局部变量
* 成员变量都要初始化
* 不要访问 raw union 的成员变量，不要使用 union，使用 variant
* 不要使用可变长参数，类似 `printf` ，而是使用可变长参数的模板，类似`std::forward`

什么是 downcasts

```cpp
class D : public B {};
B * pB = ....
D * pD = static_cast<D*>(bObj)
```

# bound safety

* 不要使用 raw pointer arithmetic ， 使用 `array_view` GSL
* 不要使用数组下标
* 不要使用 array to pointer decay 。 C++ 中数组会自动退化称为指针。


关于数组和指针的区别，C 论坛上每次都有这样的问题。我们抛开 C 不谈，如果在 C++ 的领域，我们只用简单的记住，不要用数组。在  C++ 的上下文中，有很多关于数组退化为指针的特殊情况，例如 template 的类型推导，函数的 overload 等等。这些我也搞不清楚细节，但是我认为，这些都是 C++ 中为了兼容  C 留下的不好的遗产，尽量不要用。


# lifetime safety

1. 记得释放从 heap 里面申请的内存
2. 记得只释放一次
3. 不要访问已经释放过的内存


智能指针（`shared_ptr`, `unique_ptr`) 用来管理所有权的问题，ownership 。

裸指针和引用还是十分有用的，在不涉及所有权的时候。

1. 用 `make_shared` 和 `make_unique` 来构造对象
2. 
