#  Different key binding in Emacs transient mark mode


[Emacs transient mark mode][] gives you much of the standard
selection-highlighting behavior of other editors, but key binding is
not changed when mark is active, for example, many letter keys are
bound to `self-insert-command`. This is quite different with other
editors. In other editors, usually when mark is active and you input
some texts, the new texts replace the selected text.

There is an easy way to temporarily change the key binding when mark
is active and restore the original key bind automatically when mark is
inactive.

```lisp
(global-set-key (kbd "C-w") 'backward-kill-word)
(defconst wcy-transient-mode-map-alist
  `((mark-active
     ,@(let ((m (make-sparse-keymap)))
	 (define-key m (kbd "C-w") 'kill-region)
	 m))))
(add-to-list 'emulation-mode-map-alists
	     'wcy-transient-mode-map-alist)

```

The above example makes `C-w` have two different bindings. When mark
is not active, it is `backward-kill-word`, similiar to the key binding
under some shell, when mark is active, it binds to `kill-region`,
following the emacs convention.

This example just illustrates the idea how to do it, maybe you might
have your own preferred key bindings.


Emacs has a lot of key bindings, in such way, one key is able to bind
to different emacs commands depending on whether the mark is active or
not.


[Emacs transient mark mode]: http://www.emacswiki.org/emacs/TransientMarkMode
