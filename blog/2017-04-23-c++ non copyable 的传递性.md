---
layout: post
title:  "c++ non copyable 的传递性"
date:   2017/04/23 15:31:56
categories:
comments: true
draft: false
---


什么是 non copyable 的类

标准中有些类很有意思，是 non-copyable 的，不可拷贝的。 例如 `unique_ptr`, `mutex`, `lock_guard` 。
这些类的拷贝构造函数标记称为删除了。


例如 [lock_guard](http://en.cppreference.com/w/cpp/thread/lock_guard/lock_guard)

```
lock_guard( const lock_guard& ) = delete;	(3) 	(since C++11)
```

这些 non-copyable 的类，他们的对象中，无法有 copy 的语义。于是，通常来说，他们的赋值操作符重载也被标记为删除了。

```
... operator=(...) deleted; // not copy-assignable

```


这些语义上就是不可拷贝的对象，在实际中有很多应用。


这种语义是有传递性的，也就是说，如果一个类的成语变量是一个 non-copyable 的，那么这个类本身也应该是一个 non-copyable 的。例如

```cpp
#include <iostream>
#include <memory>
using namespace std;


class Foo {
  public:
  private:
    unique_ptr<int> pInt_;
};


int main(int argc, char *argv[])
{
    Foo a;
    Foo b{a};

    return 0;
}
```

这个代码会有编译错误

```
simple_non_copyable.cpp:16:9: error: call to implicitly-deleted copy constructor of 'Foo'
    Foo b{a};
        ^~~~
simple_non_copyable.cpp:9:21: note: copy constructor of 'Foo' is implicitly deleted because field 'pInt_' has a deleted copy constructor
    unique_ptr<int> pInt_;
                    ^
```

解决这个问题的方法就是，不能使用拷贝构造函数，c++ 在防止作者出现这种设计上的语义错误。
