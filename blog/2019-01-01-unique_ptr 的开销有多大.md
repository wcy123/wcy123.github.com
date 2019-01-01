---
layout: post
title:  "unique_ptr 的开销有多大"
date:   2019/01/01 12:48:42
categories:
comments: true
---

unique_ptr 的是零开销的，我们看看具体是指什么含义。


```{.cpp include=blog/2019-01-01/unique_ptr_overhead.cpp}
```

我们编译一下，然后看看生产的汇编指令

```{.text include=blog/2019-01-01/unique_ptr_overhead.out}
```

我们大致比较，可以那看到 `make_unique` 和 `new/delete` 编译出来了同样的指令，几乎没有区别。

同样，我们也可以看到，在空间占用上，开销也是一样的，因为 `sizeof(unique_ptr<Foo>) == sizeof(Foo*)`
