---
layout: post
title:  "Create a blog on github.com"
date:   2014-08-01 21:34:46
categories: github blog
---

# register a new github account

# create a new repository, `wcy123.github.com`

# create a new blog pages

{% highlight shell-session %}
bash$ cd ~

bash$ mkdir wcy123.github.com

bash$ jekyll new wcy123.github.com
New jekyll site installed in /home/wcy123/wcy123.github.com.

bash$ cd wcy123.github.com/

bash$ git init .
Initialised empty Git repository in /home/wcy123/wcy123.github.com/.git/

bash$ git add .

bash$ git commit -m 'first commit'
[master (root-commit) faad2ef] first commit
 13 files changed, 657 insertions(+)
 create mode 100644 .gitignore
 create mode 100644 _config.yml
 create mode 100644 _includes/footer.html
 create mode 100644 _includes/head.html
 create mode 100644 _includes/header.html
 create mode 100644 _layouts/default.html
 create mode 100644 _layouts/page.html
 create mode 100644 _layouts/post.html
 create mode 100644 _posts/2014-08-01-welcome-to-jekyll.markdown
 create mode 100644 about.md
 create mode 100644 css/main.css
 create mode 100644 feed.xml
 create mode 100644 index.html

bash$ git remote add origin https://github.com/wcy123/wcy123.github.com.git

bash$ git push -u origin master
Username for 'https://github.com': wcy123
Password for 'https://wcy123@github.com':
Counting objects: 19, done.
Compressing objects: 100% (17/17), done.
Writing objects: 100% (19/19), 7.22 KiB | 0 bytes/s, done.
Total 19 (delta 1), reused 0 (delta 0)
To https://github.com/wcy123/wcy123.github.com.git
 + badab29...faad2ef master -> master
Branch master set up to track remote branch master from origin.

bash$ git checkout -b gh-pages
Switched to a new branch 'gh-pages'
bash$ git push -u origin gh-pages
Total 0 (delta 0), reused 0 (delta 0)
To https://github.com/wcy123/wcy123.github.com.git
 * [new branch]      gh-pages -> gh-pages
Branch gh-pages set up to track remote branch gh-pages from origin.
{% endhighlight %}


