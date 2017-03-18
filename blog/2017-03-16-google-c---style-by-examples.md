---
layout: post
title:  "google c++ style by examples"
date:   2017/03/16 10:12:23
categories:
comments: true
---

详细参考 [https://google.github.io/styleguide/cppguide.html]()

# 文件名

* good

```
my_useful_class.cc
my-useful-class.cc
myusefulclass.cc
myusefulclass_test.cc // _unittest and _regtest are  deprecated.
```

# 类名

```c++
class UrlTable { ...
class UrlTableTester { ...
struct UrlTableProperties { ...

// typedefs
typedef hash_map<UrlTableProperties *, string> PropertiesMap;

// using aliases
using PropertiesMap = hash_map<UrlTableProperties *, string>;

// enums
enum UrlTableErrors { ...
```

# 变量名

```c++
int a_local_variable;
int a_struct_data_member;
int a_class_data_member_;
```

```c++
string tableName; // 不好
```

# 成员变量


```c++
class TableInfo {
  ...
 private:
  string table_name_;  // OK - underscore at end.
  string tablename_;   // OK.
  static Pool<TableInfo>* pool_;  // OK.
};
```

```c++
struct UrlTableProperties {
  string name;
  int num_entries;
  static Pool<UrlTableProperties>* pool;
};
```

# 常量

```c++
const int kDaysInAWeek = 7;
```

# 函数名

* good

  ```c
  void StartRpc() {}
  ```
* bad

  ```c
  void StartRpc() {}
  ```

缩写不需要
