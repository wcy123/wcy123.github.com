# 用 Makefile + pandoc + markdown 写博客

更新，已经移植到了 mdbook 工具了。[看这里](2020-04-04-start-to-use-mdbook.html)

静态页面生成有很多工具，我用过 Jekyll, Hugo, Hexo 等等。我原来用 Jekyll
写笔记，其实也够用了。最近想画图，这个东西不能自动集成 metapost ，也是挺麻烦的。

这些静态网页工具，功能太多，有很多其实不需要，这的需要自定义一些功能，折腾这些架构又过于复杂。例如

 - 集成 metapost , dot 等画图工具。我自己读过一个别人写的 hexo plugin 支持 dot 集成，但是折腾一次很麻烦。
 - template 模式过于复杂，每个工具都有自己的一套自定义模板语言
 - 不支持  asciidoc 等混合格式。

本来我想继续使用 Jekyll 的，但是突然间需要让我升级 ruby , 然后升级 gem
然后升级 bundle ，然后升级 jekyll ，然后说有一个依赖关系找不到。 WTF

我不是 ruby 的专家。我不想折腾了。

Hexo 类似的问题，升级完 nodejs ，一大堆问题。

于是，我试着用原始的方法生成网页。类似 unix 下的哲学，每个工具完成一点点工作，然后把他们组合在一起。于是，我用到下面这些工具。

 - atom + markdown preview plus 支持编辑和预览
 - Makefile 管理生产方式
 - pandoc 处理 markdown 生产 html
 - asciidoc ， 处理 asciidoc ，生产 html
 - MathJax 处理公式
 - metapost 处理画图。
 - perl 生产 index.html

 需要安装

```console
%  brew install mactex
% brew install pandoc
% apm install markdown-preview-plus
```

Makefile 一般 unix 系统都自带的。

```makefile
first_blog.html: first_blog.md
    pandoc -s --output $@ $<
```

然后 `make first_blog.html` 就可以生成了。这下自由了，不需要安装一个编程语言环境，就可以搞定网页生成了。 而且每个模块是可以替换的。如果觉得 pandoc 不好，可以用 kramdown 等等其他命令行工具处理 markdown 。

我的 Makefile 比这个复杂一些，也是抄别人的。 感兴趣的可以

```console
% git clone https://github.com/wcy123/wcy123.github.io
% git checkout pandoc
```

这里面每段代码都很简单，类似浆糊弄在一起。

css 我找了 solarized light scheme 。直接 copy paste 过去的。
