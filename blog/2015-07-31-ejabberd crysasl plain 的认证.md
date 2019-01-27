---
layout: post
title:  "ejabberd crysasl plain 的认证"
date:   2015/07/31 20:48:14
categories:
draft: true
comments: true
---

# 启动时的时候注入表格

注入表格 [sasl_mechanism](https://github.com/wcy123/ejabberd/blob/fb6267f38ee47f3f725f88324d132741c85dfb6a/src/cyrsasl.erl#L148)  [这里](../../../07/30/ejabberd-%E7%9A%84%E5%90%AF%E5%8A%A8%E8%BF%87%E7%A8%8B/index.html#table_sasl_mechanism) 可以看到初始化的部分。

## 调用 [cyrsasl:register_mechanism(<<"PLAIN">>, ?MODULE, plain)](https://github.com/wcy123/ejabberd/blob/5a35405cd523127fcd051a38414529680b69505c/src/cyrsasl_plain.erl#L37)



### [register_mechanism](https://github.com/wcy123/ejabberd/blob/fb6267f38ee47f3f725f88324d132741c85dfb6a/src/cyrsasl.erl#L98)

#### 检查是否已经禁止 [is_disabled(Mechanism)](https://github.com/wcy123/ejabberd/blob/fb6267f38ee47f3f725f88324d132741c85dfb6a/src/cyrsasl.erl#L99)

 有参数
 [disable_sasl_mechanisms](https://github.com/wcy123/ejabberd/blob/fb6267f38ee47f3f725f88324d132741c85dfb6a/src/cyrsasl.erl#L236)
 控制。参数名称不分大小写。参见
 [str:to_upper](https://github.com/wcy123/ejabberd/blob/fb6267f38ee47f3f725f88324d132741c85dfb6a/src/cyrsasl.erl#L238)
 。

 注意，这里不支持 per host 的配置。


  例如
  ```
  (a@debian)52> ejabberd_config:get_option(disable_sasl_mechanisms, fun(V)->V end).
  undefined
  ```

```
#sasl_mechanism{mechanism=<<"PLAIN">>, module=crysasl_plain, password_type=plain}
```

# [mech_new](https://github.com/wcy123/ejabberd/blob/5a35405cd523127fcd051a38414529680b69505c/src/cyrsasl_plain.erl#L42)
  有三个输出函数 `get_password`, `check_passwd` 和
  `check_passord_digest`。 [参考这里](https://github.com/wcy123/ejabberd/blob/1048e21643cb610f112f8dc95d32e3230b819361/src/ejabberd_c2s.erl#L392-L403)

对于 plain 来讲，只有
[CheckPassword](https://github.com/wcy123/ejabberd/blob/5a35405cd523127fcd051a38414529680b69505c/src/cyrsasl_plain.erl#L42)
是有意义的。这个 state 会存储在 c2s 的状态里面。



# [parse](https://github.com/wcy123/ejabberd/blob/5a35405cd523127fcd051a38414529680b69505c/src/cyrsasl_plain.erl#L72)

```
(a@debian)61> cyrsasl_plain:parse(<<"user@domain\0user\0passwd">>).
[<<"user@domain">>,<<"user">>,<<"passwd">>]
(a@debian)55> cyrsasl_plain:parse(<<"\0user\0passwd">>).
[<<>>,<<"user">>,<<"passwd">>]
```

[[<<"">>, UserMaybeDomain, Password]](https://github.com/wcy123/ejabberd/blob/5a35405cd523127fcd051a38414529680b69505c/src/cyrsasl_plain.erl#L60) 可以得到 UserMaybeDomain 和 Password。




# [parse_domain(UserMaybeDomain)](https://github.com/wcy123/ejabberd/blob/5a35405cd523127fcd051a38414529680b69505c/src/cyrsasl_plain.erl#L61)

```
(a@debian)59> cyrsasl_plain:parse_domain(<<"user@domain">>).
[<<"user">>,<<"domain">>]
(a@debian)60> cyrsasl_plain:parse_domain(<<"user">>).
[<<"user">>]
```
# plain 不关心 [_Domain](https://github.com/wcy123/ejabberd/blob/5a35405cd523127fcd051a38414529680b69505c/src/cyrsasl_plain.erl#L63) 部分。

# 问题

1. 似乎没有必要使用那三个匿名函数。不如直接调用
   [ejabberd_auth:check_password_with_authmodule](https://github.com/wcy123/ejabberd/blob/1048e21643cb610f112f8dc95d32e3230b819361/src/ejabberd_c2s.erl#L401)
   我们得到了灵活性，失去了可读性。多余绕这么一个弯弯。


2. plain 折腾了半天，似乎也没有干啥。还是要调用
   [ejabberd_auth:check_password_with_authmodule](https://github.com/wcy123/ejabberd/blob/1048e21643cb610f112f8dc95d32e3230b819361/src/ejabberd_c2s.erl#L401)
   来检查密码。



# 对于 plain 只用，[check_password_with_authmodule/3](https://github.com/wcy123/ejabberd/blob/fb6267f38ee47f3f725f88324d132741c85dfb6a/src/ejabberd_auth.erl#L144) 来检查用户密码。 又调用回来了。

plain 模块的作用，就是解析用户名和密码，并不做真正的验证。验证由 auth modules 来完成。

  - internal
  - external
  - anonymous
  - ldap
  - odbc
  - pam
  - riak
  - etc

# [check_password_with_authmodule](https://github.com/wcy123/ejabberd/blob/fb6267f38ee47f3f725f88324d132741c85dfb6a/src/ejabberd_auth.erl#L153)

## [auth_modules(Server)](https://github.com/wcy123/ejabberd/blob/fb6267f38ee47f3f725f88324d132741c85dfb6a/src/ejabberd_auth.erl#L146)

## 查询参数  [gen_mod:default_db](https://github.com/wcy123/ejabberd/blob/fb6267f38ee47f3f725f88324d132741c85dfb6a/src/ejabberd_auth.erl#L430)

有参数
[{default_db, Host}](https://github.com/wcy123/ejabberd/blob/9a9633dbc50430185a49cfda489bc87bed838d7c/src/gen_mod.erl#L288)
来控制。

```
(a@debian)64> ejabberd_config:get_option({default_db, <<"localhost">>}, fun (A)->A end, mnesia).
mnesia
```

默认是  mnesia 。

可用参数是

 - [odbc](https://github.com/wcy123/ejabberd/blob/9a9633dbc50430185a49cfda489bc87bed838d7c/src/gen_mod.erl#L268)
 - [internal](https://github.com/wcy123/ejabberd/blob/9a9633dbc50430185a49cfda489bc87bed838d7c/src/gen_mod.erl#L269)
 - [mnesia](https://github.com/wcy123/ejabberd/blob/9a9633dbc50430185a49cfda489bc87bed838d7c/src/gen_mod.erl#L270)
 - [riak](https://github.com/wcy123/ejabberd/blob/9a9633dbc50430185a49cfda489bc87bed838d7c/src/gen_mod.erl#L271)

[mnesia 和 internal](https://github.com/wcy123/ejabberd/blob/fb6267f38ee47f3f725f88324d132741c85dfb6a/src/ejabberd_auth.erl#L431) 是等价的。

参数[auth_method](https://github.com/wcy123/ejabberd/blob/fb6267f38ee47f3f725f88324d132741c85dfb6a/src/ejabberd_auth.erl#L435) 控制认证方式。

```
(a@debian)65> ejabberd_config:get_option({auth_method,<<"localhost">>}, fun (A)->A end).
[internal,anonymous]
```

然后返回一大堆认证模块。

```
(a@debian)69> ejabberd_auth:auth_modules(<<"localhost">>).
[ejabberd_auth_internal,ejabberd_auth_anonymous]
```


# [check_password_loop](https://github.com/wcy123/ejabberd/blob/fb6267f38ee47f3f725f88324d132741c85dfb6a/src/ejabberd_auth.erl#L146)

[没有配置 auth 模块的话，不接收任何用户] -> false](https://github.com/wcy123/ejabberd/blob/fb6267f38ee47f3f725f88324d132741c85dfb6a/src/ejabberd_auth.erl#L158)

调用虚函数
[check_password/3](https://github.com/wcy123/ejabberd/blob/fb6267f38ee47f3f725f88324d132741c85dfb6a/src/ejabberd_auth.erl#L160)
检查密码是否正确。这个虚函数是可变参数。plain 的时候，用 `U,S,P` 来检
查。

# [apply(AuthModule, check_password, Args)](https://github.com/wcy123/ejabberd/blob/fb6267f38ee47f3f725f88324d132741c85dfb6a/src/ejabberd_auth.erl#L160) 一个一个模块调用 `check_password)`。

# [ejabberd_auth_internal:check_password/3](https://github.com/wcy123/ejabberd/blob/fb6267f38ee47f3f725f88324d132741c85dfb6a/src/ejabberd_auth_internal.erl#L89) 的实现，最简单的。

有两种方式，scram or not

如果是 non-scram 的模式，就很简单， [when is_binary(Password)](https://github.com/wcy123/ejabberd/blob/fb6267f38ee47f3f725f88324d132741c85dfb6a/src/ejabberd_auth_internal.erl#L95) 直接比较就行了。

如果是 scram 的模式，[when is_record(Scram, scram)](https://github.com/wcy123/ejabberd/blob/fb6267f38ee47f3f725f88324d132741c85dfb6a/src/ejabberd_auth_internal.erl#L98) 就调用 [is_password_scram_valid](https://github.com/wcy123/ejabberd/blob/fb6267f38ee47f3f725f88324d132741c85dfb6a/src/ejabberd_auth_internal.erl#L99) 来检查密码。
