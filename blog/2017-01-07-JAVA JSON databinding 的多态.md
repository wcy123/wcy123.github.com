---
layout: post
title:  "JAVA JSON databinding 的多态"
date:   2017/01/07 20:18:26
categories:
comments: true
---
JSON 在 REST API 的调用中越来越多的应用，如何表达多态是经常碰到的一个问题。

有三种方式解决这个问题
- `PROPERTY` ，`EXISTING_PROPERTY`， POJO 映射为 JSON Object ，某一个字段表示类型。
-  `WRAPPER_OBJECT` POJO 映射为只有一个字段的 JSON Object ，字段名表示类型。
-  `WRAPPER_ARRAY` POJO 映射为两个元素的数组 ，前面的表示类型，后面的表示指值。


# 背景

```java
@Data
static class  Zoo {
    private Animal leader;
    private Animal[] followers;
}
```
假设有一个 `Zoo` 类，里面包含了一个特殊的动物 `leader`，和一些动物 `followers`。下面的代码试图转换 JSON

```java
public static void main(String[] args) throws IOException {
    Zoo zoo = new Zoo();
    zoo.setLeader(new Dog());
    zoo.setFollowers(new Animal[]{new Dog(), new Cat()});
    final ObjectMapper mapper = new ObjectMapper();
    final String json = mapper.writeValueAsString(zoo);
    System.out.println(json);
    final Zoo zoo2 = mapper.readValue(json, Zoo.class);
    System.out.println(zoo2);
}
```
例子 `Dog` 和  `Cat` 类

```
static class Dog implements Animal {
    private String name = "wangwang";
    @Override
    public String getAnimalType() {
        return "dog";
    }

    @Override
    public String getName() {
        return name;
    }
    @Override
    public void setName(String name) {
        this.name = name;
    }
}
static class Cat implements Animal {
    private String name = "miao";

    @Override
    public String getAnimalType() {
        return "cat";
    }

    @Override
    public String getName() {
        return name;
    }
    @Override
    public void setName(String name) {
        this.name = name;
    }
}
```


# 把 POJO 映射称为 JSON Object , `include = JsonTypeInfo.As.EXISTING_PROPERTY`

`include = JsonTypeInfo.As.PROPERTY` 也类似，区别不大。

用字段 `animalType` 来表示类型信息。

```json
{
  "leader": {
    "name": "wangwang",
    "animalType": "dog"
  },
  "followers": [
    {
      "name": "wangwang",
      "animalType": "dog"
    },
    {
      "name": "miao",
      "animalType": "cat"
    }
  ]
}
```

```java
@JsonTypeInfo(use = JsonTypeInfo.Id.NAME, include = JsonTypeInfo.As.EXISTING_PROPERTY,
        property = "animalType")
@JsonSubTypes({
        @JsonSubTypes.Type(value = Dog.class, name = "dog"),
        @JsonSubTypes.Type(value = Cat.class, name = "cat")})
@JsonIgnoreProperties(ignoreUnknown = true)
public interface Animal {
    String getAnimalType();
    String getName();
    void setName(String name);
}
```
这种类型还有很多变种。例如 `use` 可以选择 `JsonTypeInfo.Id.CLASS` 之类的。


# 把 POJO 映射称为数组 , `include = JsonTypeInfo.As.WRAPPER_ARRAY`

```
{
  "leader": [
    "dog",
    {
      "name": "wangwang",
      "animalType": "dog"
    }
  ],
  "followers": [
    [
      "dog",
      {
        "name": "wangwang",
        "animalType": "dog"
      }
    ],
    [
      "cat",
      {
        "name": "miao",
        "animalType": "cat"
      }
    ]
  ]
}
```

每个POJO 映射为一个两个元素的 JSON 数组，第一个元素表示类型，第二个元素表示真正的内容。

# 把 POJO 映射称为包装对象 `nclude = JsonTypeInfo.As.WRAPPER_OBJECT`

```json
{
  "leader": {
    "dog": {
      "name": "wangwang",
      "animalType": "dog"
    }
  },
  "followers": [
    {
      "dog": {
        "name": "wangwang",
        "animalType": "dog"
      }
    },
    {
      "cat": {
        "name": "miao",
        "animalType": "cat"
      }
    }
  ]
}
{
  "leader": {
    "dog": {
      "name": "wangwang",
      "animalType": "dog"
    }
  },
  "followers": [
    {
      "dog": {
        "name": "wangwang",
        "animalType": "dog"
      }
    },
    {
      "cat": {
        "name": "miao",
        "animalType": "cat"
      }
    }
  ]
}
```
每个 POJO 映射为一个 只有一个 property 的 POJO Object ，这个 property name 表示类型信息。 property value 是 POJO 真正的内容。
