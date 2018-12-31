---
layout: post
title:  "c++11 的 extern template"
date:   2017/04/29 23:19:45
categories:
comments: true
---

c++98 的 template 有些问题，代码会膨胀，而且不容易定位到底是实例化了那一个实例。


我设计了一个例子，展现这两个问题。

```{.cpp include=./cpp_src/extn_tmpl.hpp}
```

这里可以看到，根据预处理的宏定义 `EXTRA` 的不同，会产生不同的实例。当然，这个例子看起来十分傻，但是在真实的项目中，由于多人开发，项目复杂，这种例子还真有可能会出现。

```{.cpp include=./cpp_src/extn_tmpl_1.cpp}
```

```{.cpp include=./cpp_src/extn_tmpl_2.cpp}
```

这里是简单的使用 `extn_tmpl.hpp`

```{.cpp include=./cpp_src/extn_tmpl_main.cpp}
```

我们采用下面的命令编译程序

```
+ c++ -std=c++11 -DEXTRA=1 -c -O3 -fno-inline -o extn_tmpl_1.o extn_tmpl_1.cpp
+ c++ -std=c++11 -DEXTRA=2 -c -O3 -fno-inline -o extn_tmpl_2.o extn_tmpl_2.cpp
+ c++ -std=c++11 -c -O3 -fno-inline -o extn_tmpl_main.o extn_tmpl_main.cpp
+ nm extn_tmpl_2.o | c++filt
0000000000000000 T foo_caller2(int, int)
0000000000000010 T int foo<int>(int, int)
+ nm extn_tmpl_1.o | c++filt
0000000000000000 T foo_caller1(int, int)
0000000000000010 T int foo<int>(int, int)
+ ls -l extn_tmpl_1.o extn_tmpl_2.o
-rw-r--r--  1 wangchunye  staff  760 Apr 29 23:32 extn_tmpl_1.o
-rw-r--r--  1 wangchunye  staff  760 Apr 29 23:32 extn_tmpl_2.o
+ c++ extn_tmpl_main.o extn_tmpl_1.o extn_tmpl_2.o
+ ./a.out 1
+ echo $?
3
+ ./a.out 2
+ echo $?
3
+ c++ extn_tmpl_main.o extn_tmpl_2.o extn_tmpl_1.o
+ ./a.out 1
+ echo $?
4
+ ./a.out 2
+ echo $?
4
```

注意到，`extn_tmpl_1` 和 `extn_tmpl_2` 因为 `EXTRA` 的定义不同，看到了不同的 `foo` 的实例化。
而实际使用哪一个，完全取决于链接的时候，是 `extn_tmpl_1` 在前面，还是 `extn_tmpl_2` 在前面，谁在前面，就用哪一个。

这个太危险了，因为在编译期，我们无法确定最终的可执行文件使用哪一个 `foo` 的实例化。

还有一个问题，就是 `foo<int>()` 在 `extn_tmpl_1` 和 `extn_tmpl_2` 中都有定义。导致了代码臃肿。

如果我们使用 `extern template` 的 c++11 的特性，

```
+ c++ -std=c++11 -DEXTERN_TMPL -DEXTRA=1 -c -O3 -fno-inline -o extn_tmpl_1.o extn_tmpl_1.cpp
+ c++ -std=c++11 -DEXTERN_TMPL -DEXTRA=2 -c -O3 -fno-inline -o extn_tmpl_2.o extn_tmpl_2.cpp
+ nm extn_tmpl_2.o
+ c++filt
0000000000000000 T foo_caller2(int, int)
                 U int foo<int>(int, int)
+ nm extn_tmpl_1.o | c++filt
0000000000000000 T foo_caller1(int, int)
                 U int foo<int>(int, int)
+ ls -l extn_tmpl_1.o extn_tmpl_2.o
-rw-r--r--  1 wangchunye  staff  664 Apr 29 23:38 extn_tmpl_1.o
-rw-r--r--  1 wangchunye  staff  664 Apr 29 23:38 extn_tmpl_2.o
+ c++ extn_tmpl_main.o extn_tmpl_1.o extn_tmpl_2.o
Undefined symbols for architecture x86_64:
  "int foo<int>(int, int)", referenced from:
      foo_caller1(int, int) in extn_tmpl_1.o
```

因为 `-DEXTERN_TMPL` , `extern template` 被激活。

这样我们注意到两件事

 1. `foo<int>` 不再出现在  `extn_tmpl_1.o` 或者 `extn_tmpl_2.o` 的定义中，而成为了未定符号。
 2. 文件大小变小了，从 760 字节，变成了 664 字节。

也许你会说，文件变小一点点，没什么大不了，然而，在真实的项目中，有很多 header only 的库，巨大的头文件，被几千个小文件包含，这个代码膨胀的数字就很可观了。


还有，我们在链接的时候，得到了一个链接错误，这正是我们想要的，我们需要确定的知道实例化的实现是什么，如果不知道，那么就抛出错误。

我们可以重新编译 `extn_tmpl_main.o` ，来定义 `foo<int>`。

```
+ c++ -std=c++11 -DEXTERN_TMPL -DEXTRA=0 -c -O3 -fno-inline -o extn_tmpl_main.o extn_tmpl_main.cpp
+ nm extn_tmpl_main.o
+ c++filt
                 U foo_caller1(int, int)
                 U foo_caller2(int, int)
0000000000000000 T int foo<int>(int, int)
0000000000000010 T _main
```

这样，我们确切的知道，`foo<int>` 定义在 `extn_tmpl_main` 中， 无论怎样调换 `extn_tmpl_1` 和 `extn_tmpl_2` 链接的时候的位置，都得到相同的运行结果。

```
+ c++ extn_tmpl_main.o extn_tmpl_1.o extn_tmpl_2.o
+ ./a.out 1
+ echo $?
2
+ ./a.out 2
+ echo $?
2
+ c++ extn_tmpl_main.o extn_tmpl_2.o extn_tmpl_1.o
+ ./a.out 1
+ echo $?
2
+ ./a.out 2
+ echo $?
2
```
