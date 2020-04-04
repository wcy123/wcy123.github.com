#  "c++ GSL 中的 owner"


`owner` 是一个底层机制，用于标记一个指针的是所指对象的所有者。


```cpp
foo(owner<int*> p) {
    delete p; // OK
}
```

正常来讲，用户层面的代码不会使用这个 owner ，这个东西主要为了静态分析工具来分析资源泄露。


在 [GSL](https://github.com/Microsoft/GSL.git) 中， owner 的定义很简单。

```cpp
template <class T>
using owner = T;
```


这里用了 c++11 的一个新功能, `using` 做 type 别名。

Effective Modern C++ 中建议尽量使用 using 来替代 typedef 。

主要有两个好处

1. 支持模板。
2. 更加准确


为什么说更加准确？例如

```cpp
template<typename T>
struct owner {
    typedef T type;
};
```

这个看起来是不错的。但是使用的时候，需要 `owner<int*>::type p` ，这样做是有坑的。
没有人阻止客户可以写下下面的代码

```cpp
template<>
struct owner<int*> {
   typedef float* type;
};
template<>
struct owner<float*> {
};
```

第一种情况是，完全定义了一个不相关的类型， `owner<int*>::type` 其实是 `float*`。

第二种情况是，完全没有 `type` 这个定义，会产生编译错误，抱怨
`owner<float*>` 在实例化的过程中，`::type` 没有定义。


回到这个 owner 上，我们看到，`owner<T*>` 其实就是 `T*` 的别名而已。很简单，很巧妙的写法。


但是，这个技巧不是给普通应用程序的代码使用的，而是给静态分析工具使用的。
