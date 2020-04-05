# setup emacs IDE for Rust

Objectives:

1. generate a simple hello world in Rust.
1. format on save
2. auto completion
2. code navigation
2. cargo integration

## use `100ms_dot_emacs`

If you are not so patient, you can install
[100ms_dot_emacs](https://github.com/wcy123/100ms_dot_emacs), it works
out of box.

## generate a new project

[https://doc.rust-lang.org/cargo/guide/creating-a-new-project.html]()


```console
% cargo new hello_world --bin
     Created binary (application) `hello_world` package
% tree .
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


## install emacs rust-mode

[https://github.com/rust-lang/rust-mode]()


## format on save


install  rustfmt

[https://github.com/rust-lang/rustfmt]()


```console
rustup component add rustfmt
```

```console
M-x rust-format-buffer
```


you might need `exec-path-from-shell` if you are using mac os.

[https://github.com/purcell/exec-path-from-shell]()


```console
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

## auto completion

install racer

[https://github.com/racer-rust/racer#installation]()


```console
% cargo install racer
% cargo toolchain add nightly
% cargo +nightly install racer
% rustup component add rust-src
```


[https://github.com/racer-rust/emacs-racer]()


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


You can input `std::io::B<TAB>` , to test whether it works or not.

```text
To test go to definition: Place your cursor over a symbol and press M-. to jump to its definition.

Press C-x 4 . to jump to its definition in another window.

Press C-x 5 . to jump to its definition in another frame.

Press M-, to jump back to the previous cursor location.

If it doesn't work, try M-x racer-debug to see what command was run and what output was returned.

```

In this way, we can navigate in source code.


## cargo integration

```lisp
(add-hook 'rust-mode-hook #'cargo-minor-mode)
```

`cargo-minor-mode` is enabled in rust mode, now we can use the following cargo commands.

```text
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

## language server support


```lisp
(use-package lsp-mode
  :defines (lsp-keymap-prefix)
  :commands (lsp lsp-deferred)
  :init (setq lsp-keymap-prefix "C-l")
  :hook (((rust-mode)
          . lsp-deferred)
         (lsp-mode . lsp-enable-which-key-integration)))
```
