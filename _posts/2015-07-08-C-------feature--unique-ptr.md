---
layout: post
title:  "C++11 的 feature, unique_ptr"
date:   2015/07/08 16:39:21
categories:
comments: true
---


参考链接
 1. [unique_ptr][1]
 2. [C++11: unique_ptr][2]

[1]: http://www.cplusplus.com/reference/memory/unique_ptr/ "unique_ptr"
[2]: http://www.drdobbs.com/cpp/c11-uniqueptr/240002708 "C++11: unique_ptr"


`unique_ptr` 的语义

1. `unique_ptr <T>` 看起来和指针 T* 很像
2. 自动调用析构函数，释放所指对象的资源，不产生资源泄漏。
3. `unique_ptr<T>` 保证只有一个对象拥有 (own) 某一个指针。

做到第一点很容易，重载 `operator*`，`operator->` 和 `operator []`即可。
很多人都会写。为了简单，`unique_ptr<T>` 不支持指针算数操作，`++`, `--`
之类的。因为 `unique_ptr<T> + 1` 之后，他不能保证语义 2， 既他无法确定
他是 `T* +1` 的唯一拥有者。


{% highlight c++ %}
#include <iostream>
#include <memory>
using namespace std;
struct A {
    A() {
        cerr <<  __FILE__ << ":" << __LINE__
             << ": [" << __FUNCTION__<< "] "
             << endl;
    }
    ~A() {
        cerr <<  __FILE__ << ":" << __LINE__
             << ": [" << __FUNCTION__<< "] "
             << endl;
    }
};
void test1()
{
    unique_ptr<A> ptr(new A());
}
int main(int argc, char *argv[])
{
    cerr <<  __FILE__ << ":" << __LINE__ << ": [" << __FUNCTION__<< "] "
         << "begin call test1"
         << endl;
    test1();
    cerr <<  __FILE__ << ":" << __LINE__ << ": [" << __FUNCTION__<< "] "
         << "done"
         << endl;
    return 0;
}
{% endhighlight %}

程序输出


{% highlight bash %}
clang++ -std=c++11  -Wall -Werror unique_ptr.cpp && ./a.out
unique_ptr.cpp:20: [main] begin call test1
unique_ptr.cpp:6: [A]
unique_ptr.cpp:10: [~A]
unique_ptr.cpp:24: [main] done
{% endhighlight %}

第二点就有点麻烦，需要借用 C++11 的其他特性才能实现，最重要的就是右值
引用和 `std::move` 。

## 禁止拷贝构造函数


{% highlight c++ %}
void test2_1(unique_ptr<A> x)
{
}
void test1()
{
    unique_ptr<A> ptr(new A());
    test2_1(ptr);
}
{% endhighlight %}

这样会产生编译错误

{% highlight bash %}
clang++ -std=c++11  -Wall -Werror unique_ptr.cpp && ./a.out
unique_ptr.cpp:20:13: error: call to deleted constructor of 'unique_ptr<A>'
    test2_1(ptr);
            ^~~
/usr/bin/../lib/gcc/x86_64-linux-gnu/4.9/../../../../include/c++/4.9/bits/unique_ptr.h:356:7: note:
      'unique_ptr' has been explicitly marked deleted here
      unique_ptr(const unique_ptr&) = delete;
      ^
unique_ptr.cpp:14:28: note: passing argument to parameter 'x' here
void test2_1(unique_ptr<A> x)
                           ^
1 error generated.
{% endhighlight %}


## 禁止赋值操作符重载


{% highlight c++ %}
void test1()
{
    unique_ptr<A> ptr(new A());
    unique_ptr<A> ptr2;
    ptr2 = ptr;
}
{% endhighlight %}

产生编译错误

{% highlight bash %}
clang++ -std=c++11  -Wall -Werror unique_ptr.cpp && ./a.out
unique_ptr.cpp:18:10: error: overload resolution selected deleted operator '='
    ptr2 = ptr;
{% endhighlight %}

## 那他有啥用

不能复制，不能赋值，怎么用呢？我们经常用到传递指针的操作啊。

传递指针给另一个函数的时候，有两种情况

1. 调用者保留指针的所有权
2. 调用者放弃指针所有权


### 放弃所有权


{% highlight c++ %}
#include <iostream>
#include <memory>
using namespace std;
struct A {
    A() {
        cerr <<  __FUNCTION__ << endl;
    }
    ~A() {
        cerr <<  __FUNCTION__ << endl;
    }
};
void test2(unique_ptr<A> ptr2)
{
    cerr << "test2 grab ownership" << endl;
    cerr << ptr2.get() << endl;
    cerr << "test2 done"<< endl;
}
void test1()
{
    unique_ptr<A> ptr(new A());
    cerr << "test1 give up ownership" << endl;
    test2(std::move(ptr));
    cerr << ptr.get() << endl;
}
int main(int argc, char *argv[])
{
    test1();
    return 0;
}
{% endhighlight %}

输出结果


{% highlight bash %}
clang++ -std=c++11  -Wall -Werror unique_ptr.cpp && ./a.out
A
test1 give up ownership
test2 grab ownership
0xdfb010
test2 done
~A
0
{% endhighlight %}


可能有问题了，不是说不能调用拷贝构造函数吗？怎么
`test2(std::move(ptr))` 就不报错了呢？这个不是拷贝构造函数，这个是移动
构造函数 (move constructor)。`std::move` 的作用是把一个变量变成右值引
用 (rvalue reference) 。这两个是 C++ 11 新的特性。

### 保留所有权

这个简单，就用 `unique_ptr<A> &` 代替就行了。这就是 `unique*ptr` 的作
用，如果 `ptr` 不放弃所有权，那么就不可能有其他的 `unique*ptr<T>` 拥有
同样的对象。 除非你传递 `ptr` 对象本身的引用。


## 和容器的关系

`unique_ptr` 可以很好的和容器一起工作。`vector<unique_ptr<A>`


{% highlight c++ %}
#include <iostream>
#include <memory>
#include <vector>
using namespace std;
struct A {
    A(int _i):i(_i) {
        cerr <<  __FUNCTION__ <<  ' ' << i << endl;
    }
    ~A() {
        cerr <<  __FUNCTION__ << ' ' << i << endl;
    }
    int i;
};
void test1()
{
    cerr << "begin" << endl;
    {
        vector<unique_ptr<A> > v;
        for(int i = 0; i < 3; ++i){
            v.push_back(unique_ptr<A>(new A(i)));
        }
    }
    cerr << "done" << endl;
}
int main(int argc, char *argv[])
{
    test1();
    return 0;
}
{% endhighlight %}

这里隐含使用了右值引用，如果没有右值引用，实现这个功能还不容易啊。
`v.push_back` 实际上的函数原型是


{% highlight c++ %}
void push_back( T&& value );
{% endhighlight %}

下面这个例子也行，使用到了转移构造函数(move constructor)


{% highlight c++ %}
unique_ptr<A> ptr(new A(i));
v.push_back(std::move(ptr));
{% endhighlight %}


## 和 `shared_ptr` 的关系

`unique_ptr` 和 `shared_ptr` 都可以自动析构动态生成的对象。但是二者不同。

1. 多个 `shared_ptr` 对象可以共享同一个指针，而 `unique_ptr` 可以保证
   我是指针的唯一拥有者。
2. 当所有 `shared_ptr` 对象都被析构掉了，则动态指针所指的对象被析构。因
   为 `unique_ptr` 唯一拥有该指针，`unique_ptr` 对象析构的时候，也会析构指
   针所指的对象。

## 和 `auto_ptr` 的关系

`unique_ptr` 代替了 `auto_ptr` 。 `auto_ptr` 是以前标准遗留的一个比较
难看的设计。为了避免抛出异常的时候，`auto_ptr` 的对象可以被成功析构。
但是因为缺少右值引用和移动构造函数的支持，`auto_ptr` 的语义很不清晰。
`auto_ptr` 也不能和容器很好的工作，你不能用 `vector<auto_ptr<T> >`。

## 性能

`unique_ptr` 的性能很好，几乎没有任何多余的开销。
