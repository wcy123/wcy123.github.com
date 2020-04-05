# C Preprocessor tricks



In this blog, I will show some examples about advanced C Preprocessor
examples. First of all, I would say CPP macros are ugly, hard to debug
and error prone.

## define a list of items

I would like to use an example to illustrate the idea behind it. For
example, I would like to implementat a set of unix command as below

```c
#define CMD(XX) XX(ls) XX(cp) XX(rm) XX(echo)
```

Then I could be able to write a common template function as below.

```c
#define DEFINE_COMMON_TEMPLATE(name)            \
void name()                                     \
{                                               \
   printf(#name " is not implemented\n");       \
}

CMD(DEFINE_COMMON_TEMPLATE)
```

It will expanded to

```c
void ls ()
{
  printf ("ls" " is not implemented\n");
}

void cp ()
{
  printf ("cp" " is not implemented\n");
} // ...
```

In the `main` function, we can use the similiar trick, as below

```c
#define COMMAND_SWITCH(name)                    \
    if (strcmp(argv[1], #name) == 0){           \
        name();                                 \
    } else
    CMD(COMMAND_SWITCH) {
        printf("unknown commmand %s\n", argv[1]);
    }
```

And it will expanded into

```c
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
```

The whole program is listed here.

```c
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
```


## get the number of varadic argument

```c
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#define _GET_N_OF_ARGS(_0,_1,_2,_3,_4,N,...) N
#define N_OF_ARGS(...) _GET_N_OF_ARGS(__VA_ARGS__,5,4,3,2,1,0)
int main(int argc, char *argv[])
{
    printf("%d\n", N_OF_ARGS(a));
    printf("%d\n",N_OF_ARGS(a,b));
    printf("%d\n",N_OF_ARGS(a,b,c));
    printf("%d\n",N_OF_ARGS(a,b,c,d));
    printf("%d\n",N_OF_ARGS(a,b,c,d,e));
    return 0;
}
```


`N_OF_ARGS` can handle at most 5 arguments and at least 1, otherwise,
it is unpredictable error.

```c
int main(int argc, char *argv[])
{
    printf("%d\n",1);
    printf("%d\n",2);
    printf("%d\n",3);
    printf("%d\n",4);
    printf("%d\n",5);
    return 0;
}
```


But
```c
N_OF_ARGS()
// => _GET_N_OF_ARGS(,5,4,3,2,1)
// => 1
printf("%d\n",N_OF_ARGS(a,b,c,d,e,f));
// => _GET_N_OF_ARGS(a,b,c,d,e,f)
// => f
```

## get the nth element of arguments
