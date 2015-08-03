---
layout: post
title:  "ejabberdctrl register a new user"
date:   2015/08/01 23:20:54
categories:
comments: true
draft: true
---

运行命令

```
% ejabberdctl --node a@debian register test localhost 123
User test@localhost successfully registered
% ejabberdctl --node a@debian registered_users localhost
test
```

# admin 的命令机制。

```
     #ejabberd_commands{name = register, tags = [accounts],
			desc = "Register a user",
			module = ?MODULE, function = register,
			args = [{user, binary}, {host, binary}, {password, binary}],
			result = {res, restuple}},
```

注册命令 [ejabberd_commands](https://github.com/wcy123/ejabberd/blob/19aad464da3079af02b891ea7c4abd164526c51c/src/ejabberd_admin.erl#L117)


# 运行的最后命令是

```
sh -c '/usr/local/bin/erl       -sname ejabberdctl-1       -noinput       -hidden       -pa /usr/local/ejabberd/lib/ejabberd/ebin              -s ejabberd_ctl -extra a@debian registered_users localhost'
```

当然，设置了好多环境变量之类的东西。

# 从这里开始 [ejabberd_ctl:start](https://github.com/wcy123/ejabberd/blob/fb6267f38ee47f3f725f88324d132741c85dfb6a/src/ejabberd_ctl.erl#L65)

## 得到命令行参数 [init:get_plain_arguments](https://github.com/wcy123/ejabberd/blob/fb6267f38ee47f3f725f88324d132741c85dfb6a/src/ejabberd_ctl.erl#L66)

## 调用 [rpc:call(Node, ?MODULE, process, [Args])](https://github.com/wcy123/ejabberd/blob/fb6267f38ee47f3f725f88324d132741c85dfb6a/src/ejabberd_ctl.erl#L83)

## [ejabberd_ctl:process(Args)](https://github.com/wcy123/ejabberd/blob/fb6267f38ee47f3f725f88324d132741c85dfb6a/src/ejabberd_ctl.erl#L200)

因为是 rpc ， 也可以在 erlang shell 中执行这个命令。

```
(a@debian)9> ejabberd_ctl:process(["registered_users","localhost"]).
test
test
23:31:11.271 [debug] Executing command ejabberd_admin:registered_users with Args=[<<"localhost">>]
0
```

## [get_accesscommands](https://github.com/wcy123/ejabberd/blob/fb6267f38ee47f3f725f88324d132741c85dfb6a/src/ejabberd_ctl.erl#L201) 读取参数 [ejabberdctl_access_commands](https://github.com/wcy123/ejabberd/blob/fb6267f38ee47f3f725f88324d132741c85dfb6a/src/ejabberd_ctl.erl#L238) ， 这个参数我没有设置。 `[]`

## [process2(Args, AccessCommands)](https://github.com/wcy123/ejabberd/blob/fb6267f38ee47f3f725f88324d132741c85dfb6a/src/ejabberd_ctl.erl#L213)
## [process2(Args, Auth, AccessCommands)](https://github.com/wcy123/ejabberd/blob/fb6267f38ee47f3f725f88324d132741c85dfb6a/src/ejabberd_ctl.erl#L216)

### [try_run_ctp(Args, Auth, AccessCommands)](https://github.com/wcy123/ejabberd/blob/fb6267f38ee47f3f725f88324d132741c85dfb6a/src/ejabberd_ctl.erl#L217)

#### 运行钩子 [ejabberd_ctl_process](https://github.com/wcy123/ejabberd/blob/fb6267f38ee47f3f725f88324d132741c85dfb6a/src/ejabberd_ctl.erl#L247)

#### [try_call_command](https://github.com/wcy123/ejabberd/blob/fb6267f38ee47f3f725f88324d132741c85dfb6a/src/ejabberd_ctl.erl#L249)

#### [call_command](https://github.com/wcy123/ejabberd/blob/fb6267f38ee47f3f725f88324d132741c85dfb6a/src/ejabberd_ctl.erl#L268)

##### [Command = list_to_atom(binary_to_list(CmdStringU))](https://github.com/wcy123/ejabberd/blob/fb6267f38ee47f3f725f88324d132741c85dfb6a/src/ejabberd_ctl.erl#L285)

这里产生内存泄漏，不过如果在一个安全的网络环境中，可以忍

##### [ejabberd_commands:get_command_format](https://github.com/wcy123/ejabberd/blob/fb6267f38ee47f3f725f88324d132741c85dfb6a/src/ejabberd_ctl.erl#L286)

```
(a@debian)14> ejabberd_commands:get_command_format(register)
(a@debian)14> .
{[{user,binary},{host,binary},{password,binary}],
 {res,restuple}}
(a@debian)15> ejabberd_commands:get_command_format(registered_users).
{[{host,binary}],{users,{list,{username,string}}}}
```
##### 解释参数 [format_args(Args, ArgsFormat)](https://github.com/wcy123/ejabberd/blob/fb6267f38ee47f3f725f88324d132741c85dfb6a/src/ejabberd_ctl.erl#L290)

```
(a@debian)23> ejabberd_ctl:format_args(["localhost"], element(1,ejabberd_commands:get_command_format(registered_users))).
[<<"localhost">>]
```

可以看到 [format_arg(Arg, integer)](https://github.com/wcy123/ejabberd/blob/fb6267f38ee47f3f725f88324d132741c85dfb6a/src/ejabberd_ctl.erl#L321) ，这里只支持 integer, binary , string 三种类型。

###### [ejabberd_commands:execute_command](https://github.com/wcy123/ejabberd/blob/fb6267f38ee47f3f725f88324d132741c85dfb6a/src/ejabberd_ctl.erl#L292)


###### [execute_command(AccessCommands, Auth, Name, Arguments)](https://github.com/wcy123/ejabberd/blob/7ab6c4b4fd65d75e670152c38551d4311e1b481b/src/ejabberd_commands.erl#L305)

####### [check_access_commands](https://github.com/wcy123/ejabberd/blob/7ab6c4b4fd65d75e670152c38551d4311e1b481b/src/ejabberd_commands.erl#L308)

# 会运行命令 [ejabberd_adm:register](https://github.com/wcy123/ejabberd/blob/19aad464da3079af02b891ea7c4abd164526c51c/src/ejabberd_admin.erl#L338)
