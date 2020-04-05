# 基于返回值的 java generic 类型推导


```java
public class  HelloWorld {
    public static void main(String[] args) {
        System.out.println("hello world");
    }

    boolean foo() {
        return new Adapter().handle();
    }
    public static class Adapter {
        public <T> T handle(){
            return null;
        }
    }
}
```

这里遇到几个问题. 在类型推到的过程中, T 到底是什么类型,
我没有找到好的方法, 于是用看字节码的方法看.

```text
javap -s -c ./HelloWorld.class
Compiled from "HelloWorld.java"
public class HelloWorld {
  public HelloWorld();
    descriptor: ()V
    Code:
       0: aload_0
       1: invokespecial #1                  // Method java/lang/Object."<init>":()V
       4: return

  public static void main(java.lang.String[]);
    descriptor: ([Ljava/lang/String;)V
    Code:
       0: getstatic     #2                  // Field java/lang/System.out:Ljava/io/PrintStream;
       3: ldc           #3                  // String hello world
       5: invokevirtual #4                  // Method java/io/PrintStream.println:(Ljava/lang/String;)V
       8: return

  boolean foo();
    descriptor: ()Z
    Code:
       0: new           #5                  // class HelloWorld$Adapter
       3: dup
       4: aload_0
       5: invokespecial #6                  // Method HelloWorld$Adapter."<init>":(LHelloWorld;)V
       8: invokevirtual #7                  // Method HelloWorld$Adapter.handle:()Ljava/lang/Object;
      11: checkcast     #8                  // class java/lang/Boolean
      14: invokevirtual #9                  // Method java/lang/Boolean.booleanValue:()Z
      17: ireturn
}
```

 - `8:` 表示 `T` 经过 Type Erase , 实际都是返回 `Object`
 - `11:` 表示, 在 `foo` 中, 推导出来的类型是 Boolean
 - `14:` 表示在做 unbox , 这里会产生 `NullPointerException`
