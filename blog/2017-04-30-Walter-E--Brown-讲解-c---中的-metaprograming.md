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

恩，这个看起来很简单，不是吗？实例化的部分就有难度了。

首先，我们定义一个简单的东西，`declval`

```cpp
template<typename T>
T declval();
```

声明一个模板函数，`declval<T>()` 的返回值的类型是 `T` 。这个函数没有定义，我们无法运行期调用这个函数。

编译期，我们可以使用 `decltype(declval<T>())` 得到类型 `T` 。 `decltype` 是 c++11 的特性。


如果一个类定义了赋值操作符重载，那么下面的表达式就是成立的。

```cpp
decltype( declval<U&>() = declval<U const&>() )
```

`declval<U const&>()` 返回一个 `U const&` 的对象 `x`，`declval<U&>()` 返回一个 `U&` 的对象 `y` ，然后试图调用赋值语句，`x=y` 。

等等，这里我们并没有真正的执行求值(evaluate) 的动作，无论是编译期还是运行期。`decltype` 就像 `sizeof`, `noexcept` 和 `typeid` 一样，并不真正执行求值的动作。一切发生在想象中。

无论如何，如果 `U` 没有定义 `=` 操作符重载的话，上面的语句就会失败(failure) ，注意，我没有使用出错(error) 这个表达方式。 SFINAE (Substitue Failure Is Not An Error) 。就是说，如果上面的表达式是不合法的 (ill-formed) ，那么编译器不认为是错误，继续尝试匹配其他的表达式。

```cpp
template<typename T>
struct is_copy_assignable {
  private:
    template<class U, class = decltype( declval<U&>() = declval<U const&>() )>
    static true_type try_assignment(U&& );

    static false_type try_assignment(...);
  public:
    using type = decltype( try_assignment(declval<T>()));
    static constexpr bool value = type::value;
};
```

这里在多余解释一些东西。

通常，起名字是程序员最头痛的事情，如果你不引用一个东西，就不要给他起名字。例如上面的例子，`class = ...` 就没有起名字，尽管没有名字，这个模板参数可以有一个默认值。默认值就是那一长串 `decltype( declval<U&>() = declval<U const&>()` 。


```cpp
static false_type try_assignment(...);
```

这个是一个十分不常用的语法，`...` ，来自于 C 语言的历史遗产，`printf(...)` 。 这个在 C++ 中极力不提倡使用，推荐使用可变长模板。原因是这个没有 type safe ，多少 c 语言的 bug 倒在这个上面。 这里使用了 C++ 中，函数重载 (overload) 的一个不常用的特性，可变长的参数，在函数重载(overload) 中，是最后一个选项。只有其他匹配不成功的时候，才会匹配这个函数。


把两个重载的 `try_assigment` 连起来看，我们理解一下 SFINAE 。如果 `U` 定义了 `=` 操作符重载，那么  `decltype( declval<U&>() = declval<U const&>()` 就是一个合理的表达式，匹配成功，于是 `try_assigment()` 的返回值的类型是 `true_type` 。否则，匹配失败，但是失败不是错误，继续匹配，匹配到了第二个 `try_assigment`，这时，返回值的类型是  `false_type` 。

如果我们在重复利用 `delctype` 和  `declval` 的技巧，就可以得到 `type` 的定义。

```cpp
using type = decltype( try_assignment(declval<T>()));
```

后面的 `static constexpr bool value` 就不难理解了。


这段代码极其不好理解，因为看起来十分的奇怪。这也是为什么 template 让大家觉得陌生。


这段代码并不是最好的，因为用到太多的小技巧，后面 Walter E. Brown 会试图重写一下。

有几个技术是值得学习的。 `decltype` ，这个十分有用。`decltype` 很独特，因为他的参数是一个表达式，而不是一个 type 。 为了配合使用 `decltype` ，我们才会引入一个 `declval<T>()` 的傻函数。 `decltype` + `declval` 的技术不是很难，十分有用，应该被掌握。

`...` 的可变参数，还有函数重载的复杂规则，我们应该敬而远之。尤其是函数重载。考虑到默认构造函数，默认类型转换函数，默认参数，模板函数等等语言特性，模板函数的重载规则是十分复杂的。如果大多数人都记不住这些复杂的规则，那么利用这些规则写出来的代码，就可读性很差了。

这些例子远远没有到达库函数的质量，因为我们没有考虑很多边缘案例，例如，如果 T 有等号操作符重载，但是他的返回值不是 `T&` 。


完整代码

```include
quote cpp cpp_src/is_copy_assignable_0.cpp
```

程序输出

```include
quote plain cpp_src/is_copy_assignable_0.out
```

# `void_t` 一个奇怪的，而有用的模板类

```cpp
template<class...>
using void_t = void;
```

`void_t` 已经是 c++17 的标准了 [http://en.cppreference.com/w/cpp/types/void_t]() 。 但是大神讲这个视频的时候，还没有进去。


这个类型看起来没啥用，很简单。

你给他无论啥类型，他都返回一个 void 类型。

关键点是，你给他的类型必须是有效的类型，不能是匹配失败的类型。

这里还用的了 c++11 的一个新特性， `using` ，这个十分有用，让代码看起来十分简洁。



后面大神讲到应用这个 `void_t` 重写 `is_copy_assignable` 的时候，现场一片掌声。


# 使用 `void_t`，编写 `has_type_member`

如果 `T::type` 是一个合法的表达式，那么 `has_type_member<T>::value` 是 true ，否则就是  false 。例如

 1. `has_type_member<enable_if<true> >::value` = true
 2. `has_type_member<int>::value` = false

按照模式，我们首先定义 primary template

```cpp
// primary defination.
template<class T, class = void >
struct has_type_member : public true_type {
};

```

很明显，大多数我们自定义的类，都没有 `T::type` 的定义，于是默认返回 false 。

注意 `class = void` 十分关键。 要理解他为什么十分关键，就需要理解 c++ 是如何找到匹配的 template specialization 的。这个比较长，我简单说一下我的理解。

 1. 看用户给定的参数，如果有给定参数，那么用用户提供的参数。
 2. 如果用户没有提供模板参数，那么用默认的。
 3. 找到所有模板参数之后，看看哪一个是更加特殊的。


对于 `has_type_member<int>`来说，`has_type_member` 的第一个模板参数是  int ，用户提供的，第二个模板参数，用户没有提供，我们用默认的，就是 void 。

头两条简单，我们很容易理解了。这个时候，编译器发现有两个匹配的模板。

部分特例化的 `struct has_type_member<T, void_t<typename T::type> > : public true_type` ，这个匹配不上，因为 `void_t<int::type>` 匹配失败。 匹配失败不是错误，继续匹配。匹配到了 primary template 。于是 `has_type_member<int>::value` 是 false 。

对于 `has_type_member<enable_if<true,void> >`来说，`has_type_member` 的第一个模板参数是 `enable_if<true,void>` ，用户提供的，第二个模板参数，用户没有提供，我们用默认的，就是 void 。


部分特例化的 `struct has_type_member<T, void_t<typename T::type> > : public true_type` ，这个匹配成功，因为 `void_t<int::type>` 是合理的表达式。于是 ` has_type_member<T, void_t<typename T::type>::value` 是  true 。


这个模式十分有用，简单易懂。可以判断一个类是否有成员函数，静态成员变量，成员变量等等。我们就离实现 `concept` 不远了。


完整代码

```include
quote cpp cpp_src/has_type_member_0.cpp
```

程序输出

```include
quote plain cpp_src/has_type_member_0.out
```
