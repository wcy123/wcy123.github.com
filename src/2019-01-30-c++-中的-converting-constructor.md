# C++ 中的 converting constructor

如果构造函数可以只传递一个参数，那么这个构造函数又叫做类型转换构造函数, converting constructor 。参考 https://en.cppreference.com/w/cpp/language/converting_constructor

converting constructor 很有用，也容易被误用。

例如

```cpp
struct Foo {
  Foo(int value);
};

Foo::Foo(int value) { //
  cout << "Foo(int) is called, value=" << value;
}

```

## 应用场景

主要有以下几种应用场景

### 构造一个对象

```cpp
Foo obj(10);
```

这种是很普通的调用，这个时候，converting constructor 看起来和普通构造函数一样，没有什么特殊的地方。

### 用不同类型直接赋值

```cpp
  Foo obj = 10;
```

这个时候会调用调用 `Foo::Foo(int)` 。

### 函数参数

```cpp
void fun1(Foo v) {
}
int main(int argc, char *argv[])
{
  fun1(100);
  return 0;
}
```

虽然 `fun1` 接收参数的类型是 `Foo` ，然而我们可以传递一个 int 类型的参数给 `fun1` ，这个时候，会调用 `Foo::Foo(int)` ，构造一个临时 `Foo` 对象，然后把这个临时对象传递给 `fun1` 。

### 函数返回值

```cpp
Foo fun2() {
    return 100;
}
int main(int argc, char *argv[])
{
    fun2();
}
```

类似的，尽管 `fun2` 的返回值类型是  `Foo` ，但是依然可以使用 `return 100;` ，这个时候，使用 `Foo::Foo(int)` 构造一个临时对象，`fun2` 返回这个临时对象。


## 正确的使用，会提高代码的可读性


很多标准库中的类，例如 `std::vector` ，`std::string` 都有看起来不错的 converting constructor ，可以使代码的可读性很好。

```cpp
string a = "hello world";
vector<int> v = {1,2,3};
```

converting constructor 还可以组合起来。例如

```cpp
void fun3(const vector<string>& vs){
    int c = 0;
    for(auto & s: vs) {
      cout << "vs[" << c++ << "] = " << s << endl;
    }
}
int main(int argc, char *argv[])
{
    fun3({"hello", "world"});
}
```

这里调用的 vector 构造函数

```cpp
vector::vector( std::initializer_list<T> init,
                const Allocator& alloc = Allocator() );
```

尤其应该注意的是，转换构造函数可以有多个参数，如果其他参数有默认值的话。

参考 https://en.cppreference.com/w/cpp/container/vector/vector 。

其中 `T` 是 `std::string` ，于是又调用

```cpp
basic_string( const CharT* s,
              size_type count,
              const Allocator& alloc = Allocator() );
```

参考 https://en.cppreference.com/w/cpp/string/basic_string/basic_string 。

类似的，也可以作为返回值。

```cpp
vector<string> fun4() { return {"hello", "world"}; }

int main(int argc, char *argv[])
{
    int c = 0;
    for(auto & s: fun4()) {
      cout << "vs[" << c++ << "] = " << s << endl;
    }
}
```

这样的代码，看起来十分简洁。

## 错误的使用，代码会有隐藏的 bug

```
// 这段代码会有编译错误
vector<string> fun5() { return 10u; }
```

这里会调用 `vector( size_type count );` ，为了避免这种错误，标准库里面的定义是

```cpp
explicit vector( size_type count );
explicit vector( size_type count, const Allocator& alloc = Allocator() );
```

关键字 `explicit` 起到了应有的作用，尽管 `vector(size_type count)` 只有一个参数，因为 `explicit` 关键字的存在，这个构造函数不是一个转换构造函数。

在实际项目中，除了通用性非常好的标准库之外，大多数情况下，我们都不需要使用转换构造函数。转换构造函数看起来很酷，实际上有一些缺点，违反了一些原则。

1. 显式比隐式的要好
2. 破坏了 C++ 中的类型安全机制。
3. 考虑到函数重载 function overloading ，很容易搞不清楚调用了哪一个版本的函数。
4. 破坏了 POLA 原则，https://en.wikipedia.org/wiki/Principle_of_least_astonishment 。有些场景，会让用户很吃惊。

实际上，大多数的只有一个参数的构造函数都应该加上了 `explicit` 关键字。
