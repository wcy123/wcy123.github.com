---
layout: post
title:  "Walter E. Brown 讲解 c++ 中的 metaprograming"
date:   2017/04/30 22:29:23
categories:
comments: true
---


今天看了 Walter E. Brown 的[youtube 视频](https://www.youtube.com/watch?v=a0FliKwcwXE)，介绍 c++ 中的 metaprogramming 受益非浅。 这里记录一下学到的内容。


我已经熟悉如何使用 `enable_if` , `is_same` ，`conditional` 等等。但是从来没有想过的如何自己动手实现这些功能，觉得实现这些功能，一定非常非常难。 Brown 的视频启发我，鼓励我，去实现这些功能，没有想象中的那么难，只是因为我们不熟悉 (unfimiliar)模板编程。


## `true_type` 和 `false_type`

这两个定义十分简单。很多简单的东西，看上去显而易见，第一感觉是这个东西没啥用，后来慢慢体会到，这些东西十分有用。就像阿拉伯数字 `0` 。

```
struct true_type {
    static constexpr bool value = true;
};
struct false_type {
    static constexpr bool value = false;
};
```

## `is_same` 判断两个类是否相等

首先定义主干模板 (primary template)

```cpp
template<class T, class U>
struct is_same : public false_type {
};
```

定义一个模板，`is_same<T,U>` ，默认情况下，这两个类不相同。


然后部分特例化 (partial specialization)

```cpp
template<class T>
struct is_same<T,T> : public true_type {
};
```

完整的例子如下 。

```include
quote cpp cpp_src/is_same_0.cpp
```

输出


```include
quote plain cpp_src/is_same_0.out
```


这个例子中，我们看到模板编程的一个编程模式

 1. 定义 primary template 。
 2. 然后 partial specialization 。

很多时候，我们都是遵循这个模式。 primary template 类似定义接口，表明我们的模板看起来像个什么样子。 有的时候，顺便定义默认实现。 第二步，我们定义什么时候，和默认定义不一样。



## `is_void` 查看一个类型是否是 `void`

`is_void<T>::value` 是 true ，如果 `T` 是  `void` ， `void const` ，`void const volatile` 或者是  `void const volatile` 。


首先定义 primary template .

```cpp
template<class T>
struct is_void : public false_type {
};
```

默认 `T` 不是 `void` 。

然后，我们做 partial specialization 。

```cpp
// partial specialization.
template<>
struct is_void<void> : public true_type {
};
template<>
struct is_void<void const> : public true_type {
};
template<>
struct is_void<void const volatile> : public true_type {
};
template<>
struct is_void<void volatile> : public true_type {
};
```

这个看起来有点傻，但是可读性很好，几乎就是把我们的需求重新用 cpp 语言描述一遍。后面会有一个改进版本。


完整代码

```include
quote cpp cpp_src/is_void_0.cpp
```

程序输出

```include
quote plain cpp_src/is_void_0.out
```

# `remove_cv` 删除一个类的 const volatile 修饰

`remove_cv<void>`, `remove_cv<void const>` ,`remove_cv<void volatile>` 和 `remove_cv<void const volatile>` 都是 `void` 。

定义 primary template

```cpp
// partial specialization
template<class T>
struct remove_cv<T const> {
    using type = T;
};
```

定义 partial specialization

```cpp
// partial specialization
template<class T>
struct remove_cv<T const> {
    using type = T;
};
template<class T>
struct remove_cv<T volatile> {
    using type = T;
};
template<class T>
struct remove_cv<T volatile const> {
    using type = T;
};
```

这里通常用 `type` 来表示应用一个 template 之后的结果。


完整代码

```include
quote cpp cpp_src/remove_cv_0.cpp
```

程序输出

```include
quote plain cpp_src/remove_cv_0.out
```

## 重写 `remove_cv`

我们可以写简化一下 `remove_cv` ，这里的 "简化" ，我理解不是“变得更简单" 的意思。而是让他变得更加 “可组合的”。

这个比较简单，直接上代码了。这里不停地重复一个模式，就是上面谈到的模式。


完整代码

```include
quote cpp cpp_src/remove_cv_1.cpp
```

程序输出

```include
quote plain cpp_src/remove_cv_1.out
```


## 重写 `is_void`


有了 `is_same` 和 `remove_cv` ，我们可以简化 `is_void` 的实现。

```cpp
// primary defination.
template<class T>
struct is_void : public is_same<typename remove_cv<T>::type, void> {
};
```

完整代码

```include
quote cpp cpp_src/is_void_1.cpp
```

程序输出

```include
quote plain cpp_src/is_void_1.out
```

# `is_one_of` 判断一个类是否是某些类中的一个


 1. `is_one_of<int>::value` 是  `false` 。
 1. `is_one_of<int,int>::value` 是  `true` 。
 2. `is_one_of<int, float, int>::value` 是  `true` 。
 3. `is_one_of<int, float, double>::value` 是  `false` 。
 4. `is_one_of<int, float, double, int>::value` 是  `true` 。

这里使用了 c++11 中的可变长模板的特性，但是基本的模板编程模式是不变的。

定义  primary template

```cpp
// primary defination.
template<class T, class... P0toN>
struct is_one_of : public false_type {
};

```

定义 partial specialization

```cpp
// partial specialization.
template<class T, class... P1toN>
struct is_one_of<T,T, P1toN...> : public true_type {
};
```

这个是显而易见的，如果和列表中的第一个类型相同，那么就是 `true_type` 。


如果匹配失败呢，那么我们有一种递归消减的模式。

```cpp
template<class T, class U, class... P1toN>
struct is_one_of<T, U, P1toN...> : public is_one_of<T, P1toN...> {
};
```

或者

```cpp
template<class T, class U, class... P1toN>
struct is_one_of<T, U, P1toN...> {
    static constexpr bool value = is_one_of<T, P1toN...>::value;
};
```

我不确定哪一种风格是好的。



完整代码

```include
quote cpp cpp_src/is_one_of_0.cpp
```

程序输出

```include
quote plain cpp_src/is_one_of_0.out
```



# `is_copy_assignable` 判断一个类是否可复制，可赋值


如果 `T` 定义了拷贝构造函数和赋值操作符重载，那么 `is_copy_assignable<T>::value` 是 `true` ，否则就是  `false` 。举几个 `false` 的例子

 1. `is_copy_assignable<unique_ptr<int> >::value`
 2. `is_copy_assignable<mutex >::value`
 3. `is_copy_assignable<lock_guard<mutex> >::value`


这个实现起来是很有难度的。我们从简单的事情做起。第一步，写 primary template 。


```cpp
template<typename T>
struct is_copy_assignable : true_type {
}
```

通常，起名字是程序员最头痛的事情，如果你不引用一个东西，就不要给他起名字。例如上面的例子，`U` 就是没有用的。于是，我们可以这样写。
