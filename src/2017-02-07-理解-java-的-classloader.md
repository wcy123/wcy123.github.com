# 理解 java 的 classloader



最近对 classloader 感兴趣, 于是研究了一下。下面是 classloader 的核心代码。

```java
public Class<?> loadClass(String name) throws ClassNotFoundException;
protected Class<?> loadClass(String name, boolean resolve)
        throws ClassNotFoundException;
protected Class<?> findClass(String name) throws ClassNotFoundException;
protected final Class<?> defineClass(byte[] b, int off, int len)
        throws ClassFormatError;
protected final void resolveClass(Class<?> c);
```

可以看到，只有 `loadClass(String)` 是 public 的成语函数，其他几个函数都是 `protected` ，意味着是只有子类可以调用。


例如下面的 java 普通代码

```java
class FactoryA {
    A build() {
          return new A();
    }
}
```

如果 A 是第一次被访问，也就是说在此之前，如果 class A 没有被加载过，那么隐含着下面的语句在其中。

```java
  this.getClass().getClassLoader().loadClass("full.package.name.A");
```

https://en.wikipedia.org/wiki/Java_Classloader 和很多其他网上文章，都讲过一下 class loader 中的几个基础 class loader ，和他们的作用。

- bootstrap class loader
- Extensions class loader
- System class loader


# 代理模式

大多数 class loader 会采用代理模式，因为默认的 ClassLoader 是这么做的。

```java
protected Class<?> loadClass(String name, boolean resolve)
        throws ClassNotFoundException
{
     // First, check if the class has already been loaded
     Class<?> c = findLoadedClass(name);
     if (c == null) {
         try {
             c = parent.loadClass(name, false);
         } catch (ClassNotFoundException e) {
             // ClassNotFoundException thrown if class not found
             // from the non-null parent class loader
         }
         if (c == null) {
             // If still not found, then invoke findClass in order
             // to find the class.
             c = findClass(name);
         }
     }
     if (resolve) {
         resolveClass(c);
     }
     return c;
}
```

这里是简化过的代码，去掉了关于锁，bootstrap 和统计的部分。可以看到，默认的实现是这样的。

1. 首先判断是否已经加载过。如果加载过，用以前的，不会重复加载。
2. 如果没有，那么代理给 `parent` 来加载，bootstrap class loader 没有  parent ，我们先忽略 bootstrap 相关的处理代码。
3. 如果 `parent` 没有加载成功，那么调用 `findClass` 来处理。
4. 调用 `resolveClass` 来连接(linking) 相关的依赖的类。这个也可能引入继续加载其他的类。

默认的 findClass 直接抛出 `ClassNotFoundException`

```java
protected Class<?> findClass(String name) throws ClassNotFoundException {
    throw new ClassNotFoundException(name);
}
```

这意味着，如果我们自己写一个自定义的 class loader 类，那么我们最好重载 findClass  这个成员函数。

如果我们定义一个 `findClass` 那么我们如何返回一个 `Class<?>` 对象呢？ 答案就是，调用 `defineClass`。

首先，我们需要找到 class 的字节码，表示称为 `byte[]` ，然后传递给  `defineClass` 返回一个可用的 `Class<?>` 对象。


这里还有一个问题，`this.getClass().getClassLoader()` 返回的是哪一个 class loader 呢？

答案就是，如果某一个 class loader 调用了 `defineClass` 返回了某一个 `Class<?>` 对象，那么这个 `Class<?>#getClassLoader()` 函数，就会返回这个 class loader 。


这个规则看起来很简单，但是我们需要分析一下，可能的后果。

String 是一个很常用的类，是 bootstap class loader 定义的，换句话说，是 bootstrap class loader 调用了 defineClass 返回的 `Class<String>` 。那么，在 String 这个类里面，依赖的其他类，例如  `Object` ，就会直接调用 bootstrap class loader 来加载。不会使用 extension class loader 或者 system class loader 。


我们都想想这个规则，就会发现，如果子类的行为重新定义了搜索顺序，那么就很危险了，容易因为使用者的混乱。

例如 tomcat 的 war class loader ，他重新定义了 `loadClass` ，先看看自己能否找到 class ，如果找不到，然后再代理给 `parent` ，即 ear class loader 去搜索。
