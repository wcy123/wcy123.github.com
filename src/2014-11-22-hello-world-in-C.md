#  "hello world in C"



用 C 语言写一个 `hello world` 程序需要一下几个步骤：

- 编辑、
- 编译成为汇编
- 汇编编译二进制目标代码
- 链接成可执行文件


## 编辑

这一个步骤很简单，找一个你熟悉的编辑器，生成一个 `hello_world.c`， 如下。

```c
extern int puts(const char * s);
int main(int argc, char *argv[])
{
    puts("hello world!");
    return 0;
}
```

这里我我没有使用常用的 `#include <stdio.h>`，我想强调的是 `#include`
不是必须的。

## 编译
在 linux 的命令行下，输入编译命令。如下。

```console
% gcc -O3 -c -S -o hello_world.S  hello_world.c
```

- `-c` 表示编译。
- `-S` 表示输出汇编语言，默认直接输出二进制目标文件。
- `-o hello_world.S`  表示指定输出文件名称。
- `-O3` 表示强烈优化代码。这样生成的 asm 代码可读性更好。

我们如果察看 `hello_world.S`， 有如下结果

```x86asm
	.file	"hello_world.c"
	.section	.rodata.str1.1,"aMS",@progbits,1
.LC0:
	.string	"hello world!"
	.section	.text.unlikely,"ax",@progbits
.LCOLDB1:
	.section	.text.startup,"ax",@progbits
.LHOTB1:
	.p2align 4,,15
	.globl	main
	.type	main, @function
main:
.LFB0:
	.cfi_startproc
	subq	$8, %rsp
	.cfi_def_cfa_offset 16
	movl	$.LC0, %edi
	call	puts
	xorl	%eax, %eax
	addq	$8, %rsp
	.cfi_def_cfa_offset 8
	ret
	.cfi_endproc
.LFE0:
	.size	main, .-main
	.section	.text.unlikely
.LCOLDE1:
	.section	.text.startup
.LHOTE1:
	.ident	"GCC: (Debian 4.9.1-19) 4.9.1"
	.section	.note.GNU-stack,"",@progbits

```

## 生成二进制目标文件

```console
% as -o hello_world.o hello_world.S
```

`as` 是一个汇编器, 把汇编代码转换成为二进制目标代码。

我们可以看看这个目标代码都有什么。

```console
% nm hello_world.o
0000000000000000 T main
                 U puts
```

可以看到，我们的程序定义了连个符号，`main` ， `puts`。

- `T` 表示在本目标文件中包含一大块内存的定义， 这一大块内存地址用
  `main` 来表示。这一块内存里面存储的就是`main` 函数的机器指令。

- `U` 表示本目标文件没有引用了一个符号 `puts` 但是不知道这个符号定义在什么地方。

我们可以看一下这个目标文件的内容。

```console
% objdump -d hello_world.o

hello_world.o:     file format elf64-x86-64


Disassembly of section .text.startup:

0000000000000000 <main>:
   0:	48 83 ec 08          	sub    $0x8,%rsp
   4:	bf 00 00 00 00       	mov    $0x0,%edi
   9:	e8 00 00 00 00       	callq  e <main+0xe>
   e:	31 c0                	xor    %eax,%eax
  10:	48 83 c4 08          	add    $0x8,%rsp
  14:	c3                   	retq
```

我们可以看到，`main` 对应的地址是 `0` ，其实这个地址会变化，这就是链接
的作用。下面就是机器指令。`c3` 机器指令 `retq`。

在 x64 中， 我们调用函数的时候，如果参数少于 6 个，那么参数是由寄存器
传递，即， RDI, RSI, RCX, RDX, R8, 和 R9 。多余 6 个就用 stack 传递。

我们看到 `%edi` 的值是 `0`。 `mov $0x0, %edi` ，不是我们输入的
`"helloworld"`的地址。调用函数 `puts` 也变成了 `callq e`。这是因为还没
有链接过。一个目标文件是不能知道这些地址是什么。

## 链接

一个可执行程序必须有入口，通常我们说是 `main` 函数。其实这个是可以指定的。`ld` 输出的可执行文件，都有一个入口函数。这个入口函数默认不是`main` 而是 `_start` 。 这个 `_start` 函数定一个在 `crt1.o` 的文件里
面。`puts` 定义在 `libc.a` 里面。所以我们的链接命令是。

```console
% ld -o hello_world \
   -dynamic-linker "/lib64/ld-linux-x86-64.so.2"\
    hello_world.o \
    "/usr/lib/x86_64-linux-gnu/crt1.o"\
    "/usr/lib/x86_64-linux-gnu/crti.o"\
    "/usr/lib/gcc/x86_64-linux-gnu/4.9/crtbegin.o"\
    "-L/usr/lib/gcc/x86_64-linux-gnu/4.9" \
    -lc  \
    "/usr/lib/gcc/x86_64-linux-gnu/4.9/crtend.o"\
    "/usr/lib/gcc/x86_64-linux-gnu/4.9/../../../x86_64-linux-gnu/crtn.o"
```

- `-o hello_world` 指明输出可执行文件的名称。
- `hello_world.o` 我们刚刚生成的目标文件。
- `-dynamic-linker` 指定 `ld-linux-x86-64.so.2` 是 loader ,否则无法加载可执行文件。
- `.../crt1.o` , `.../crti.o`， `crtbegin.o`, `crtn.o`, `crtend.o` 就
  是负责处理程序开始和结束的一些内容。例如 `atexit` 之类的。定义
  `_start`。
- `-lc` 表示链接标准 c 的库函数，`puts` 就定义在里面。



我们看一下 `hello_world` 的内容。

```consoel
% objdump -d  hello_world

hello_world:     file format elf64-x86-64


Disassembly of section .init:
....
0000000000400370 <puts@plt>:
  400370:	ff 25 92 03 20 00    	jmpq   *0x200392(%rip)        # 600708 <_GLOBAL_OFFSET_TABLE_+0x18>
  400376:	68 00 00 00 00       	pushq  $0x0
  40037b:	e9 e0 ff ff ff       	jmpq   400360 <_init+0x18>

Disassembly of section .text:

00000000004003a0 <main>:
  4003a0:	48 83 ec 08          	sub    $0x8,%rsp
  4003a4:	bf 58 04 40 00       	mov    $0x400458,%edi
  4003a9:	e8 c2 ff ff ff       	callq  400370 <puts@plt>
  4003ae:	31 c0                	xor    %eax,%eax
  4003b0:	48 83 c4 08          	add    $0x8,%rsp
  4003b4:	c3                   	retq

00000000004003b5 <_start>:
  4003b5:	31 ed                	xor    %ebp,%ebp
  4003b7:	49 89 d1             	mov    %rdx,%r9
  4003ba:	5e                   	pop    %rsi
  4003bb:	48 89 e2             	mov    %rsp,%rdx
  4003be:	48 83 e4 f0          	and    $0xfffffffffffffff0,%rsp
  4003c2:	50                   	push   %rax
  4003c3:	54                   	push   %rsp
  4003c4:	49 c7 c0 50 04 40 00 	mov    $0x400450,%r8
  4003cb:	48 c7 c1 e0 03 40 00 	mov    $0x4003e0,%rcx
  4003d2:	48 c7 c7 a0 03 40 00 	mov    $0x4003a0,%rdi
  4003d9:	e8 a2 ff ff ff       	callq  400380 <__libc_start_main@plt>
  4003de:	f4                   	hlt
  4003df:	90                   	nop
  ...
```

这个输出太长，我截断了一些。

我们可以看到 `_start` 函数的定义，看到 `puts` 函数的定义，`puts` 在地
址 `0x400470`。看到 `main` 函数数的定义，地址在 `0x4003a0`， 看调用
`puts` 的语句变成了。

```text
4003a9:	e8 c2 ff ff ff       	callq  400370 <puts@plt>
```

`4003a9` 表示这条机器指令的绝对位置。`callq e` 也变成了 `callq 400370`，即 `puts` 的地址。

```text
4003a4:	bf 58 04 40 00       	mov    $0x400458,%edi
```

`"hello world"` 的地址也不是 `0` 了，而是 `0x400458`。


这就是链接器的作用，原来不确定的地址，例如 `main`, `"hello world"`,
`puts`都变成了活生生的地址了。

## 运行

```console
% ./hello_world
hello world!
```

## 简化过程

我们不必每一次都这么麻烦，指定这么多的细节命令，`gcc` 可以一次从头干到
尾。实际上，很少有人直接这么一步一步地做，都是直接调用 `gcc` 一步到位。

```console
% gcc -o hello_world hello_world.c
% ./hello_world
hello world!
```

如果有多个文件，常用的作法是

```console
% gcc -c -o hello_world.o hello_world.c
% gcc -o hello_world hello_world.o
% ./hello_world
hello world!
```
