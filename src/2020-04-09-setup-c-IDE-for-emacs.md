# setup Emacs IDE for C/C++

## Objectives

1. auto complete
2. auto format on save
3. LSP (eglot or lsp-mode)


## Install clangd

[cland][clangd] is a language server for C/C++. [install
clangd][clangd] lists many installation method. Here I show how to
install from source codes.

[clangd]: https://clangd.llvm.org/installation.html


### download

``` console
% mkdir ~/build/llvm
% cd ~/build/llvm
% ls -l
% curl -Lo llvm-10.0.0.src.tar.xz https://github.com/llvm/llvm-project/releases/download/llvmorg-10.0.0/llvm-10.0.0.src.tar.xz
% curl -Lo clang-10.0.0.src.tar.xz https://github.com/llvm/llvm-project/releases/download/llvmorg-10.0.0/clang-10.0.0.src.tar.xz
% curl -Lo clang-tools-extra-10.0.0.src.tar.xz https://github.com/llvm/llvm-project/releases/download/llvmorg-10.0.0/clang-tools-extra-10.0.0.src.tar.xz
```

### extract

``` console
% cd ~/build/llvm
% tar -xvf llvm-10.0.0.src.tar.xz
% tar -xvf clang-10.0.0.src.tar.xz -C llvm-10.0.0.src/tools/
% mv ./llvm-10.0.0.src/tools/clang-10.0.0.src llvm-10.0.0.src/tools/clang
% tar -xf clang-tools-extra-10.0.0.src.tar.xz -C llvm-10.0.0.src/tools/clang/tools
% mv llvm-10.0.0.src/tools/clang/tools/clang-tools-extra-10.0.0.src llvm-10.0.0.src/tools/clang/tools/clang-tools-extra
```

### configure, build and install

I am not sure it is a bug or not, but I have to change `llvm-10.0.0.src/tools/clang/tools/CMakeLists.txt` as below


``` diff
--- llvm-10.0.0.src/tools/clang/tools/CMakeLists.txt.orig       2020-04-09 11:46:25.844204200 +0800
+++ llvm-10.0.0.src/tools/clang/tools/CMakeLists.txt    2020-04-09 11:46:39.615524000 +0800
@@ -36,7 +36,7 @@
 # on top of the Clang tooling platform. We keep them in a separate repository
 # to keep the primary Clang repository small and focused.
 # It also may be included by LLVM_EXTERNAL_CLANG_TOOLS_EXTRA_SOURCE_DIR.
 -add_llvm_external_project(clang-tools-extra extra)
 +add_llvm_external_project(clang-tools-extra)

 # libclang may require clang-tidy in clang-tools-extra.
 add_clang_subdirectory(libclang)
```

``` console
% cd ~/build/llvm/
% mkdir build_out
% cd  build_out
% cmake -DCMAKE_BUILD_TYPE=MINSIZEREL -DBUILD_SHARED_LIBS=on  ../llvm-10.0.0.src
% make && sudo make install
% which clangd # /usr/local/bin/clangd, make sure it is exits.
```

## emacs configuration

If you are not so patient, you can install
[100ms_dot_emacs](https://github.com/wcy123/100ms_dot_emacs), it works
out of box.

### eglog


``` elisp
(use-package eglot
  :defines (eglot-mode-map eglot-server-programs)
  :hook (((c-mode c++-mode) . eglot-ensure))
  :bind (:map eglot-mode-map
              ("C-c h" . eglot-help-at-point)
              ("C-c f r" . xref-find-references)
              ("C-c f d" . eglot-find-declaration ;; xref-find-definitions
               )
              ("C-c f D" . xref-find-definitions-other-window)
              ("C-c f t" . eglot-find-typeDefinition)
              ("C-c f i" . eglot-find-implementation)
              ("C-c =" . eglot-format-buffer)
              ("C-c c" . eglot-completion-at-point)
              ("C-c r" . eglot-rename)
              ("C-c a" . eglot-code-actions))
  :config
  (add-to-list 'eglot-server-programs '((c++-mode c-mode) "clangd")))
```
### company

``` elisp
(use-package company
  :after (prog-mode)
  :diminish (company-mode . "C")
  :defines (
            company-active-map
            company-idle-delay
            company-minimum-prefix-length
            company-show-numbers
            company-tooltip-limit
            company-dabbrev-downcase)
  :hook (prog-mode . company-mode)
  :bind (:map company-active-map
              (("C-n" . company-select-next)
               ("C-p" . company-select-previous)))
  :config
  (setq company-idle-delay              nil)
  (setq company-minimum-prefix-length   2)
  (setq company-show-numbers            t)
  (setq company-tooltip-limit           20)
  (setq company-dabbrev-downcase        nil)
  :bind (:map prog-mode-map
              ("C-r" . company-complete)))
```

### format on save

```elisp
(use-package clang-format
  :defines (clang-format-fallback-style)
  :after (cc-mode)
  :config
  (set-default 'clang-format-fallback-style "Google")
  (add-hook 'c-mode-common-hook #'(lambda()
                                    (add-hook 'before-save-hook
                                              'clang-format-buffer t t))))
```

### for `Makefile` project

We need to install [Bear]()

[Bear]: https://github.com/rizsotto/Bear

``` console
% mkdir ~/build
% cd ~/build
% git clone --depth 1 git@github.com:rizsotto/Bear.git
% cd Bear
% cmake .
% make
% sudo make install
```

We create a simple hello world project as below

``` makefile
# Makefile
hello: hello.c
```

``` c
#include <stdio.h>
struct point {
   int x;
   int y;
};
int main(int char, char argv[]) {
   struct point pt;
   pt.x ; # press C-r to complete, C-r is bind to company-complete
}
```

``` console
% bear make # this generate compile_commands.json
```

### for `cmake` project

``` console
cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=on . # this command generate compile_commands.json
```

### common used key bindigns


| key | command               |
|:----|:----------------------|
| M-7 | compile               |
| C-r | company-complete      |
| M-. | xref-find-definitions |
| M-, | xref-pop-marker-stack |
