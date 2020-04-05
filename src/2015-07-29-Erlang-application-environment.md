# Erlang application environment



我们知道 Erlang 的函数 `application:get_env/2` 可以返回 app 的配置信息。
例如我们有一个简单的 app 文件，放在 `./ebin/myapp.ebin` 中。


```erlang
{application,myapp,
             [{description,[]},
              {vsn,"1"},
              {registered,[]},
              {applications,[kernel,stdlib]},
              {mod,{myapp_app,[]}},
              {env,[{a,1},{b,2}]},
              {modules,[myapp_app,myapp_server,myapp_sup]}]}.
```


```erlang-repl
bash$ erl -pa ebin
Erlang/OTP 18 [erts-7.0] [source] [64-bit] [async-threads:10] [hipe] [kernel-poll:false]

Eshell V7.0  (abort with ^G)
1> application:load(myapp).
ok
2> application:get_env(myapp,a).
{ok,1}
3> application:get_env(myapp,b).
{ok,2}
4>
```

除此之外，我们可以通过命令行来修改 app env 。

```erlang-repl
bash$ erl -pa ebin -myapp a first b second
Erlang/OTP 18 [erts-7.0] [source] [64-bit] [async-threads:10] [hipe] [kernel-poll:false]

Eshell V7.0  (abort with ^G)
1> application:get_env(myapp,a).
undefined
2> application:load(myapp).
ok
3> application:get_env(myapp,a).
{ok,first}
4> application:get_env(myapp,b).
{ok,second}
5>
```

注意到在 `application:load(myapp)` 之前，`application:get_env(myapp)` 都是返回 `undefined`。

我们也注意到，命令行的配置，覆盖掉了原来 `myapp.app` 中的默认配置。

还有一种办法可以指定 app 环境，就是利用配置文件。例如，我们写一个配置文件 `a.config`，如下

```erlang
[{myapp,
  [{a,one},
   {b,two},
   {c,yes}]}].
```

然后执行命令


```erlang-repl
bash$ erl -pa ebin -config a.config
Erlang/OTP 18 [erts-7.0] [source] [64-bit] [async-threads:10] [hipe] [kernel-poll:false]

Eshell V7.0  (abort with ^G)
1> application:get_env(myapp,a).
undefined
2> application:load(myapp).
ok
3> application:get_env(myapp,a).
{ok,one}
4> application:get_env(myapp,b).
{ok,two}
5> application:get_env(myapp,c).
{ok,yes}
6>
```

可以看到，我们还可以指定新的参数, `c`。


配置文件 `a.config` 可以 `myapp.app` 中的配置，命令行参数 `-myapp a
yi` 可以覆盖配置文件中的设置。


```erlang-repl
bash$ erl -pa ebin -config a.config -myapp a yi
Erlang/OTP 18 [erts-7.0] [source] [64-bit] [async-threads:10] [hipe] [kernel-poll:false]

Eshell V7.0  (abort with ^G)
1> application:load(myapp).
ok
2> application:get_env(myapp,a).
{ok,yi}
3> application:get_env(myapp,b).
{ok,two}

```
