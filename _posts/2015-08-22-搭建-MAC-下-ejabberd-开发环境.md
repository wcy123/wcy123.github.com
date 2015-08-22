---
layout: post
title:  "搭建 MAC 下 ejabberd 开发环境"
date:   2015/08/22 14:20:07
categories:
comments: true
draft: true
---


## 下载安装 erlang

 - [R18](http://www.erlang.org/download/otp_src_18.0.tar.gz)
 - [R17.5](http://www.erlang.org/download/otp_src_17.5.tar.gz)

## 依赖库

 - brew , erlang 其实不依赖这个库， 仅仅为了方便安装其他库，[参考](http://brew.sh)

 ```
 ruby -e "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/master/install)"
 ```

 － odbc

 ```
 brew install unixodbc
 ```

 - 安装 openssl

 ```
 brew install openssl
 ```

 - 安装 JDK ，下载，打开，下一步，下一步, ... ，略过。

## 从源代码安装 Erlang

```
export CFLAGS='-I/usr/local/Cellar/unixodbc/2.3.2_1/include/'
export LDFLAGS='-L/usr/local/Cellar/unixodbc/2.3.2_1/lib/'

# for R17
./configure --prefix=/usr/local/Cellar/erlang/R17.5 --with-odbc=/usr/local/Cellar/unixodbc/2.3.2_1 --with-ssl=/usr/local/Cellar/openssl/1.0.2d_1
# for R18
./configure --prefix=/usr/local/Cellar/erlang/R18.0 --with-odbc=/usr/local/Cellar/unixodbc/2.3.2_1 --with-ssl=/usr/local/Cellar/openssl/1.0.2d_1
```

修改 `~/.bash_profile` 方便在两个版本切换

```
alias r17='export PATH=/usr/local/Cellar/erlang/R17.5/bin:/usr/local/bin:/usr/bin:/bin:/usr/sbin:/sbin'
alias r18='export PATH=/usr/local/Cellar/erlang/R18.0/bin:/usr/local/bin:/usr/bin:/bin:/usr/sbin:/sbin'

## brew 有的时候会提示带宽不够，根据提示，设置下面的环境变量，这个串可
## 以在 github 网页上的个人帐户设置上找到export
HOMEBREW_GITHUB_API_TOKEN=b78529xxxxxxxxxxxxxxxxxxxxx4de83f
```


## 安装 rebar

官方有一个 [rebar](https://github.com/basho/rebar)， 我就用我自己 fork
的一个版本。我做了一个小修改，在 R18 下面就没有编译错误了。其他的版本
也有类似修改。

```
mkdir ~/d/working
cd ~/d/working
git clone https://github.com/wcy123/rebar.git
cd rebar
./bootstrap
sudo cp ./rebar /usr/local/bin/
```

## 按住 distel

这个库不是必须安装，仅对 emacs 开发有帮助。官方有一个版本
[distel](https://github.com/massemanet/distel) ， 我用我自己 fork 的一
个版本。 我做了一个小修改，调试的时候，`f` 键可以返回当前函数，也就是
退栈。

## 配置 emacs

```el
;; 这个用来自动监测 erl 安装在哪里了。方便切换不同的 erlang 版本，
;; 如果不在乎版本的话，直接写下面的，也是可以的。
;;    (setq erlang-root-dir "where_your_install_erlang")
;;    (add-to-list 'load-path "where_your_erlang_mode_el")
(let* ((erl-exec (locate-file "escript" exec-path)))
  (when erl-exec
    (let ((temp-file (make-temp-file "erl")))
      (with-temp-file temp-file
        (insert "#!/usr/bin/env escript
-export([main/1]).
main(_) ->
    ToolPath = code:lib_dir(tools),
    RootDir = code:root_dir(),
    io:format(\"(setq erlang-root-dir ~p)~n\",[RootDir]),
    ElangModePath = filename:join(ToolPath, \"emacs\"),
    io:format(\"(add-to-list 'load-path ~p)~n\",[ElangModePath]).
"))
      (with-temp-buffer
        (call-process-shell-command "escript"
                                    nil ;input
                                    (current-buffer) ;output
                                    nil      ;display
                                    temp-file)
        (message "%s" (buffer-string))
        (delete-file temp-file)
        (eval-buffer)
        ))))
  (require 'erlang)
  ;;  this is set properly in the detection period
  ;; (setq erlang-root-dir  "/home2/chunywan/d/local/lib/erlang")
  (setq inferior-erlang-machine-options ;; 自动生成唯一的 node name
        (list "-sname"
              (format "%s" (emacs-pid))))
  (setq erlang-compile-extra-opts
        (list '(i . "./include") ;; 根据 otp 的目录结构，一般都需要 include
              'export_all ;; 调试的时候方便
              (cons 'i (expand-file-name
                     "d/working/easemob/ejabberd/deps/p1_xml/include"
                     (getenv "HOME"))) ;; 调试 p1_xml 的时候方便
              'debug_info))
  ;; TODO: this is no good way to detect distel is installed.
  (let ((distel-root (expand-file-name "~/d/working/distel")))
    (when (file-exists-p distel-root)
      (let ((dist-el (expand-file-name "elisp" distel-root)))
        (add-to-list 'load-path dist-el)
        (require 'distel)
        (distel-setup)
        (defconst distel-shell-keys
          '(("M-i" erl-complete)
            ("M-?" erl-complete)
            ("M-." erl-find-source-under-point)
            ("M-," erl-find-source-unwind)
            ("M-*" erl-find-source-unwind)
            )
          "Additional keys to bind when in Erlang shell.")
        (defun erlang-shell-mode-hook-1 ()
          ;; add some Distel bindings to the Erlang shell
          (dolist (spec distel-shell-keys)
            (define-key erlang-shell-mode-map (read-kbd-macro (car spec)) (cadr spec))))
        (add-hook 'erlang-shell-mode-hook 'erlang-shell-mode-hook-1))))
```

## 下载 easemob/ejabberd

```
git clone https://github.com/easemob/ejabberd.git
git checkout easemob-dev
```

## 安装 libyaml

```
brew install libyaml
brew list libyaml
```

## 安装 autotools

```
brew install libtool autoconf automake
```


## 编译 ejabberd

```
sh auto_gen.sh
export CFLAGS=-I/usr/local/Cellar/libyaml/0.1.6_1/include/
export LDFLAGS='-L/usr/local/Cellar/libyaml/0.1.6_1/lib'
./configure --enable-zlib --enable-odbc --enable-mysql --enable-tools
make
make rel
cp ejabberd.yml.exmaple rel/etc/ejabberd/ejabberd.yml
rel/bin/ejabberdctrl live
```
