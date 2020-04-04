# "深入理解 Return Value Optimization

让我们用一个例子来看看 g++ 的 RVO ( Return Value Optimization ) 是怎么工作的。

```cpp
{{#include 2019-01-30/test_rvo.cpp}}
```

如果我们没有指定编译选项，g++ 默认打开了 RVO 的优化开关

```bash-session
% g++ -std=c++11 test_rvo.cpp
% ./a.out
begin block
&mark 0x7fff5476766c
construct: Foo[0@0x7fff54767708]
&begin 0x7fff5476770c
obj = Foo[0@0x7fff54767708]
&end 0x7fff54767704
deconstructor: Foo[0@0x7fff54767708]
end block
```

我们知道 x86 平台下，堆栈是向下生长的，也就是说，堆栈上的地址分配如下。

```
# main 函数的堆栈空间
0x7fff5476770c: &begin
0x7fff54767708: &obj
0x7fff54767704: &end
...
# build 函数的堆栈空间
0x7fff5476766c: &mark
```

可以看到，`build()` 函数在构造 `Foo` 对象的时候，实际上使用的是 `main` 函数中的堆栈地址空间。换句话说，在调用 `build` 之前，`Foo` 对象的内存就已经提前分配好了，使用的是 `main` 函数的堆栈地址空间，而 `Foo` 对象的初始化是在调用 `build` 函数之后执行的。

同时我们注意到，拷贝构造函数没有被调用。

我们试试关闭 RVO 。

```
+ g++ -fno-elide-constructors -std=c++11 test_rvo.cpp
+ ./a.out
begin block
&mark 0x7fff52a1366c
construct: Foo[0@0x7fff52a13668]
copy from Foo[0@0x7fff52a13668]to Foo[0@0x7fff52a13700]
deconstructor: Foo[0@0x7fff52a13668]
copy from Foo[0@0x7fff52a13700]to Foo[0@0x7fff52a13708]
deconstructor: Foo[0@0x7fff52a13700]
&begin 0x7fff52a1370c
obj = Foo[0@0x7fff52a13708]
&end 0x7fff52a136f0
deconstructor: Foo[0@0x7fff52a13708]
end block
```

`-fno-elide-constructors` 表示关闭 RVO 优化开关。

堆栈分析
```
# main 函数的堆栈空间
0x7fff52a1370c: &begin
0x7fff52a13708: &obj
0x7fff52a13700: &拷贝的临时对象 tmp-obj2
0x7fff52a136f0: &end
# build 函数的堆栈空间
...
0x7fff52a1366c: &mark
0x7fff52a13668: &构造临时对象 tmp-obj1
```

我们看到，拷贝构造函数被调用了两次。我们仔细看一下发生了什么。

1. 在 main 函数的堆栈空间上，预留内存 0x7fff52a13700 ，准备接收返回值 `tmp-obj2` 。
2. 在 build 函数的堆栈空间上，申请内存 0x7fff52a13668 ，并且构造了临时对象 `tmp-obj1`
3. 调用拷贝构造函数，把 `tmp-obj1` 拷贝到 `tmp-obj2` 上。
4. build 函数在返回之前，调用析构函数，析构掉 `tmp-obj1` 。
5. 在 main 函数中，调用拷贝构造函数，把 `tmp-obj2` 拷贝构造到 `obj` 变量上。
6. 立刻析构掉临时对象 `tmp-obj2` 。
7. 继续在执行 main 函数之后的代码，在代码块结束的时候，调用析构函数，析构掉 `obj` 对象。

RVO 的优化实在是太有用了，以至于编译器默认是打开这个优化开关的。

我们可以经常使用类似下面的代码

```cpp
HeavyClass foo();

// at call site
HeavyClass obj = foo();
```

这种代码可读性好，而且我们不用担心效率的问题， RVO 可以保证代码十分高效的运行。

在实际项目中，我会看到下面的代码

```cpp
void foo(HeavyClass * ret);

// at call site
HeavyClass obj;
foo(&obj);
```

这种方式可读性不好。 作者本来的目的是防止多次拷贝对象，然而， 这样通常导致一次多余的函数调用。 因为一般我们在 `foo` 函数里面要构造一个对象，然后拷贝到 `obj` 。更糟糕的是， `obj` 对象被初始化两次，第一次初始化是在调用 `foo` 之前，使用默认构造函数。这个时候 `obj` 对象是一个无意义的对象。因为 RAII 的语义，导致这个设计是很丑陋的。
