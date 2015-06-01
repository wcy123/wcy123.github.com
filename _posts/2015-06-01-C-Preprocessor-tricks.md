---
layout: post
title:  "C Preprocessor tricks"
date:   2015/06/01 09:38:40
categories:
comments: true
---


In this blog, I will show some examples about advanced C Preprocessor
examples. First of all, I would say CPP macros are ugly, hard to debug
and error prone.

## define a list of items

I would like to use an example to illustrate the idea behind it. For
example, I would like to implementat a set of unix command as below

{% highlight c %}
#define CMD(XX) XX(ls) XX(cp) XX(rm) XX(echo)
{% endhighlight %}

Then I could be able to write a common template function as below.

{% highlight c %}
#define DEFINE_COMMON_TEMPLATE(name)            \
void name()                                     \
{                                               \
   printf(#name " is not implemented\n");       \
}

CMD(DEFINE_COMMON_TEMPLATE)
{% endhighlight %}

It will expanded to

{% highlight c %}
void ls ()
{
  printf ("ls" " is not implemented\n");
}

void cp ()
{
  printf ("cp" " is not implemented\n");
} // ...
{% endhighlight %}

In the `main` function, we can use the similiar trick, as below

{% highlight c %}
#define COMMAND_SWITCH(name)                    \
    if (strcmp(argv[1], #name) == 0){           \
        name();                                 \
    } else
    CMD(COMMAND_SWITCH) {
        printf("unknown commmand %s\n", argv[1]);
    }
{% endhighlight %}

And it will expanded into

{% highlight c %}
  if (strcmp (argv[1], "ls") == 0)
    {
      ls ();
    }
  else if (strcmp (argv[1], "cp") == 0)
    {
      cp ();
    }
  else if (strcmp (argv[1], "rm") == 0)
    {
      rm ();
    }
  else if (strcmp (argv[1], "echo") == 0)
    {
      echo ();
    }
  else
    {
      printf ("unknown commmand %s\n", argv[1]);
    }
{% endhighlight %}

The whole program is listed here.

{% highlight c %}
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#define CMD(XX) XX(ls) XX(cp) XX(rm) XX(echo)

#define DEFINE_COMMON_TEMPLATE(name)            \
void name()                                     \
{                                               \
   printf(#name " is not implemented\n");       \
}

CMD(DEFINE_COMMON_TEMPLATE)

int main(int argc, char *argv[])
{
#define COMMAND_SWITCH(name)                    \
    if (strcmp(argv[1], #name) == 0){           \
        name();                                 \
    } else

    CMD(COMMAND_SWITCH) {
        printf("unknown commmand %s\n", argv[1]);
    }
    return 0;
}
{% endhighlight %}
