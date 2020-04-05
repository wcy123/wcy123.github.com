# 阅读 Subscriber 的实现中关于  backpressure 的部分


rxjava 中最具有挑战性的设计就是 backpresure 。例如 `zip` 操作符，合并两个 `Observable` A 和 B 。如果  B 的产生速度比 A 快，那么就需要不停的缓存 B 多余生成出来的数据，这样内存就无限增长了。 backpressure 的机制就是让 B 生成慢一点。

目前为止，我看到 rxjava 的设计是很丑陋的。这种机制是没有强制性的。更糟糕的是， rxjava 暴露了 `Observable.create(OnSubscribe<?> onSubscribe)` 这个函数，如果不了解这个机制，上来"想当然" 的实现一个 `OnSubscribe` ，而不管 backpressure 机制，很容易产生 `MissingBackpressureException` 。

“想当然” 不是使用者的错，而是库的设计者的错误。可惜的是，太多用户重度使用这个 `Observable.create(OnSubscribe<?> onSubscribe)` 函数，为了保证现有程序能够继续运行，就不能隐藏这个函数。于是，我们在注释中，可以看到下面一段话

>  <strong>This method requires advanced knowledge about building operators and data sources; please consider other standard methods first; </strong>

本文试图得到 “advanced knowledge” 。

下面是 backpressure 的协议是如何建立的。

```java
someObservable.subscribe(new Subscriber<T>() {
    @Override
    public void onStart() {
      request(1);
    }

    @Override
    public void onCompleted() {
      // gracefully handle sequence-complete
    }

    @Override
    public void onError(Throwable e) {
      // gracefully handle error
    }

    @Override
    public void onNext(T n) {
      // do something with the emitted item "n"
      // request another item:
      request(1);
    }
});
```

可见底层 `subscriber` 在刚刚启动的时候，发起流控请求 `onStart` , `request(1)` 。告诉楼上的，哥们，别整太多，一个数据就够了，多了处理不了。
`onNext` 中，先处理数据，处理完了，告诉楼上的，接着往下放数据，别多，就一个。

这里需要注意的是，不能再 `request(n)` 函数里面产生数据，否则递归调用 `onNext` ，可能导致爆栈了。

我们看看 `Subscriber` 是如何实现这个协议的。

```java
public abstract class Subscriber<T> implements Observer<T>, Subscription {
// represents requested not set yet
private static final long NOT_SET = Long.MIN_VALUE;
private final SubscriptionList subscriptions;
private final Subscriber<?> subscriber;
private Producer producer;
private long requested = NOT_SET; // default to not set
}
```
本文重点关注  backpressure ，只看和这个相关的变量
* `NOT_SET` 表示无效的请求数据量。或者说，还 `Subscriber` 没有提供请求的数据量时的状态。
* `subscriber` ，如果这个值不为 `null`，那么把 backpressure 相关的处理，交给这个  `subscriber` 处理。有大多数很多操作符，自己并不能很好的处理这种过载，需要一层层向上传递，一直到数据源，只有产生数据的地方，才能比较好的处理，因为在那里，可以很容易的少产生一些数据。
* `producer` 如果本 `subscriber` 可以处理，那么代理给 `producer` 来处理。
* `requested` ，计数器，记录楼下的请求多少数据。
  * 如果是  `NOT_SET` ，就是说楼下还不知道请求多少。
  * 如果是 `MAX_LONG` ，就是说楼下来者不拒，不怕 overload
  * 如果是其他值，就是说楼下的最多能处理多少数据。

```java
 protected final void request(long n) {
       // if producer is set then we will request from it
       // otherwise we increase the requested count by n
       if (producer != null) {
            producer.request(n);
       } else {
            requested = requested + n;
       }
}
```
这个函数被我简化了，去掉了关于线程安全的部分。这样代码的可读性好多了。
* 就是说如果有  `producer` ，那么计数的功能就交给 `producer` 了。
* 如果没有，那么 `requested` 用来计数。

这里简化了代码，去掉了 `requested` 溢出的处理，就是说当 `requested + n` 比 `LONG_MAX` 还要大的时候，会防止其变成负数。

```java
public void setProducer(Producer p) {
    boolean passToSubscriber = subscriber != null && requested == NOT_SET;
    producer = p;
    if (passToSubscriber) {
        subscriber.setProducer(producer);
    } else {
        if (requested == NOT_SET) {
            producer.request(Long.MAX_VALUE);
        } else {
            producer.request(requested);
        }
    }
}
```
同样，这里去掉了关于线程安全的代码。

个人认为，`setProducer` 这个函数名字起的不好，因为这个函数除了设置 `producer` 成员变量之外，还会调用 `produce.request` 函数。

再来分析一下这个 `setProducer` 函数
* 底层是否掉用过本层的 `request(n)` 。
    * 如果调用过，`requested != NOT_SET`，意味着底层出发了流控请求。
    * 如果没有调用过，`requested == NOT_SET`，意味着底层没有出发了流控请求。
* `producer` 是真正处理流控的逻辑。`subscriber` 把流控逻辑交给 `producer`处理。如果没有 `producer` , `subscriber` 也就只能简单的计数，根本处理不了流控。
* 如果在触发流控请求之前，`setProducer` 函数被调用，那么要看本层是否愿意处理这个流控请求。
   * 如果成员变量 `subscriber` 不是空，那么表示本层 Subscriber 不愿意，或者不能够处理好这个 backpressure ，交个上层处理 `subscriber.setProducer(producer)`。
   * 上层如果不产生数据，本层的 `OnNext` 也不会触发。从而达到了流控的目的。这样一层一层往上传，一直要交给数据源那一层才好处理。换句话说，如果你需要创建了一个  Observable，例如你写了一个新的 operator ，但是不能很好地处理 backpressure ，那么最好往上传递。在 `OnSubscribe` 的时候，把本层 `subscriber`和上层 `subscriber` 串起来。
   * 如果本层愿意处理 backpressure 请求，那么就调用 `procuder.request` 处理请求。
* 如果是在触发流控请求之后， `setProducer` 被调用，那么无论本层是否愿意，都要处理这个请求。

代码虽短，这个逻辑太复杂了。

# 小结

这里刚刚是一个皮毛，真正的 producer 处理流控逻辑还没有提到。下次有时间，专门分析一个真正的流控逻辑。

同时，我们也看到，最好不要自己写 operator 和 OnSubscribe ，而是调用现成的 `from` 系列函数， `createSync` 之类的提供流控的工厂方法，构造 Observable。
