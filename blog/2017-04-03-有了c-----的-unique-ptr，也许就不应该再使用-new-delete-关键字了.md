---
layout: post
title:  "有了c++11 的 unique_ptr，也许就不应该再使用 new/delete 关键字了"
date:   2017/04/03 18:35:58
categories:
comments: true
---


```cpp
Widget* BuildWidget() {
    Widget * ret = new SomeWidget();
    ...
    return ret;
}
```

上面是以前 c++ 经常的风格。有了 c++11 的 `unique_ptr` 或许我们应该这么写

```cpp
std::unique_ptr<Widget> BuildWidget() {
    std::unique_ptr<Widget>  ret = std::make_unique<Widget>(a1,a2);
    ...
    return ret;
}
```


也许有人担心性能，其实 `unique_ptr` 和一个 raw pointer 是同样的大小。这么做几乎没有任何额外开销。

这样做的最大好处是明确所有权。 C++ 区别于其他语言的重要特点就是资源管理。RAII
Resource acquisition is initialization 。 于是 new 了一个对象，谁拥有这个对象就变得十分重要了。
返回 `unique_ptr` 明确的说明，所有权转移给了调用者。这里指释放资源，资源不仅仅是内存。

就算调用者拒绝拥有这个对象，也不会产生资源泄露

```cpp
std::unique_ptr<Widget> pWidget = BuildWidget();
```

当 `pWidget` 被析构的是时候，会自动释放资源。


很可惜，`make_unique` 不在  c++11 中，我们可以很容易自己写一个。

```cpp
template<typename T, typename ...Args>
std::unique_ptr<T> make_unique(Args &&...args) {
    return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
}
```

这么做还有一个好处，就是针对异常友好。就是说在程序某处 throw exception 的时候，不会产生资源泄露。


`std::unique_ptr` 也可以放在成员变量中。例如

```cpp
class A {
...
std::unique_ptr<B> pB;
...
};
```

和

```cpp
class A {
...
B* pB;
...
};
```

相比之下，`unique_ptr` 当然是有好处的。

还有一种方案，就是A 对象直接拥有一个 b 对象，而不是指针，例如下面这种情况，或许更合适。


```cpp
class A {
...
B b;
...
};
```
