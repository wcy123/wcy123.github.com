---
layout: post
title:  "C++ vector<A> 与 A::A() 的关系"
date:   2015/07/08 16:57:40
categories:
comments: true
---

使用 `vector<A>` 到底要调多少次构造函数`A::A()`呢？


{% highlight c++ %}
#include <iostream>
#include <vector>
using namespace std;

class A {
public:
    explicit A(int v);
    explicit A(const A& other);
    ~A();
public:
    int value;
};
A::A(int v):value(v)
{
    cout << __FUNCTION__
         << ' ' << value
         << ' ' << (void*)this
         << endl;
}
A::A(const A& other):value(other.value)
{
    cout << __FUNCTION__
         << ' ' << value
         << ' ' << (void*)this
         << ' ' << (void*)&other
         << endl;
}
A::~A()
{
    cout << __FUNCTION__
         << ' ' << value
         << ' ' << (void*)this
         << endl;
}

vector<A> foo(int i)
{
    vector<A> x;
    for(int i = 0; i < 3; ++i){
        x.push_back(A(i));
    }
    cout << "foo return " << (void*) &x<< endl;
    return x;
}
int main(int argc, char *argv[])
{
    vector<A> x = foo(10);
    cout << "main get " << (void*) &x << endl;
    return 0;
}
{% endhighlight %}

运行结果 # 号后面的是我写的注释


{% highlight bash %}
clang++ -Wall -Werror -std=c++11 rvalue.cpp && ./a.out
# 构造临时对象 A(i), i = 0 ，地址是栈(stack)上的地址
A 0 0x7fff06edb8e0
# x.push_back(A(i)) 调用拷贝构造函数，other 的地址是刚刚创建的 A(i)
# 新对象x[0]的地址是 0x2528010，是在堆(heap)上的地址
A 0 0x2528010 0x7fff06edb8e0
# 调用析构函数，析构栈上的临时对象 x[0]
~A 0 0x7fff06edb8e0
# 和i=0类似 构造临时对象 A(i), i = 1，
# 因为是在栈上，自然重用刚刚 A(0),i=0 用过的地址。
A 1 0x7fff06edb8e0
# 和i=0类似 调用拷贝构造函数，准备 push_back
# x[1] 的地址是 0x2526034
A 1 0x2528034 0x7fff06edb8e0
# 糟糕，vector 发现内存不够了，动态扩展内存
# 用拷贝构造函数，把 x[0] 拷贝的新的位置上
# x[0] 的地址是 0x2528030，和 x[1] 是连续分配的，因为 sizeof(A) == 4
A 0 0x2528030 0x2528010
# 析构刚刚的 x[0]
~A 0 0x2528010
# 析构掉刚刚栈上的临时对象
~A 1 0x7fff06edb8e0
# 和 i=0 类似 构造临时对象 A(i), i = 2
A 2 0x7fff06edb8e0
# 和 i=0 类似 调用拷贝构造函数，准备 push_back
A 2 0x2528018 0x7fff06edb8e0
# 糟糕，内存又不够了，动态扩展内存
# 移动(move) x[0] 到新地址
A 0 0x2528010 0x2528030
# 移动(move) x[1] 到新地址
A 1 0x2528014 0x2528034
# 析构掉旧的 x[0]
~A 0 0x2528030
# 析构掉旧的 x[1]
~A 1 0x2528034
# 析构掉临时对象
~A 2 0x7fff06edb8e0
# foo() 返回 vector<A> 对象
foo return 0x7fffe5026248
# 因为有返回值优化，main 中x地址和 foo中x的地址是同一个地址。
main get 0x7fffe5026248
# main 函数返回，析构掉 vector 中的三个对象。
~A 0 0x2528010
~A 1 0x2528014
~A 2 0x2528018
{% endhighlight %}


如何避免多余的调用构造函数呢？提前申请内存！


{% highlight c++ %}
    vector<A> x;
    x.reserve(10);
{% endhighlight %}


{% highlight c++ %}
clang++ -Wall -Werror -std=c++11 vector_a.cpp && ./a.out
A 0 0x7ffffbf11a30
A 0 0x1044010 0x7ffffbf11a30
~A 0 0x7ffffbf11a30
A 1 0x7ffffbf11a30
A 1 0x1044014 0x7ffffbf11a30
~A 1 0x7ffffbf11a30
A 2 0x7ffffbf11a30
A 2 0x1044018 0x7ffffbf11a30
~A 2 0x7ffffbf11a30
foo return 0x7ffffbf11a88
main get 0x7ffffbf11a88
~A 0 0x1044010
~A 1 0x1044014
~A 2 0x1044018
{% endhighlight %}

可以看到，内存被提前申请了，所以不会有多余的拷贝构造函数了。
