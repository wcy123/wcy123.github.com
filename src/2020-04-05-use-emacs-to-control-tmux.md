# use emacs to control tmux

There are two main use cases for using emacs to control tmux session.

1. Recording your command history in a markdown file.
2. Bash programming

## How it works

`tmux` has many subcommands to control tmux, one of is `send-keys`, for example,

```console
tmux send-keys -t ! ls
```

It sends `ls` to the last active window. refer to [tmux manual][tmux-man] for detail about `-t`

[tmux-man]: http://man7.org/linux/man-pages/man1/tmux.1.html

It is not easy to send control characters, like return, tab
etc. Fortunately with latest `tmux 3.0a`, it supports `-H` command
line option, e.g.

```console
tmux send-keys -t ! -H 6c 73 0a
```

`0a` means return key so that we can execute `ls` in the last active window.

In elisp, we can easily convert any string into hex format as below

```lisp
(defun tmux-cc--convert-keys(strings)
  (seq-map #'(lambda(c) (format "%x" c)) strings))
```

```lisp
(tmux-cc--convert-keys "ls\n") => ("6c" "73" "a")
```

It becomes interesting when we invoke tmux `send-keys` from within a
emacs session.

```lisp
(setq strings "ls\n")
(apply #'call-process
   `("tmux" nil "*tmux cc*" t
     "send-keys" "-t" "op" "-H" ,@(tmux-cc--convert-keys strings)))
```

In this way, we can send arbitrary strings from a emacs buffer to a
tmux session.

There is a complete implemenation in [https://github.com/wcy123/tmux-cc]()

To install the package, you can put the following lines in your
`~/.emacs`.

```lisp
(use-package tmux-cc
  :straight
  (tmux-cc :type git
           :host github
           :repo "wcy123/tmux-cc")
  :commands
  (tmux-cc-send-current-line tmux-cc-select-block tmux-cc-send-region))
```

And it is recommended to bind `C-z` in `markdown-mode` or `shell-script-mode`.

```lisp
(use-package markdown-mode
  :defines (markdown-mode-map)
  :mode "\\.md\\'"
  :mode "\\.markdown\\'"
              ("C-z" . tmux-cc-send-current-line))
```

Or you can just install [https://github.com/wcy123/100ms_dot_emacs]()
it works out of box.
