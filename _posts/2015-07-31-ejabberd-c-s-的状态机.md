---
layout: post
title:  "ejabberd_c2s 的状态机"
date:   2015/07/31 18:51:22
categories:
comments: true
draft: true
---

# 从 [wait_for_stream](https://github.com/wcy123/ejabberd/blob/1048e21643cb610f112f8dc95d32e3230b819361/src/ejabberd_c2s.erl#L354) 开始。

客户的发送的第一条消息就是


{% highlight xml %}
<stream:stream
    to='localhost'
    xmlns:stream='http://etherx.jabber.org/streams'
    xmlns='jabber:client' version='1.0'>
{% endhighlight %}

调用 `wait_for_stream` 的参数是

{% highlight erl %}
call ejabberd_c2s:wait_for_stream(
 {xmlstreamstart,
  <<"stream:stream">>,
  [{<<"xmlns:stream">>, <<"http://etherx.jabber.org/streams">>},
   {<<"xmlns">>,<<"jabber:client">>},
   {<<"to">>,<<"localhost">>},
   {<<"version">>,<<"1.0">>}]}, State)
{% endhighlight %}

第一个参数也是 `ejabberd_receiver` 投递的消息。把 xml 转换为 erlang 的数据类型了。

### 检查 [NS_STREAM](https://github.com/wcy123/ejabberd/blob/1048e21643cb610f112f8dc95d32e3230b819361/src/ejabberd_c2s.erl#L357)

这个一般都能通过。

## 解析 Server 名字 [Server jlib:nameprep](https://github.com/wcy123/ejabberd/blob/1048e21643cb610f112f8dc95d32e3230b819361/src/ejabberd_c2s.erl#L361)

这里支持 `nameprep` 的功能。

## 解析语言标识 [Lang = case xml:get_attr_s(<<"xml:lang">>, Attrs) of](https://github.com/wcy123/ejabberd/blob/1048e21643cb610f112f8dc95d32e3230b819361/src/ejabberd_c2s.erl#L364)

这里有防止  DoS 攻击。

## [IsBlacklistedIP = is_ip_blacklisted(StateData#state.ip, Lang)](https://github.com/wcy123/ejabberd/blob/1048e21643cb610f112f8dc95d32e3230b819361/src/ejabberd_c2s.erl#L377)

检查 black list 运行 [check_bl_c2s](https://github.com/wcy123/ejabberd/blob/1048e21643cb610f112f8dc95d32e3230b819361/src/ejabberd_c2s.erl#L2524) 的钩子。

## 如果主机名称是否在配置的主机列表中

### 如果不在，则退出。

[{stop, normal, StateData}](https://github.com/wcy123/ejabberd/blob/1048e21643cb610f112f8dc95d32e3230b819361/src/ejabberd_c2s.erl#L524) 这个好远。

### [如果在的话](https://github.com/wcy123/ejabberd/blob/1048e21643cb610f112f8dc95d32e3230b819361/src/ejabberd_c2s.erl#L379-L515)

#### 改变 [change_shaper](https://github.com/wcy123/ejabberd/blob/1048e21643cb610f112f8dc95d32e3230b819361/src/ejabberd_c2s.erl#L380)。

参考 [acl:match_rule](https://github.com/wcy123/ejabberd/blob/fb6267f38ee47f3f725f88324d132741c85dfb6a/src/acl.erl#L261) ets 表格 [access](https://github.com/wcy123/ejabberd/blob/fb6267f38ee47f3f725f88324d132741c85dfb6a/src/acl.erl#L262)

对于 c2s 来说，[change_shaper](https://github.com/wcy123/ejabberd/blob/ba69c469b551e77929bd1a6eb3fd16a82ef13687/src/ejabberd_socket.erl#L201) 运行这里 [is_pid(SocketData#socket_state.receiver)](https://github.com/wcy123/ejabberd/blob/ba69c469b551e77929bd1a6eb3fd16a82ef13687/src/ejabberd_socket.erl#L202)。

然后调用这里 [ejabberd_receiver:change_shaper](https://github.com/wcy123/ejabberd/blob/51508a9fc34b4c048b64be8be93e1188f9f046ef/src/ejabberd_receiver.erl#L97)

然后这里
[State#state{shaper_state = NewShaperState}](https://github.com/wcy123/ejabberd/blob/51508a9fc34b4c048b64be8be93e1188f9f046ef/src/ejabberd_receiver.erl#L224)
，`ejabberd_receiver` 就有了一个新的 `shaper`

BTW: shaper is `normal` 根据简单的配置文件。

[TODO shaper](deadlink)


### 检查 [xml:get_attr_s(<<"version">>, Attrs)](https://github.com/wcy123/ejabberd/blob/1048e21643cb610f112f8dc95d32e3230b819361/src/ejabberd_c2s.erl#L381)

这个只有一个 pattern 可以匹配，不如写成

{% highlight erl %}
<<"1.0">> = xml:get_attr_s(<<"version">>, Attrs),
{% endhighlight %}

1. [send_header](https://github.com/wcy123/ejabberd/blob/1048e21643cb610f112f8dc95d32e3230b819361/src/ejabberd_c2s.erl#L383)

发送


{% highlight xml %}
<?xml version='1.0'?>
<stream:stream xmlns='jabber:client'
  xmlns:stream='http://etherx.jabber.org/streams'
  id='2540400772'
  from='localhost' version='1.0' xml:lang='en'>
{% endhighlight %}

这里的 stream id 在 [init](https://github.com/wcy123/ejabberd/blob/1048e21643cb610f112f8dc95d32e3230b819361/src/ejabberd_c2s.erl#L265) 的时候 [streamid = new_id()](https://github.com/wcy123/ejabberd/blob/1048e21643cb610f112f8dc95d32e3230b819361/src/ejabberd_c2s.erl#L334) 得到的。

2. 看是否有 [case StateData#state.authenticated](https://github.com/wcy123/ejabberd/blob/1048e21643cb610f112f8dc95d32e3230b819361/src/ejabberd_c2s.erl#L384) 过。

  第一次来的时候，还没有认证过。<a name="auth1">这里</a>
  参考<a href="#wait_for_feature_request1">wait_for_feature_request</a>

3. 准备 TLS, SASL, ZLIB

  TODO， 这些 feature 单独描述。这里先集中看 PLAIN

4. 创建[SASLState = cyrsasl:server_new](https://github.com/wcy123/ejabberd/blob/1048e21643cb610f112f8dc95d32e3230b819361/src/ejabberd_c2s.erl#L390) 仅为以后使用。

4. 基本的 feature 是 [cyrsasl:listmech(Server)](https://github.com/wcy123/ejabberd/blob/1048e21643cb610f112f8dc95d32e3230b819361/src/ejabberd_c2s.erl#L412)

```
(a@debian)48> cyrsasl:listmech(<<"localhost">>).
[<<"PLAIN">>,<<"DIGEST-MD5">>,<<"SCRAM-SHA-1">>, <<"ANONYMOUS">>]
```

   查询表格 [sasl_mechanism](https://github.com/wcy123/ejabberd/blob/fb6267f38ee47f3f725f88324d132741c85dfb6a/src/cyrsasl.erl#L148)  [这里](../../../07/30/ejabberd-%E7%9A%84%E5%90%AF%E5%8A%A8%E8%BF%87%E7%A8%8B/index.html#table_sasl_mechanism) 可以看到初始化的部分。

   [ejabberd_auth:store_type(Host)](https://github.com/wcy123/ejabberd/blob/fb6267f38ee47f3f725f88324d132741c85dfb6a/src/cyrsasl.erl#L151) 返回参数配置。

```
  (a@debian)50> ejabberd_auth:store_type(<<"localhost">>).
  plain
```


4. 运行钩子 [c2s_stream_features](https://github.com/wcy123/ejabberd/blob/1048e21643cb610f112f8dc95d32e3230b819361/src/ejabberd_c2s.erl#L457)

此时得到的 feature list 是  [StreamFeatures1 = TLSFeature ++ CompressFeature ++ Mechs](https://github.com/wcy123/ejabberd/blob/1048e21643cb610f112f8dc95d32e3230b819361/src/ejabberd_c2s.erl#L456) 。

钩子可以过滤。

4. [send_element](https://github.com/wcy123/ejabberd/blob/1048e21643cb610f112f8dc95d32e3230b819361/src/ejabberd_c2s.erl#L459)

发送


{% highlight xml %}
<stream:features>
<mechanisms
  xmlns='urn:ietf:params:xml:ns:xmpp-sasl'>
  <mechanism>PLAIN</mechanism>
  <mechanism>DIGEST-MD5</mechanism>
  <mechanism>SCRAM-SHA-1</mechanism>
  <mechanism>ANONYMOUS</mechanism>
</mechanisms>
</stream:features>"
{% endhighlight %}



# <a name="wait_for_request1"></a>[wait_for_feature_request](https://github.com/wcy123/ejabberd/blob/1048e21643cb610f112f8dc95d32e3230b819361/src/ejabberd_c2s.erl#L733)

这里先忽略管理模块 [when ?IS_STREAM_MGMT_TAG(Name)](https://github.com/wcy123/ejabberd/blob/1048e21643cb610f112f8dc95d32e3230b819361/src/ejabberd_c2s.erl#L730)

收到消息


{% highlight xml %}
<auth xmlns='urn:ietf:params:xml:ns:xmpp-sasl' mechanism='PLAIN'>
AHRlc3QAMTIz
</auth>
{% endhighlight %}


```
echo AHRlc3QAMTIz | base64 --decode | xxd
0000000: 0074 6573 7400 3132 33                   .test.123

```

转换为  Erlang 的消息，调用 [wait_for_feature_request](https://github.com/wcy123/ejabberd/blob/1048e21643cb610f112f8dc95d32e3230b819361/src/ejabberd_c2s.erl#L733)


{% highlight erl %}
{xmlstreamelement,
  {xmlel,<<"auth">>,
    [{<<"xmlns">>,<<"urn:ietf:params:xml:ns:xmpp-sasl">>},
     {<<"mechanism">>,<<"PLAIN">>}],
    [{xmlcdata,<<"AHRlc3QAMTIz">>}]}}
{% endhighlight %}

xml 转换为 erlang 的数据结构是 [#xmlel{name = Name, attrs = Attrs, children = Els} = El](https://github.com/wcy123/ejabberd/blob/1048e21643cb610f112f8dc95d32e3230b819361/src/ejabberd_c2s.erl#L735)

运行 [{?NS_SASL, <<"auth">>}](https://github.com/wcy123/ejabberd/blob/1048e21643cb610f112f8dc95d32e3230b819361/src/ejabberd_c2s.erl#L743) 这里。

[{?NS_TLS, <<"starttls">>}](https://github.com/wcy123/ejabberd/blob/1048e21643cb610f112f8dc95d32e3230b819361/src/ejabberd_c2s.erl#L806) 和 [{?NS_COMPRESS, <<"compress">>}](https://github.com/wcy123/ejabberd/blob/1048e21643cb610f112f8dc95d32e3230b819361/src/ejabberd_c2s.erl#L829) 的功能先忽略。


[ClientIn = jlib:decode_base64(xml:get_cdata(Els))](https://github.com/wcy123/ejabberd/blob/1048e21643cb610f112f8dc95d32e3230b819361/src/ejabberd_c2s.erl#L746) 一定是上面 shell 命令显示的，用户名和密码。


[cyrsasl:server_start](https://github.com/wcy123/ejabberd/blob/1048e21643cb610f112f8dc95d32e3230b819361/src/ejabberd_c2s.erl#L747) 开始验证密码。

首次按 double check 认证机制，[lists:member(Mech, listmech(State#sasl_state.myname))](https://github.com/wcy123/ejabberd/blob/fb6267f38ee47f3f725f88324d132741c85dfb6a/src/cyrsasl.erl#L171)

查表，得到认证模块 [[#sasl_mechanism{module = Module}]](https://github.com/wcy123/ejabberd/blob/fb6267f38ee47f3f725f88324d132741c85dfb6a/src/cyrsasl.erl#L176) 。

创建一个认证模块的实例，
[Module:mech_new](https://github.com/wcy123/ejabberd/blob/fb6267f38ee47f3f725f88324d132741c85dfb6a/src/cyrsasl.erl#L178)
。


[server_step(State#sasl_state{mech_mod = Module,](https://github.com/wcy123/ejabberd/blob/fb6267f38ee47f3f725f88324d132741c85dfb6a/src/cyrsasl.erl#L182) 检查密码。 `server_step` 调用虚函数 [Module:mech_step](https://github.com/wcy123/ejabberd/blob/fb6267f38ee47f3f725f88324d132741c85dfb6a/src/cyrsasl.erl#L193)

[cyrsasl_plain:mech_step](https://github.com/wcy123/ejabberd/blob/5a35405cd523127fcd051a38414529680b69505c/src/cyrsasl_plain.erl#L45)
返回的是 `{ok, Prop}` 。这个返回值好难找。就是虚函数 `[check_password_loop](https://github.com/wcy123/ejabberd/blob/fb6267f38ee47f3f725f88324d132741c85dfb6a/src/ejabberd_auth.erl#L159)` 的返回值。`Prop` 就是对应的 `auth` 模块。

 1. [cyrsasl:server_start](https://github.com/wcy123/ejabberd/blob/1048e21643cb610f112f8dc95d32e3230b819361/src/ejabberd_c2s.erl#L747)
 2. [cyrsasl:server_step](https://github.com/wcy123/ejabberd/blob/fb6267f38ee47f3f725f88324d132741c85dfb6a/src/cyrsasl.erl#L182)
 3. [Module:mech_step](https://github.com/wcy123/ejabberd/blob/fb6267f38ee47f3f725f88324d132741c85dfb6a/src/cyrsasl.erl#L193)
 4. Module = `cyrsasl_plain`
 5. [cyrsasl_plain:mech_step](https://github.com/wcy123/ejabberd/blob/5a35405cd523127fcd051a38414529680b69505c/src/cyrsasl_plain.erl#L45)
 6. [{ok, [{username, User}, {authzid, AuthzId}, {auth_module, AuthModule}]}](https://github.com/wcy123/ejabberd/blob/5a35405cd523127fcd051a38414529680b69505c/src/cyrsasl_plain.erl#L50)
 7. AuthModule 是 [(State#state.check_password)](https://github.com/wcy123/ejabberd/blob/5a35405cd523127fcd051a38414529680b69505c/src/cyrsasl_plain.erl#L48) 的返回值。
 8. `check_password` 是三个匿名函数中 [ejabberd_auth:check_password_with_authmodule](https://github.com/wcy123/ejabberd/blob/1048e21643cb610f112f8dc95d32e3230b819361/src/ejabberd_c2s.erl#L397) 。
 9. AuthModule 是 [ejabberd_auth:check_password_loop](https://github.com/wcy123/ejabberd/blob/fb6267f38ee47f3f725f88324d132741c85dfb6a/src/ejabberd_auth.erl#L146) 的返回值。
 10. 是一个认证成功的模块，[true -> {true, AuthModule}](https://github.com/wcy123/ejabberd/blob/fb6267f38ee47f3f725f88324d132741c85dfb6a/src/ejabberd_auth.erl#L161)

所以  Props 的返回值是一个 proplist ，其中有 `username`, `authzid`, `auth_module` 和 `auth_module`。

## [(StateData#state.sockmod):reset_stream(StateData#state.socket)](https://github.com/wcy123/ejabberd/blob/1048e21643cb610f112f8dc95d32e3230b819361/src/ejabberd_c2s.erl#L751) 。

sockmod 是 `ejabberd_socket`

###  [ejabberd_receiver:reset_stream](https://github.com/wcy123/ejabberd/blob/ba69c469b551e77929bd1a6eb3fd16a82ef13687/src/ejabberd_socket.erl#L172)

#### [do_call(Pid, reset_stream)](https://github.com/wcy123/ejabberd/blob/51508a9fc34b4c048b64be8be93e1188f9f046ef/src/ejabberd_receiver.erl#L102)

##### [handle_call(reset_stream, _From,](https://github.com/wcy123/ejabberd/blob/51508a9fc34b4c048b64be8be93e1188f9f046ef/src/ejabberd_receiver.erl#L195)

 1. [close(#xml_stream_state{port = Port})](https://github.com/processone/xml
 1a46ce78e663aa9d8c1588a750303f26c46cf470/src/xml_stream.erl#L222)
 问题，close 之前没有发送 `</stream>` ？

问题，应该不会调用，close ，为啥呢？

  1. [reset_stream(StateData#state.socket)](https://github.com/wcy123/ejabberd/blob/1048e21643cb610f112f8dc95d32e3230b819361/src/ejabberd_c2s.erl#L751) 中，state.socket 的值是 `{socket_state,gen_tcp,#Port<7424.13522>,<ejabberd_receiver>}`
  2. receiver 的状态隐藏了。
  3. ??? 奇怪。 TODO


## 运行钩子 [c2s_auth_result](https://github.com/wcy123/ejabberd/blob/1048e21643cb610f112f8dc95d32e3230b819361/src/ejabberd_c2s.erl#L760)

## [send_element](https://github.com/wcy123/ejabberd/blob/1048e21643cb610f112f8dc95d32e3230b819361/src/ejabberd_c2s.erl#L763)


{% highlight xml %}
<success xmlns='urn:ietf:params:xml:ns:xmpp-sasl'/>
{% endhighlight %}


## 下一个状态 [wait_for_stream](https://github.com/wcy123/ejabberd/blob/1048e21643cb610f112f8dc95d32e3230b819361/src/ejabberd_c2s.erl#L767) ，

这个时候，
[authenticated = true](https://github.com/wcy123/ejabberd/blob/1048e21643cb610f112f8dc95d32e3230b819361/src/ejabberd_c2s.erl#L769)
。


# `wait_for_stream`

收到新的消息。


{% highlight xml %}
<stream:stream
   to='localhost'
   xmlns:stream='http://etherx.jabber.org/streams'
   xmlns='jabber:client' version='1.0'>
{% endhighlight %}

调用

```
ejabberd_c2s:wait_for_stream(
 {xmlstreamstart,<<"stream:stream">>,
                [{<<"xmlns:stream">>,<<"http://etherx.jabber.org/streams">>},
                 {<<"xmlns">>,<<"jabber:client">>},
                 {<<"to">>,<<"localhost">>},
                 {<<"version">>,<<"1.0">>}]}
```
## 和上面一样检查 ip black list, change shaper , [send_header](https://github.com/wcy123/ejabberd/blob/1048e21643cb610f112f8dc95d32e3230b819361/src/ejabberd_c2s.erl#L383)

##  [StateData#state.authenticated](https://github.com/wcy123/ejabberd/blob/1048e21643cb610f112f8dc95d32e3230b819361/src/ejabberd_c2s.erl#L384) 的情况和上面不一样了，运行 [case StateData#state.resource](https://github.com/wcy123/ejabberd/blob/1048e21643cb610f112f8dc95d32e3230b819361/src/ejabberd_c2s.erl#L470)

问题，这里最好 double check `true` ，保证程序健壮。


## 如果没有 resource [<<"">> ->](https://github.com/wcy123/ejabberd/blob/1048e21643cb610f112f8dc95d32e3230b819361/src/ejabberd_c2s.erl#L471) 的情况

### 运行钩子 [roster_get_versioning_feature](https://github.com/wcy123/ejabberd/blob/1048e21643cb610f112f8dc95d32e3230b819361/src/ejabberd_c2s.erl#L473)

### 忽略 [StreamManagementFeature](https://github.com/wcy123/ejabberd/blob/1048e21643cb610f112f8dc95d32e3230b819361/src/ejabberd_c2s.erl#L476) ，返回 `true`

### 运行钩子 [c2s_stream_features](https://github.com/wcy123/ejabberd/blob/1048e21643cb610f112f8dc95d32e3230b819361/src/ejabberd_c2s.erl#L499)

### 发送数据  [ejabberd_c2s:send_element](https://github.com/wcy123/ejabberd/blob/1048e21643cb610f112f8dc95d32e3230b819361/src/ejabberd_c2s.erl#L501)


{% highlight erl %}
[{xmlel,
   <<"bind">>, %% name
   [{<<"xmlns">>,<<"urn:ietf:params:xml:ns:xmpp-bind">>}], %% attrs
   []  %% children
   },
 {xmlel,<<"session">>,  %% name
  [{<<"xmlns">>,<<"urn:ietf:params:xml:ns:xmpp-session">>}], %% attris
  []},
 {xmlel,<<"sm">>,[{<<"xmlns">>,<<"urn:xmpp:sm:2">>}],[]},
 {xmlel,<<"sm">>,[{<<"xmlns">>,<<"urn:xmpp:sm:3">>}],[]}]
{% endhighlight %}


{% highlight xml %}
<stream:features>
  <bind xmlns='urn:ietf:params:xml:ns:xmpp-bind'/>
  <session xmlns='urn:ietf:params:xml:ns:xmpp-session'/>
  <sm xmlns='urn:xmpp:sm:2'/>
  <sm xmlns='urn:xmpp:sm:3'/>
</stream:features>
{% endhighlight %}


[stream_mgmt_enabled(StateData)](https://github.com/wcy123/ejabberd/blob/1048e21643cb610f112f8dc95d32e3230b819361/src/ejabberd_c2s.erl#L477) 是 true 所以 sm2 sm3 存在。


看看这些 features 是从哪里来的。 [StreamManagementFeature](https://github.com/wcy123/ejabberd/blob/1048e21643cb610f112f8dc95d32e3230b819361/src/ejabberd_c2s.erl#L476) 是空的。

```
(a@debian)23> Server = <<"localhost">>.
<<"localhost">>
(a@debian)24>  ejabberd_hooks:run_fold(roster_get_versioning_feature, Server, [], [Server]).
[]
```

[attrs = [{<<"xmlns">>, ?NS_BIND}]](https://github.com/wcy123/ejabberd/blob/1048e21643cb610f112f8dc95d32e3230b819361/src/ejabberd_c2s.erl#L489) 和 [attrs = [{<<"xmlns">>, ?NS_SESSION}]](https://github.com/wcy123/ejabberd/blob/1048e21643cb610f112f8dc95d32e3230b819361/src/ejabberd_c2s.erl#L492) 是默认就有的。

下一个状态 [wait_for_bind](https://github.com/wcy123/ejabberd/blob/1048e21643cb610f112f8dc95d32e3230b819361/src/ejabberd_c2s.erl#L505)


# [wait_for_bind({xmlstreamelement, El}, StateData)](https://github.com/wcy123/ejabberd/blob/1048e21643cb610f112f8dc95d32e3230b819361/src/ejabberd_c2s.erl#L1046)

收到消息


{% highlight xml %}
<iq type="set" id="1">
  <bind xmlns="urn:ietf:params:xml:ns:xmpp-bind" />
</iq>
{% endhighlight %}


调用

```
ejabberd_c2s:wait_for_bind(
{xmlstreamelement,
 {xmlel,<<"iq">>,
        [{<<"type">>,<<"set">>},{<<"id">>,<<"1">>}],
        [{xmlel,<<"bind">>,
         [{<<"xmlns">>, <<"urn:ietf:params:xml:ns:xmpp-bind">>}],
         []}]}},{state, ...})
```

## 检查  [jlib:iq_query_info(El)](https://github.com/wcy123/ejabberd/blob/1048e21643cb610f112f8dc95d32e3230b819361/src/ejabberd_c2s.erl#L1047) 。

```
(a@debian)32> El = {xmlel,<<"iq">>, [{<<"type">>,<<"set">>},{<<"id">>,<<"1">>}], [{xmlel,<<"bind">>, [{<<"xmlns">>, <<"urn:ietf:params:xml:ns:xmpp-bind">>}], []}]}.
{xmlel,<<"iq">>,
       [{<<"type">>,<<"set">>},{<<"id">>,<<"1">>}],
       [{xmlel,<<"bind">>,
               [{<<"xmlns">>,<<"urn:ietf:params:xml:ns:xmpp-bind">>}],
               []}]}
(a@debian)33> jlib:iq_query_info(El).
{iq,<<"1">>,set,<<"urn:ietf:params:xml:ns:xmpp-bind">>,<<>>,
    {xmlel,<<"bind">>,
           [{<<"xmlns">>,<<"urn:ietf:params:xml:ns:xmpp-bind">>}],
           []}}

```

## 检查 [R1 = xml:get_path_s](https://github.com/wcy123/ejabberd/blob/1048e21643cb610f112f8dc95d32e3230b819361/src/ejabberd_c2s.erl#L1051)

```
(a@debian)34> SubEl = {xmlel,<<"bind">>, [{<<"xmlns">>,<<"urn:ietf:params:xml:ns:xmpp-bind">>}], []}.
{xmlel,<<"bind">>,
       [{<<"xmlns">>,<<"urn:ietf:params:xml:ns:xmpp-bind">>}],
       []}
(a@debian)35> xml:get_path_s(SubEl, [{elem, <<"resource">>}, cdata]).
<<>>
```

## 根据 [now()](https://github.com/wcy123/ejabberd/blob/1048e21643cb610f112f8dc95d32e3230b819361/src/ejabberd_c2s.erl#L1058) 来生成一个临时的 [jlib:resourceprep(R1)](https://github.com/wcy123/ejabberd/blob/1048e21643cb610f112f8dc95d32e3230b819361/src/ejabberd_c2s.erl#L1053) 。

```
(a@debian)36> iolist_to_binary([randoms:get_string()
                                        | [jlib:integer_to_binary(X)
                                           || X <- tuple_to_list(now())]]).
<<"18749916231438494959141559">>
(a@debian)37>
```

错误处理 [Err = jlib:make_error_reply](https://github.com/wcy123/ejabberd/blob/1048e21643cb610f112f8dc95d32e3230b819361/src/ejabberd_c2s.erl#L1063) 忽略。

## 资源冲突检测 [resource_conflict_action](https://github.com/wcy123/ejabberd/blob/1048e21643cb610f112f8dc95d32e3230b819361/src/ejabberd_c2s.erl#L1067)

### [ejabberd_sm:is_existing_resource(U, S, R)](https://github.com/wcy123/ejabberd/blob/1048e21643cb610f112f8dc95d32e3230b819361/src/ejabberd_c2s.erl#L1002)

```
(a@debian)37> ejabberd_sm:is_existing_resource(<<"test">>, <<"localhost">>, <<"18749916231438494959141559">>).
false
```


#### [get_resource_sessions](https://github.com/wcy123/ejabberd/blob/fb6267f38ee47f3f725f88324d132741c85dfb6a/src/ejabberd_sm.erl#L679)

##### [get_sm_backend](https://github.com/wcy123/ejabberd/blob/fb6267f38ee47f3f725f88324d132741c85dfb6a/src/ejabberd_sm.erl#L750)

有参数 [sm_db_type](https://github.com/wcy123/ejabberd/blob/fb6267f38ee47f3f725f88324d132741c85dfb6a/src/ejabberd_sm.erl#L751) 控制。

可行值

  - internal 和 mnesia 等价
  - mnesia
  - odbc
  - redi

默认值是 [mnesia](https://github.com/wcy123/ejabberd/blob/fb6267f38ee47f3f725f88324d132741c85dfb6a/src/ejabberd_sm.erl#L756)。

```
(a@debian)38> ejabberd_config:get_option(sm_db_type, fun(V) -> V end).
undefined
```

[list_to_atom("ejabberd_sm_" ++ atom_to_list(DBType))](https://github.com/wcy123/ejabberd/blob/fb6267f38ee47f3f725f88324d132741c85dfb6a/src/ejabberd_sm.erl#L757) 动态构造回调模块。


#### [Mod:get_sessions](https://github.com/wcy123/ejabberd/blob/fb6267f38ee47f3f725f88324d132741c85dfb6a/src/ejabberd_sm.erl#L686)

读取表 [session](https://github.com/wcy123/ejabberd/blob/a0fafc383a3088bb2a277aa127f90f16735a1569/src/ejabberd_sm_mnesia.erl#L77)。

 1. 启动看这里 [ejabberd_sm:start](https://github.com/wcy123/ejabberd/blob/dcf5aefea0363cc96d404591696e306fc323d8f5/src/ejabberd_app.erl#L64)。
 2. 读参数 [Mod = get_sm_backend](https://github.com/wcy123/ejabberd/blob/fb6267f38ee47f3f725f88324d132741c85dfb6a/src/ejabberd_sm.erl#L321)
 3. [Mod:init](https://github.com/wcy123/ejabberd/blob/fb6267f38ee47f3f725f88324d132741c85dfb6a/src/ejabberd_sm.erl#L322) 初始化对应的模块。
 4. 创建 ets 表格 [sm_iqtable](https://github.com/wcy123/ejabberd/blob/fb6267f38ee47f3f725f88324d132741c85dfb6a/src/ejabberd_sm.erl#L323)
 5. 注册钩子 [roster_in_subscription](https://github.com/wcy123/ejabberd/blob/fb6267f38ee47f3f725f88324d132741c85dfb6a/src/ejabberd_sm.erl#L326)
 6. 注册钩子 [offline_message_hook](https://github.com/wcy123/ejabberd/blob/fb6267f38ee47f3f725f88324d132741c85dfb6a/src/ejabberd_sm.erl#L328)
 7. 注册钩子  [remove_user](https://github.com/wcy123/ejabberd/blob/fb6267f38ee47f3f725f88324d132741c85dfb6a/src/ejabberd_sm.erl#L330)
 8. [ejabberd_commands:register_commands](https://github.com/wcy123/ejabberd/blob/fb6267f38ee47f3f725f88324d132741c85dfb6a/src/ejabberd_sm.erl#L333)
 9. [ejabberd_sm_mnesia:init](https://github.com/wcy123/ejabberd/blob/a0fafc383a3088bb2a277aa127f90f16735a1569/src/ejabberd_sm_mnesia.erl#L38)  运行  [gen_server:start_link({local, ?MODULE}, ?MODULE, [], [])](https://github.com/wcy123/ejabberd/blob/a0fafc383a3088bb2a277aa127f90f16735a1569/src/ejabberd_sm_mnesia.erl#L39)。这个让人费解。因为这个不符合 OTP 的设计风格。一眼看上去是有循环调用。实际上因为名字只能注册一次，所以没有问题，但这个风格不好。 错了，这里不会有循环调用，实际调用的是 [init/1([])](https://github.com/wcy123/ejabberd/blob/a0fafc383a3088bb2a277aa127f90f16735a1569/src/ejabberd_sm_mnesia.erl#L82)。 这里又有一个风格的问题，同名函数应该是有相关的，如果不相关，`init/0` 完全应该有一个不同的名字，例如  `start`。
 10. 初始化表格 [update_tables](https://github.com/wcy123/ejabberd/blob/a0fafc383a3088bb2a277aa127f90f16735a1569/src/ejabberd_sm_mnesia.erl#L83)
 11. [[ur, user, node] -> mnesia:delete_table(session)](https://github.com/wcy123/ejabberd/blob/a0fafc383a3088bb2a277aa127f90f16735a1569/src/ejabberd_sm_mnesia.erl#L126) 解决后相兼容的问题。
 12. 删除表格 [mnesia:delete_table(presence)](https://github.com/wcy123/ejabberd/blob/a0fafc383a3088bb2a277aa127f90f16735a1569/src/ejabberd_sm_mnesia.erl#L137) 。
 13. 删除表格 [mnesia:delete_table(local_session)](https://github.com/wcy123/ejabberd/blob/a0fafc383a3088bb2a277aa127f90f16735a1569/src/ejabberd_sm_mnesia.erl#L142)

     这里有一个风格问题，

     ```
     case lists:member(local_session, mnesia:system_info(tables)) of
	 true ->
	     mnesia:delete_table(local_session);
	 false ->
	     ok
     end.
     ```

     完全可以写成


     ```
     mnesia:delete_table(local_session);
     ```

     这是所谓的  defensive programming 和 let it crash 的区别。
 14. 创建表格 [session](https://github.com/wcy123/ejabberd/blob/a0fafc383a3088bb2a277aa127f90f16735a1569/src/ejabberd_sm_mnesia.erl#L84)

     这里也有一个风格问题，写成 `ok = mnesia:create_table(...)` 比较好，
     let it crash。

     这里有一个关键的架构问题，是否使用分布式数据库。mnesia 数据十分不
     好理解。从这里看，
     [ node() ](https://github.com/wcy123/ejabberd/blob/a0fafc383a3088bb2a277aa127f90f16735a1569/src/ejabberd_sm_mnesia.erl#L85)
     , ejabberd 似乎没有使用分布式数据库的功能。每个节点有自己独立的
     session table 。mnesia 数据库集群是通过 schema 中的 `db_nodes` 和
     `extra_db_nodes` 来决定的。启动 mnesia 之后，可以用
     `mnesia:change_config(extra_db_nodes)` 来实现。 ejabberd 似乎想用
     rpc 来实现可控的 session 查询，需要读 `ejabberd_route` 才能知道。
     这个问题比较难。需要分析分布式数据库和 rpc 之间的优缺点。 mnesia
     本身的复杂，导致理解上有困难，不敢用。原理是讲，是可以简化程序设
     计。但是能不能和其他的 session 管理方式了，例如 odbc 等等，无缝工
     作呢？性能上那一个好呢？强壮性？代码的可维护性？mnesia 分布式数据
     库可以支持多少 全局 table ? 会不会有瓶颈？如果有瓶颈，是不是可以
     放弃 transaction? 如果放弃 transaction ，是不是会出现一致性问题?
     初步感觉，现在的设计没有大问题。也许用 rpc 是一个好办法。现在的设
     计应给更好。

     表格格式 [-record(session, {sid, usr, us, priority, info})](https://github.com/wcy123/ejabberd/blob/9574e71e8db595ce7b2fa2f8fbfc38deec2ad74b/include/ejabberd_sm.hrl#L4)

     ```
     {sid, usr, us, priority, info}
     初步猜测，session id
     user
     server
     priority
     ```

 15. [session_counter](https://github.com/wcy123/ejabberd/blob/a0fafc383a3088bb2a277aa127f90f16735a1569/src/ejabberd_sm_mnesia.erl#L87)


     表格格式 [-record(session_counter, {vhost, count})](https://github.com/wcy123/ejabberd/blob/9574e71e8db595ce7b2fa2f8fbfc38deec2ad74b/include/ejabberd_sm.hrl#L5)。
 16. [mnesia:add_table_copy(session, node(), ram_copies)](https://github.com/wcy123/ejabberd/blob/a0fafc383a3088bb2a277aa127f90f16735a1569/src/ejabberd_sm_mnesia.erl#L92) 这一步是不是没有用啊。

 17. [mnesia:subscribe(system)](https://github.com/wcy123/ejabberd/blob/a0fafc383a3088bb2a277aa127f90f16735a1569/src/ejabberd_sm_mnesia.erl#L94) 这个啥意思？  为了监控系统时间 [handle_info({mnesia_system_event, {mnesia_down, Node}}, State)](https://github.com/wcy123/ejabberd/blob/a0fafc383a3088bb2a277aa127f90f16735a1569/src/ejabberd_sm_mnesia.erl#L104)

     这里奇怪，ets 函数也可以直接操作 mnesia table [ets:select_delete](https://github.com/wcy123/ejabberd/blob/a0fafc383a3088bb2a277aa127f90f16735a1569/src/ejabberd_sm_mnesia.erl#L105)。

     还有, `mnesia_down` 的事件是不是所有连接的 node 都能收到这个事件，
     而是特指 mnesia 集群里面的 nodes。不是说没有 mnesia 的集群，而是
     每一个 node 有自己的表格吗？那这个事件应该永远不会发生。

#### [resource_conflict_action](https://github.com/wcy123/ejabberd/blob/1048e21643cb610f112f8dc95d32e3230b819361/src/ejabberd_c2s.erl#L1001)

这个简单了，根据 priority 决定是否增加新的 session。


### 重复就关闭连接 [send_element(StateData, Err)](https://github.com/wcy123/ejabberd/blob/1048e21643cb610f112f8dc95d32e3230b819361/src/ejabberd_c2s.erl#L1075)

### 构造新的 Full JID [JID = jlib:make_jid(U, StateData#state.server, R2)](https://github.com/wcy123/ejabberd/blob/1048e21643cb610f112f8dc95d32e3230b819361/src/ejabberd_c2s.erl#L1078)

发送 [send_element(StateData, jlib:iq_to_xml(Res))](https://github.com/wcy123/ejabberd/blob/1048e21643cb610f112f8dc95d32e3230b819361/src/ejabberd_c2s.erl#L1089)


```
{xmlel,<<"iq">>,
   [{<<"id">>,<<"1">>},{<<"type">>,<<"result">>}],
    [{xmlel,<<"bind">>,
       [{<<"xmlns">>,<<"urn:ietf:params:xml:ns:xmpp-bind">>}],
       [{xmlel,<<"jid">>,[],
       [{xmlcdata,<<"test@localhost/31729980831438183620933095">>}]}]}]}
```



{% highlight xml %}
<iq id='1' type='result'>
   <bind xmlns='urn:ietf:params:xml:ns:xmpp-bind'>
      <jid>test@localhost/31729980831438183620933095</jid>
   </bind>
</iq>
{% endhighlight %}

bind 之后，用户就有自己的 resource , full JID 了。

### 下一个状态 [wait_for_session](https://github.com/wcy123/ejabberd/blob/1048e21643cb610f112f8dc95d32e3230b819361/src/ejabberd_c2s.erl#L1090)


# 状态 [wait_for_session({xmlstreamelement, El}, StateData)](https://github.com/wcy123/ejabberd/blob/1048e21643cb610f112f8dc95d32e3230b819361/src/ejabberd_c2s.erl#L1110)

收到 xml


{% highlight xml %}
<iq type=\"set\" id=\"2\">
   <session xmlns=\"urn:ietf:params:xml:ns:xmpp-session\" />
</iq>"
{% endhighlight %}

调用参数

```
ejabberd_c2s:wait_for_session(
{xmlstreamelement,
   {xmlel,<<"iq">>,
       [{<<"type">>,<<"set">>},{<<"id">>,<<"2">>}],
       [{xmlel,<<"session">>,
        [{<<"xmlns">>, <<"urn:ietf:params:xml:ns:xmpp-session">>}],
        []}]}})

```

## [update_num_stanzas_in(StateData, El)](https://github.com/wcy123/ejabberd/blob/1048e21643cb610f112f8dc95d32e3230b819361/src/ejabberd_c2s.erl#L1111)

更新变量 [StateData#state.mgmt_stanzas_in](https://github.com/wcy123/ejabberd/blob/1048e21643cb610f112f8dc95d32e3230b819361/src/ejabberd_c2s.erl#L2815)

## [jlib:iq_query_info(El)](https://github.com/wcy123/ejabberd/blob/1048e21643cb610f112f8dc95d32e3230b819361/src/ejabberd_c2s.erl#L1112)


```

(a@debian)41> El = {xmlel,<<"iq">>,
       [{<<"type">>,<<"set">>},{<<"id">>,<<"2">>}],
       [{xmlel,<<"session">>,
        [{<<"xmlns">>, <<"urn:ietf:params:xml:ns:xmpp-session">>}],
        []}]}.
{xmlel,<<"iq">>,
       [{<<"type">>,<<"set">>},{<<"id">>,<<"2">>}],
       [{xmlel,<<"session">>,
               [{<<"xmlns">>,<<"urn:ietf:params:xml:ns:xmpp-session">>}],
               []}]}
(a@debian)42> jlib:iq_query_info(El).
{iq,<<"2">>,set,<<"urn:ietf:params:xml:ns:xmpp-session">>,
    <<>>,
    {xmlel,<<"session">>,
           [{<<"xmlns">>,<<"urn:ietf:params:xml:ns:xmpp-session">>}],
           []}}
```

和  [{type = set, xmlns = ?NS_SESSION}](https://github.com/wcy123/ejabberd/blob/1048e21643cb610f112f8dc95d32e3230b819361/src/ejabberd_c2s.erl#L1113)  是一致的。



## [acl:match_rule](https://github.com/wcy123/ejabberd/blob/1048e21643cb610f112f8dc95d32e3230b819361/src/ejabberd_c2s.erl#L1117)

[NewStateData#state.access](https://github.com/wcy123/ejabberd/blob/1048e21643cb610f112f8dc95d32e3230b819361/src/ejabberd_c2s.erl#L1118) 是  c2s 。为啥呢

 1. init 中 [Access = case lists:keysearch(access, 1, Opts)](https://github.com/wcy123/ejabberd/blob/1048e21643cb610f112f8dc95d32e3230b819361/src/ejabberd_c2s.erl#L266) 这里初始化。
 2. 这里调用 [Module:start({?MODULE, SocketData}, Opts)](https://github.com/wcy123/ejabberd/blob/ba69c469b551e77929bd1a6eb3fd16a82ef13687/src/ejabberd_socket.erl#L104) 间接调用  init
 3. 这里传递 [CallMod:start(strip_frontend(Module), gen_tcp, Socket, Opts)](https://github.com/wcy123/ejabberd/blob/1cf2dfe63afe0994988e756dfbb82524f50f6af8/src/ejabberd_listener.erl#L309) Opts, CallMod 是 `ejabberd_c2s` 。
 4. 最终 Opts 是从参数里面读出来的 。[参考这里](../../../07/30/ejabberd-%E7%9A%84%E5%90%AF%E5%8A%A8%E8%BF%87%E7%A8%8B/index.html#c2s_access)


```
(a@debian)44> rr(jlib).
[iq,jid,rsm_in,rsm_out,scram,xmlel,xmlelement]
(a@debian)45> {jid,<<"test">>,<<"localhost">>,<<"31729980831438183620933095">>,
            <<"test">>,<<"localhost">>,<<"31729980831438183620933095">>}.
{jid,<<"test">>,<<"localhost">>,<<"31729980831438183620933095">>,
(a@debian)45>             <<"test">>,<<"localhost">>,<<"31729980831438183620933095">>}.
#jid{user = <<"test">>,server = <<"localhost">>,
     resource = <<"31729980831438183620933095">>,
     luser = <<"test">>,lserver = <<"localhost">>,
     lresource = <<"31729980831438183620933095">>}
(a@debian)46> acl:match_rule(<<"localhost">>,c2s, v(45)).
allow
(a@debian)47> table(access).
{atomic,[{access,{muc_create,global},[{local,allow}]},
         {access,{register,<<"localhost">>},[{all,allow}]},
         {access,{s2s_shaper,<<"localhost">>},[{all,fast}]},
         {access,{muc_create,<<"localhost">>},[{local,allow}]},
         {access,{max_user_sessions,<<"localhost">>},[{all,10}]},
         {access,{pubsub_createnode,<<"localhost">>},[{local,allow}]},
         {access,{muc,global},[{all,allow}]},
         {access,{max_user_offline_messages,global},
                 [{admin,5000},{all,100}]},
         {access,{c2s_shaper,global},[{admin,none},{all,normal}]},
         {access,{local,global},[{local,allow}]},
         {access,{local,<<"localhost">>},[{local,allow}]},
         {access,{muc_admin,<<"localhost">>},[{admin,allow}]},
         {access,{muc_admin,global},[{admin,allow}]},
         {access,{announce,<<"localhost">>},[{admin,allow}]},
         {access,{muc,<<"localhost">>},[{all,allow}]},
         {access,{c2s,<<"localhost">>},[{blocked,deny},{all,allow}]},
         {access,{configure,<<"localhost">>},[{admin,allow}]},
         {access,{c2s,global},[{blocked,deny},{all,allow}]},
         {access,{announce,global},[{admin,allow}]},
         {access,{trusted_network,global},[{loopback,allow}]},
         {access,{configure,global},[{admin,allow}]},
         {access,{pubsub_createnode,global},[{local,...}]},
         {access,{max_user_offline_messages,<<...>>},[{...}|...]},
         {access,{c2s_shaper,...},[...]},
         {access,{...},...},
         {access,...},
         {...}|...]}
(a@debian)50> io:format("~p~n",[ets:tab2list(access)]).
[{access,{muc_create,global},[{local,allow}]},
 {access,{register,<<"localhost">>},[{all,allow}]},
 {access,{s2s_shaper,<<"localhost">>},[{all,fast}]},
 {access,{muc_create,<<"localhost">>},[{local,allow}]},
 {access,{max_user_sessions,<<"localhost">>},[{all,10}]},
 {access,{pubsub_createnode,<<"localhost">>},[{local,allow}]},
 {access,{muc,global},[{all,allow}]},
 {access,{max_user_offline_messages,global},[{admin,5000},{all,100}]},
 {access,{c2s_shaper,global},[{admin,none},{all,normal}]},
 {access,{local,global},[{local,allow}]},
 {access,{local,<<"localhost">>},[{local,allow}]},
 {access,{muc_admin,<<"localhost">>},[{admin,allow}]},
 {access,{muc_admin,global},[{admin,allow}]},
 {access,{announce,<<"localhost">>},[{admin,allow}]},
 {access,{muc,<<"localhost">>},[{all,allow}]},
 {access,{c2s,<<"localhost">>},[{blocked,deny},{all,allow}]},
 {access,{configure,<<"localhost">>},[{admin,allow}]},
 {access,{c2s,global},[{blocked,deny},{all,allow}]},
 {access,{announce,global},[{admin,allow}]},
 {access,{trusted_network,global},[{loopback,allow}]},
 {access,{configure,global},[{admin,allow}]},
 {access,{pubsub_createnode,global},[{local,allow}]},
 {access,{max_user_offline_messages,<<"localhost">>},[{admin,5000},{all,100}]},
 {access,{c2s_shaper,<<"localhost">>},[{admin,none},{all,normal}]},
 {access,{s2s_shaper,global},[{all,fast}]},
 {access,{max_user_sessions,global},[{all,10}]},
 {access,{trusted_network,<<"localhost">>},[{loopback,allow}]},
 {access,{register,global},[{all,allow}]}]
ok
(a@debian)51>
```


初始化过程 [参考这里](../../../07/30/ejabberd-%E7%9A%84%E5%90%AF%E5%8A%A8%E8%BF%87%E7%A8%8B/index.html#acl_init)

先忽略这个权限管理部分。


## [jlib:make_result_iq_reply(El#xmlel{children = []})](https://github.com/wcy123/ejabberd/blob/1048e21643cb610f112f8dc95d32e3230b819361/src/ejabberd_c2s.erl#L1123)


```
(a@debian)51> jlib:make_result_iq_reply(El#xmlel{children = []}).
#xmlel{name = <<"iq">>,
       attrs = [{<<"type">>,<<"result">>},{<<"id">>,<<"2">>}],
       children = []}

```

## [send_stanza(NewStateData, Res)](https://github.com/wcy123/ejabberd/blob/1048e21643cb610f112f8dc95d32e3230b819361/src/ejabberd_c2s.erl#L1124)

最后发送的结果是


{% highlight xml %}
<iq type='result' id='2'/>
{% endhighlight %}


### [csi_filter_stanza](https://github.com/wcy123/ejabberd/blob/1048e21643cb610f112f8dc95d32e3230b819361/src/ejabberd_c2s.erl#L1913)

#### 运行钩子 [csi_filter_stanza](https://github.com/wcy123/ejabberd/blob/1048e21643cb610f112f8dc95d32e3230b819361/src/ejabberd_c2s.erl#L3031)

得到 action 为， queue, drop, or send

这里似乎看不太懂，参考 XEP-0352。


### [change_shaper(NewState, JID)](https://github.com/wcy123/ejabberd/blob/1048e21643cb610f112f8dc95d32e3230b819361/src/ejabberd_c2s.erl#L1125)

这个有啥用。

### 运行钩子  [roster_get_subscription_lists](https://github.com/wcy123/ejabberd/blob/1048e21643cb610f112f8dc95d32e3230b819361/src/ejabberd_c2s.erl#L1127) 。

```
(a@debian)54> ejabberd_hooks:run_fold(roster_get_subscription_lists, <<"localhost">>, {[], []}, [<<"test">>, <<"localhost">>]).
{[{<<"test1">>,<<"localhost">>,<<>>}],
[{<<"test1">>,<<"localhost">>,<<>>}]}
(a@debian)55> ejabberd_hooks:run_fold(roster_get_subscription_lists, <<"localhost">>, {[], []}, [<<"test1">>, <<"localhost">>]).
{[{<<"test">>,<<"localhost">>,<<>>}],
 [{<<"test">>,<<"localhost">>,<<>>}]}
(a@debian)57> JID = {jid,<<"test">>,<<"localhost">>,<<"31729980831438183620933095">>, <<"test">>,<<"localhost">>,<<"31729980831438183620933095">>}.
#jid{user = <<"test">>,server = <<"localhost">>,
     resource = <<"31729980831438183620933095">>,
     luser = <<"test">>,lserver = <<"localhost">>,
     lresource = <<"31729980831438183620933095">>}
(a@debian)58> jlib:jid_tolower(jlib:jid_remove_resource(JID)).
{<<"test">>,<<"localhost">>,<<>>}

```


### 运行钩子 [privacy_get_user_list](https://github.com/wcy123/ejabberd/blob/1048e21643cb610f112f8dc95d32e3230b819361/src/ejabberd_c2s.erl#L1136)


```
(a@debian)60> rr(ejabberd_c2s).
[iq,jid,listitem,privacy,rsm_in,rsm_out,scram,socket_state,
 state,userlist,xmlel,xmlelement]
(a@debian)61> ejabberd_hooks:run_fold(privacy_get_user_list, <<"localhost">>, #userlist{}, [<<"test">>, <<"localhost">>]).
#userlist{name = none,list = [],needdb = false}
```

### [get_conn_type(NewState)](https://github.com/wcy123/ejabberd/blob/1048e21643cb610f112f8dc95d32e3230b819361/src/ejabberd_c2s.erl#L1139)


参考这里 [get_conn_type(StateData)](https://github.com/wcy123/ejabberd/blob/1048e21643cb610f112f8dc95d32e3230b819361/src/ejabberd_c2s.erl#L2018)

Conn 应该是 `c2s`


### 构造 info ， 这就是  session table 中的  info 那一列


[ [{ip, NewState#state.ip}, {conn, Conn}, {auth_module, NewState#state.auth_module}] ](https://github.com/wcy123/ejabberd/blob/1048e21643cb610f112f8dc95d32e3230b819361/src/ejabberd_c2s.erl#L1140)

### 开会， [ejabberd_sm:open_session](https://github.com/wcy123/ejabberd/blob/1048e21643cb610f112f8dc95d32e3230b819361/src/ejabberd_c2s.erl#L1142)

 1. 调用 [open_session](https://github.com/wcy123/ejabberd/blob/fb6267f38ee47f3f725f88324d132741c85dfb6a/src/ejabberd_sm.erl#L135)
 2. 调用 [open_session(SID, User, Server, Resource, Priority, Info)](https://github.com/wcy123/ejabberd/blob/fb6267f38ee47f3f725f88324d132741c85dfb6a/src/ejabberd_sm.erl#L125l)
 3. [set_session(SID, User, Server, Resource, Priority, Info)](https://github.com/wcy123/ejabberd/blob/fb6267f38ee47f3f725f88324d132741c85dfb6a/src/ejabberd_sm.erl#L126)
 4. 调用对应模块的 [Mod:set_session](https://github.com/wcy123/ejabberd/blob/fb6267f38ee47f3f725f88324d132741c85dfb6a/src/ejabberd_sm.erl#L419)
 5. [mnesia:dirty_write(Session)](https://github.com/wcy123/ejabberd/blob/a0fafc383a3088bb2a277aa127f90f16735a1569/src/ejabberd_sm_mnesia.erl#L48)


session id 是怎么得到的？

```
(a@debian)65> State = {state, {socket_state, gen_tcp, port, ejabberd_receiver_pid}, ejabberd_socket, ref, false, <<"1077953735">>, undefined,c2s, c2s_shaper,false, false,false,false, [verify_none, compression_none], true, {jid,<<"test">>, <<"localhost">>, <<"31729980831438183620933095">>, <<"test">>, <<"localhost">>, <<"31729980831438183620933095">>}, <<"test">>, <<"localhost">>, <<"31729980831438183620933095">>, {{1438,183620, 762672}, ejabberd_c2s_pid}, {0,nil}, {0,nil}, {0,nil}, undefined, undefined, {userlist,none, [],false}, unknown, ejabberd_auth_internal, {{127,0,0,1}, 56034}, [],active,[], inactive, undefined, undefined,500, undefined,300, false,0,0,<<>>}.
#state{socket = #socket_state{sockmod = gen_tcp,
                              socket = port,receiver = ejabberd_receiver_pid},
       sockmod = ejabberd_socket,socket_monitor = ref,
       xml_socket = false,streamid = <<"1077953735">>,
       sasl_state = undefined,access = c2s,shaper = c2s_shaper,
       zlib = false,tls = false,tls_required = false,
       tls_enabled = false,
       tls_options = [verify_none,compression_none],
       authenticated = true,
       jid = #jid{user = <<"test">>,server = <<"localhost">>,
                  resource = <<"31729980831438183620933095">>,
                  luser = <<"test">>,lserver = <<"localhost">>,
                  lresource = <<"317299808314381836209330"...>>},
       user = <<"test">>,server = <<"localhost">>,
       resource = <<"31729980831438183620933095">>,
       sid = {{1438,183620,762672},ejabberd_c2s_pid},
       pres_t = {0,nil},
       pres_f = {0,nil},
       pres_a = {0,nil},
       pres_last = undefined,pres_timestamp = undefined,
       privacy_list = #userlist{name = none,list = [],...},
       conn = unknown,auth_module = ejabberd_auth_internal,
       ip = {...},...}
(a@debian)66> State#state.sid.
{{1438,183620,762672},ejabberd_c2s_pid}
```


 初始化 [init([{SockMod, Socket}, Opts])](https://github.com/wcy123/ejabberd/blob/1048e21643cb610f112f8dc95d32e3230b819361/src/ejabberd_c2s.erl#L265) 的时候，[sid = {now(), self()}](https://github.com/wcy123/ejabberd/blob/1048e21643cb610f112f8dc95d32e3230b819361/src/ejabberd_c2s.erl#L334)


下一个状态 [session_established](https://github.com/wcy123/ejabberd/blob/1048e21643cb610f112f8dc95d32e3230b819361/src/ejabberd_c2s.erl#L1150)


# [session_established](https://github.com/wcy123/ejabberd/blob/1048e21643cb610f112f8dc95d32e3230b819361/src/ejabberd_c2s.erl#L1191)

收到消息

{% highlight xml %}
<presence from="test@localhost/31729980831438183620933095" />
{% endhighlight %}

调用的是

```
{xmlstreamelement,
                                {xmlel,<<"presence">>,
                                 [{<<"from">>,
                                   <<"test@localhost/31729980831438183620933095">>}],
                                 []}}
```
## [check_from(El, FromJID)](https://github.com/wcy123/ejabberd/blob/1048e21643cb610f112f8dc95d32e3230b819361/src/ejabberd_c2s.erl#L1194)

这个似乎没啥用。直接跳到 [session_established2](https://github.com/wcy123/ejabberd/blob/1048e21643cb610f112f8dc95d32e3230b819361/src/ejabberd_c2s.erl#L1200)


## [update_num_stanzas_in](https://github.com/wcy123/ejabberd/blob/1048e21643cb610f112f8dc95d32e3230b819361/src/ejabberd_c2s.erl#L1231)

不是管理包，先忽略。

## [To = xml:get_attr_s(<<"to">>, Attrs)](https://github.com/wcy123/ejabberd/blob/1048e21643cb610f112f8dc95d32e3230b819361/src/ejabberd_c2s.erl#L1235)



```
(a@debian)71> xml:get_attr_s(<<"to">>, [{<<"from">>,
                                   <<"test@localhost/31729980831438183620933095">>}]).
<<>>
```

没有的话，就是 [jlib:make_jid(User, Server, <<"">>)](https://github.com/wcy123/ejabberd/blob/1048e21643cb610f112f8dc95d32e3230b819361/src/ejabberd_c2s.erl#L1237)


```
(a@debian)72> jlib:make_jid(<<"test">>, <<"localhost">>, <<"">>)
#jid{user = <<"test">>,server = <<"localhost">>,
     resource = <<>>,luser = <<"test">>,
     lserver = <<"localhost">>,lresource = <<>>}
```

## [NewEl1 = jlib:remove_attr(<<"xmlns">>, El)](https://github.com/wcy123/ejabberd/blob/1048e21643cb610f112f8dc95d32e3230b819361/src/ejabberd_c2s.erl#L1240)

```
(a@debian)76> El = #xmlel{name = <<"presence">>, attrs = [{<<"from">>, <<"test@localhost/31729980831438183620933095">>}], children = []}.
#xmlel{name = <<"presence">>,
       attrs = [{<<"from">>,
                 <<"test@localhost/31729980831438183620933095">>}],
       children = []}
(a@debian)77> NewEl1 = jlib:remove_attr(<<"xmlns">>, El).
#xmlel{name = <<"presence">>,
       attrs = [{<<"from">>,
                 <<"test@localhost/31729980831438183620933095">>}],
       children = []}
```

不知道为啥用。

## [NewEl = case xml:get_attr_s(<<"xml:lang">>, Attrs) of](https://github.com/wcy123/ejabberd/blob/1048e21643cb610f112f8dc95d32e3230b819361/src/ejabberd_c2s.erl#L1241)

设置语言。


## [<<"presence">>](https://github.com/wcy123/ejabberd/blob/1048e21643cb610f112f8dc95d32e3230b819361/src/ejabberd_c2s.erl#L1262) 的处理分支

### 运行钩子 [c2s_update_presence](https://github.com/wcy123/ejabberd/blob/1048e21643cb610f112f8dc95d32e3230b819361/src/ejabberd_c2s.erl#L1264)

### 运行钩子 [user_send_packet](https://github.com/wcy123/ejabberd/blob/1048e21643cb610f112f8dc95d32e3230b819361/src/ejabberd_c2s.erl#L1269)

### Resource 为空, 运行 [presence_update](https://github.com/wcy123/ejabberd/blob/1048e21643cb610f112f8dc95d32e3230b819361/src/ejabberd_c2s.erl#L1276)。

User updates his presence (non-directed presence packet)


```
%% FromJID =
(a@debian)78> {jid,<<"test">>,<<"localhost">>,<<"31729980831438183620933095">>,<<"test">>, <<"localhost">>,<<"31729980831438183620933095">>}.
#jid{user = <<"test">>,server = <<"localhost">>,
     resource = <<"31729980831438183620933095">>,
     luser = <<"test">>,lserver = <<"localhost">>,
     lresource = <<"31729980831438183620933095">>}


### [xml:get_attr_s(<<"type">>, Attrs)](https://github.com/wcy123/ejabberd/blob/1048e21643cb610f112f8dc95d32e3230b819361/src/ejabberd_c2s.erl#L2070)

是空的，所以运行 [_ ->](https://github.com/wcy123/ejabberd/blob/1048e21643cb610f112f8dc95d32e3230b819361/src/ejabberd_c2s.erl#L2093)。

### [get_priority_from_presence(OldPresence)](https://github.com/wcy123/ejabberd/blob/1048e21643cb610f112f8dc95d32e3230b819361/src/ejabberd_c2s.erl#L2096)

ejabberd_c2s:get_priority_from_presence/1 -> 0


### [update_priority(NewPriority, Packet, StateData)](https://github.com/wcy123/ejabberd/blob/1048e21643cb610f112f8dc95d32e3230b819361/src/ejabberd_c2s.erl#L2099)

### [ejabberd_sm:set_presence](https://github.com/wcy123/ejabberd/blob/1048e21643cb610f112f8dc95d32e3230b819361/src/ejabberd_c2s.erl#L2327)

更新数据库，奇怪，presence 的属性不在数据库中啊。


#### 运行钩子
[set_presence_hook](https://github.com/wcy123/ejabberd/blob/fb6267f38ee47f3f725f88324d132741c85dfb6a/src/ejabberd_sm.erl#L228)
。



### 更新状态 [NewStateData = StateData#state{pres_last = Packet, pres_timestamp = now()}](https://github.com/wcy123/ejabberd/blob/1048e21643cb610f112f8dc95d32e3230b819361/src/ejabberd_c2s.erl#L2102)

### 运行钩子 [user_available_hook](https://github.com/wcy123/ejabberd/blob/1048e21643cb610f112f8dc95d32e3230b819361/src/ejabberd_c2s.erl#L2105)

从 unavailabe 到 avaialbe 的时候，运行这个钩子。

### 发送离线消息 [resend_offline_messages](https://github.com/wcy123/ejabberd/blob/1048e21643cb610f112f8dc95d32e3230b819361/src/ejabberd_c2s.erl#L2109)


#### 运行钩子 [resend_offline_messages_hook](https://github.com/wcy123/ejabberd/blob/1048e21643cb610f112f8dc95d32e3230b819361/src/ejabberd_c2s.erl#L2378)

#### 隐私检查[privacy_check_packet](https://github.com/wcy123/ejabberd/blob/1048e21643cb610f112f8dc95d32e3230b819361/src/ejabberd_c2s.erl#L2385)

运行钩子 [privacy_check_packet](https://github.com/wcy123/ejabberd/blob/1048e21643cb610f112f8dc95d32e3230b819361/src/ejabberd_c2s.erl#L2183) 。


风格问题，


```
                 Pass = case privacy_check_packet(StateData,
								 From, To,
								 Packet, in)
					   of
					 allow -> true;
					 deny -> false
				       end,
				if Pass ->
				       ejabberd_router:route(From, To, Packet);
				   true -> ok
				end
```

可以简化为


```
   if is_privacy_allow((StateData, From, To, Packet, in) ->
      ejabberd_router:route(From, To, Packet);
   end
```

c2s 本身不处理离线消息的功能，留给钩子模块处理。

### 更新订阅状态 [resend_subscription_requests](https://github.com/wcy123/ejabberd/blob/1048e21643cb610f112f8dc95d32e3230b819361/src/ejabberd_c2s.erl#L2110)

这个结构和  resend offline message 类似。

#### 运行钩子 [resend_subscription_requests_hook](https://github.com/wcy123/ejabberd/blob/1048e21643cb610f112f8dc95d32e3230b819361/src/ejabberd_c2s.erl#L2403)

#### 这里调用 [send_packet](https://github.com/wcy123/ejabberd/blob/1048e21643cb610f112f8dc95d32e3230b819361/src/ejabberd_c2s.erl#L2406)

而没有直接调用 ejabberd_route:route 函数。 send packet 根据消息类型 [is_stanza(Packet)](https://github.com/wcy123/ejabberd/blob/1048e21643cb610f112f8dc95d32e3230b819361/src/ejabberd_c2s.erl#L1924)，调用  [send_stanza](https://github.com/wcy123/ejabberd/blob/1048e21643cb610f112f8dc95d32e3230b819361/src/ejabberd_c2s.erl#L1926) 或者  [send_element](https://github.com/wcy123/ejabberd/blob/1048e21643cb610f112f8dc95d32e3230b819361/src/ejabberd_c2s.erl#L1928) 。 send element 不改变内部状态， send stanza 使用了 [csi_filter_stanza](https://github.com/wcy123/ejabberd/blob/1048e21643cb610f112f8dc95d32e3230b819361/src/ejabberd_c2s.erl#L1913) 。

message, presence, iq 消息，而且带有 client, 或者 server name space 标志的 xml packet 都是 stanza。

### [presence_broadcast_first](https://github.com/wcy123/ejabberd/blob/1048e21643cb610f112f8dc95d32e3230b819361/src/ejabberd_c2s.erl#L2113)

### 如果不是从  unavaliable 过来的，

#### [presence_broadcast_to_trusted](https://github.com/wcy123/ejabberd/blob/1048e21643cb610f112f8dc95d32e3230b819361/src/ejabberd_c2s.erl#L2116)

### [resend_offline_messages](https://github.com/wcy123/ejabberd/blob/1048e21643cb610f112f8dc95d32e3230b819361/src/ejabberd_c2s.erl#L2121)

## 运行钩子 [c2s_loop_debug](https://github.com/wcy123/ejabberd/blob/1048e21643cb610f112f8dc95d32e3230b819361/src/ejabberd_c2s.erl#L1305)

下一个状态 [session_established](https://github.com/wcy123/ejabberd/blob/1048e21643cb610f112f8dc95d32e3230b819361/src/ejabberd_c2s.erl#L1307)。


还是自己的状态。
