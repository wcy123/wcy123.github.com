---
layout: post
title:  "c++ lambda capture by value 的实验"
date:   2017/04/02 17:38:53
categories:
comments: true
---


[c++ 中 lambda 的类型和大小](2017-03-26-c---中-lambda-的类型和大小.html) 中可以看
到 lambda 生成一个匿名类。当没有 capture 任何变量的时候，大小几乎是 0
。


c++11 中的 lambda 关键字对于程序的结构影响很大，其中的闭包是一个很关键
的概念。因为 c++ 语言本身不支持垃圾回收，所以 capture by value 和其他
语言还是有区别的。关键的问题在于，拷贝构造函数调用多少次？



例如代码

```{.cpp include=cpp_src/cpp_lambda_capture_1.cpp}
```

程序的输出如下

```{.plain include=cpp_src/cpp_lambda_capture_1.out}
```


我们仔细看输出，可以看到一下几点

1. 生成的匿名 lambda 类，内部有一个不可见的成员变量 `Foo` 。
2. 匿名 lambda 对象赋值的时候，会调用内部成员变量变量的 `Foo` 的拷贝构造函数。
3. 匿名 lambda 对象析构的时候，会调用内部成员变量的析构函数，析构掉这个不可见的成员变量。


我们看看生成对象的大小

例如代码

```{.cpp include=cpp_src/cpp_lambda_capture_2.cpp}
```

程序的输出如下

```{.plain include=cpp_src/cpp_lambda_capture_2.out}
```

可以看到，对象的大小就是 4 字节 ，和 Foo 本身的对象是一样大的。这说明

1. 除了抓住的对象的内存，匿名 lambda 类本身不占用额外的内存
2. 匿名 lambda 类没有虚函数表


这里注意到一点

```cpp
std::function<void(void)> f = [](){...};
auto lambda = [](){...};

```

这两个是不一样的， `f` 是一个 `std::function` ，内部可以包含一个
lambda 对象。会调用 lambda 对象的拷贝构造函数，构造一个新的 lambda 对
象。于是，capture by value 的 lambda 对象，也会调用每一个 capture 对象
的拷贝构造函数。
