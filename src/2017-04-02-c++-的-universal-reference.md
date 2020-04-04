---
layout: post
title:  "c++ 的 universal reference"
date:   2017/04/02 18:44:14
categories:
comments: true
---


估计很多人都没有听说过 universal reference ，这个不奇怪，因为是 Scott
Meyers 自己创造的这个术语，参考
[https://isocpp.org/blog/2012/11/universal-references-in-c11-scott-meyers]()
。 C++ 标准里面没有这个术语，标准术语是 `Reference collapsing` 。但是
这种解释是更难理解，尽管更加准确。

[Herb Sutter](https://herbsutter.com) 同意这个东西应该有一个名字，但是应该叫做 forwarding reference。
因为 universal reference 似乎建议这个东西是更加通用的东西，到处都可以用，其实不是这样。 forwarding reference 则强调
这个东西只有在做 perfect forwarding 的时候才用。 关于 perfect forwarding ，[看这里](2015-07-08-C-----的右值引用问题.html)


我理解 scott meyers 的文章的意思只是指当看起来像右值引用的 `T&&` 出现在 template 的中，他就是
`universal reference` ，既不是 `lvalue reference` 也不是 `rvalue reference` 。

universal reference 的实际效果就是，你给他一个 lvalue reference 的时候，他就是 lvalue reference ，你给他 rvalue reference 的时候，他就是  rvalue reference 。


看下面的例子

```{.cpp include=blog/2017-04-02/cpp_universal_reference_1.cpp}
```

输出如下：

```{.plain include=blog/2017-04-02/cpp_universal_reference_1.out}
```


这里需要注意，尽管 `rf` 的值是一个 rvalue reference ，但是他本身是一个 lvalue 。参考 [C++11 的右值引用问题](2015-07-08-C-----的右值引用问题.html)
