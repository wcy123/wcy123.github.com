---
layout: post
title:  "阅读 rxjava 源代码之  - map"
date:   2017/01/07 20:17:39
categories:
comments: true
---
[上一篇文章](http://www.jianshu.com/p/a927dfb384e9) 写了一个极其简化的 Rxjava Observable ，现在，我试图添加一个 `map` 操作符。

```java
public <R> Observable<R> map(Func1<T, R> func1) {
  return new Observable<R>(subscriber -> this.subscribe(new Subscriber<T>() {
      @Override
      public void onCompleted() {
          subscriber.onCompleted();
      }

      @Override
      public void onError(Throwable e) {
          subscriber.onError(e);
      }

      @Override
       public void onNext(T t) {
           R r = null;
           try {
               r = func1.call(t);
           } catch (Throwable e) {
               unsubscribe();
               return;
           }
           subscriber.onNext(r);
       }

      @Override
      public void unsubscribe() {
          subscriber.unsubscribe();
      }

      @Override
      public boolean isUnsubscribed() {
          return subscriber.isUnsubscribed();
      }
  }));
}
```

Java 本身语言限制，导致代码臃肿。代码的核心部分就是

```java
return new Observable<R>(subscriber -> this.subscribe(new Subscriber<T>() {
    @Override
     public void onNext(T t) {
           R r = null;
           try {
               r = func1.call(t);
           } catch (Throwable e) {
               unsubscribe();
               return;
           }
           subscriber.onNext(r);
       }
}
```

这里看到以下几点
1. `map` 接收两个参数 ，注，对于成员函数，第一个参数是 `this`。
2. 第一个参数是 `Observable<T> this`，
3. 第二个参数是 `Func1<T,R> func1`;
    1. `func1` 接收一个参数 `T`
    2. `func1.call(t)` 返回一个 `R`
4. `map` 要返回一个 `Observable<R>` ，那么就要在 `OnSubscribe` 的时候，需要从 `this` 里面得到一个个 `T t` ，然后用 `func1.call(t)` ，然后转移给下一个 `subscriber`。

因为  Java8 lambda 关键字的引入，我们看到函数式编程中的 variable capture 的强大。

这是一个非常简化的 `map` 实现，还有很多问题。
1. 还有非常多的操作符和 `map` 很类似，这里有很多重复代码。
2. backpressure 没有处理。
3. `unsubscribe` 还没有处理好。subscriber 链的关系没有处理。
4. 异常也没有处理好。
5. 没有保证 `onComplete` 只被调用一次 。

这个简化的实现尽管有很多问题，但是可以帮助我们理解原有复杂完整的实现。`Map` 的核心结构是这样

1. 本身含有一个 `Subscriber` 对象，订阅上层的 `Observable`
2. 返回一个 `Observable` 对象，提供给下层订阅。
3. 这种方法组合了 `Observable`，构成了一个链条。


```java
OnSubscribe<?> onSubscribe = subscriber /*传递进来的 subscriber 参数，给下层产生数据*/
     -> {
        /* this 是上层的 Observable，订阅上层 */
        this.subscribe(new Subscriber<T>() {
        @Override
        public void onNext(T t) {
           R r = null;
           try {
               r = func1.call(t);
           } catch (Throwable e) {
               unsubscribe();
               return;
           }
          /* 当上层产生数据的时候，经过转换，传递给下层*/
           subscriber.onNext(r);
       }
      };
Observable<R> ret = new Observable<R>(onSubscribe);
return ret;
}
```
