---
layout: post
title:  "c++11 中的 uniform initialization"
date:   2017/03/29 14:47:49
categories:
comments: true
---

stackoverflow 上的讨论 [nitializer_list and default constructor overload resolution](http://stackoverflow.com/questions/9020513/initializer-list-and-default-constructor-overload-resolution)

```{.cpp include=blog/2017-03-29/vector_init.cpp}
```

最常见的疑惑是构造 `vector` 。

- `vector v1{}` 调用默认构造函数 `vector::vector()`。
- `vector v({})` 调用构造函数 `vector(std::initializer_list<T> init)`。
- `vector v{{}}` 调用构造函数 `vector(std::initializer_list<T> init)`。
- `vector v2{1,2}` 调用构造函数 `vector(std::initializer_list<T> init)`
- `vector v3(3,10)` 调用的 `vector( size_type count, const T& value)`
- `vector<int> v4({3,10})` 调用构造函数 `vector(std::initializer_list<T> init)`
- `vector<int> v5{{3,10}}` 调用构造函数 `vector(std::initializer_list<T> init)`

vector 的这个设计十分糟糕。我理解是为了兼容历史版本。

这么乱，既然我们自己都容易搞乱，所以一定慎重使用 overload 这个功能。



```
v1 =
v2 = 1 2
v3 = 10 10 10
v4 = 3 10
v5 = 3 10
```
