---
layout: post
title:  "c++ 中的 remove-erase 俗语"
date:   2017/04/30 16:35:02
categories:
comments: true
---

[Erase–remove_idiom](https://en.wikipedia.org/wiki/Erase–remove_idiom)
 和 Scott Mayer 的 effective stl 都提到过这个俗语。

类似下面的代码，删除所有偶数。


```{.cpp include=cpp_src/remove_erase.cpp}
```


我们深入研究一下，`remove` 之后，vector 里面变成了什么。


```{.cpp include=cpp_src/remove_erase_2.cpp}
```


```
before erase
i= 1
i= 3
i= 7
i= 4
i= 6
i= 7
i= 8
after erase
i= 1
i= 3
i= 7
```

可以看到， `remove` 重新排列了 `vector`  中的元素。

但是后面的元素有些奇怪，`2` 不见了，出现两个 `7` 。无论如何，根据手册，后面的元素已经是无效了。我们只是好奇，后面的元素是什么。


我们看看 remove 的实现，gcc 中的 stl 的实现。

```cpp
template <class _ForwardIterator, class _Predicate>
_ForwardIterator
remove_if(_ForwardIterator __first, _ForwardIterator __last, _Predicate __pred)
{
    __first = std::__1::find_if<_ForwardIterator, typename add_lvalue_reference<_Predicate>::type>
                           (__first, __last, __pred);
    if (__first != __last)
    {
        _ForwardIterator __i = __first;
        while (++__i != __last)
        {
            if (!__pred(*__i))
            {
                *__first = std::__1::move(*__i);
                ++__first;
            }
        }
    }
    return __first;
}
```

可以看到，标准库里面用 `std::move` 函数重新排列了顺序。

这里可以看到，代码移动了 `3` 和 `7` 元素，但是不保证 `2` 元素继续有效，因为他的位置已经无效了，在移动过程中，`2` 把位置让给了 `3` 。`3` 的位置让给了 `7`。

我们验证一下移动的次数。

```{.cpp include=cpp_src/remove_erase_3.cpp}
```

我们观察一下输出
```{.plain include=cpp_src/remove_erase_3.out}
```

我试着解释一下输出结果。

 1. `{1,2,3...}` 调用 `Foo(int)` 构造了 7 个  Foo 对象
 2. vector 通过 `Foo(const Foo& x)` ，把 `initialize_list` 中的对象拷贝到 vector 中。
 3. 原来的 7  `initialize_list` 中的对象被析构掉了。
 4. `remove` 移动对象，调用了 `operator=(Foo&&)` ，后面具体解释。
 5. 在打印过程中，因为 `for(auto i:` 中，没有使用 `auto&` ，于是拷贝构造了临时对象 `1`, `3`, `7`, `4`, `6`, `0`, `8` ，然后打印，然后析构这些临时对象
 6. 调用 `vector.erase` 的时候，会调用 `~Foo` 析构掉 `8,0,6,4` 。可以看到，vector 是倒着析构这些对象的。不过这个无关紧要，标准没有强调析构顺序。
 7. 然后类似步骤 5 打印过程中，构造，析构 临时对象 。 `1,3,7`
 8. main 函数结束，析构 `vector` 对象，`vector` 对象倒着析构了 `7,3,1` 三个对象。


上面的代码，第4步中，在移动的过程中，打印调试信息，显式 `2` 的位置让给了 `3` 。然后 `3` 的位置设置为无效值 `value = 0` 。 `3` 位置上已经是带有了无效值，这个位置让给了 `7` ，同时 `7` 的位置被设置成为了无效值。
