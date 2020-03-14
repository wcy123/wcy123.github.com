---
layout: post
title:  "Rust Hello World"
date:   2020/02/08 16:58:27
comments: true
draft: true
categories:
---


* 第一次开发 Rust ，配置环境

目标

1. 生成项目结构
1. 保存的时候自动格式化代码
2. 自动补全


生成项目

https://doc.rust-lang.org/cargo/guide/creating-a-new-project.html

```shell
chunywan@localhost:rust% cargo new hello_world --bin
     Created binary (application) `hello_world` package
chunywan@localhost:rust% tree .
.
└── hello_world
    ├── Cargo.toml
    └── src
        └── main.rs

2 directories, 2 files
chunywan@localhost:rust% cd hello_world/
chunywan@localhost:hello_world% cargo build
   Compiling hello_world v0.1.0 (/Users/chunywan/d/working/learn/rust/hello_world)
    Finished dev [unoptimized + debuginfo] target(s) in 1.80s
chunywan@localhost:hello_world% cargo run
    Finished dev [unoptimized + debuginfo] target(s) in 0.00s
     Running `target/debug/hello_world`
Hello, world!
```


安装 emacs  rust 扩展

https://github.com/rust-lang/rust-mode


```
M-x rust-format-buffer
```

出现错误

```
rust-format-buffer: Could not locate executable "rustfmt"
```


安装   rustfmt

https://github.com/rust-lang/rustfmt


```
rustup component add rustfmt
```

安装之后，还是找不到  rustfmt ，但是命令行下是可以的，明显 emacs 进程没有正确配置  PATH 这个环境变量。

https://github.com/purcell/exec-path-from-shell


```
M-x package-install RET exec-path-from-shell RET.
```


```elisp
(use-package exec-path-from-shell
  :ensure t
  :config
  (when (or (memq window-system '(mac ns x))
            (memq system-type '(darwin)))
    (exec-path-from-shell-initialize)))
```


```elisp
(use-package rust-mode
  :ensure t
  :config
  (setq rust-format-on-save t))
```

代码补全

racer

https://github.com/racer-rust/racer#installation



```
cargo install racer
```

https://github.com/racer-rust/emacs-racer


```elisp
(use-package rust-mode
  :mode "\\.rs\\'"
  :ensure t
  :config
  (setq rust-format-on-save t)
  (add-hook 'rust-mode-hook #'cargo-minor-mode))
(use-package racer
  :ensure t
  :config
  :after (rust-mode)
  :config
  (add-hook 'rust-mode-hook #'racer-mode)
  (add-hook 'racer-mode-hook #'eldoc-mode)
  (add-hook 'racer-mode-hook #'company-mode)
  (define-key rust-mode-map (kbd "TAB") #'company-indent-or-complete-common)
  (setq company-tooltip-align-annotations t)
  (setq racer-loaded 1))
```

出现下面的错误




```
Company: backend company-capf user-error: No such directory: /Users/chunywan/.rustup/toolchains/\
stable-x86_64-apple-darwin/lib/rustlib/src/rust/src. Please set ‘racer-rust-src-path’ or ‘RUS\
T_SRC_PATH’
```

安装的时候没有仔细读文档


```
rustup toolchain add nightly
cargo +nightly install racer
rustup component add rust-src # 这个修复上面的错误
```

用 `std::io::B<TAB>` 测试补全通过。

```
To test go to definition: Place your cursor over a symbol and press M-. to jump to its definition.

Press C-x 4 . to jump to its definition in another window.

Press C-x 5 . to jump to its definition in another frame.

Press M-, to jump back to the previous cursor location.

If it doesn't work, try M-x racer-debug to see what command was run and what output was returned.

```

这样就可以浏览代码，实现代码的跳转了。


上面的配置中，有下面这句话。

```
(add-hook 'rust-mode-hook #'cargo-minor-mode)
```

这个就打开了 `cargo-minor-mode` ，于是可以有常用的编译命令

```
C-c C-c C-a     cargo-process-add
C-c C-c C-b     cargo-process-build
C-c C-c C-c     cargo-process-repeat
C-c C-c C-d     cargo-process-doc
C-c C-c C-e     cargo-process-bench
C-c C-c C-f     cargo-process-current-test
C-c C-c TAB     cargo-process-init
C-c C-c C-k     cargo-process-check
C-c C-c C-l     cargo-process-clean
C-c C-c RET     cargo-process-fmt
C-c C-c C-n     cargo-process-new
C-c C-c C-o     cargo-process-current-file-tests
C-c C-c C-r     cargo-process-run
C-c C-c C-s     cargo-process-search
C-c C-c C-t     cargo-process-test
C-c C-c C-u     cargo-process-update
C-c C-c C-v     cargo-process-doc-open
C-c C-c C-x     cargo-process-run-example
C-c C-c C-S-a   cargo-process-audit
C-c C-c C-S-d   cargo-process-rm
C-c C-c C-S-k   cargo-process-clippy
C-c C-c C-S-o   cargo-process-outdated
C-c C-c C-S-u   cargo-process-upgrade
```
