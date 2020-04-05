# 使用 Protobuf 设计 REST API


# 概述

一个设计的好的 REST API 接口，需要一个严格的接口定义。本文试图使用 Protobuf 作为接口设计语言，设计 API。

创建文件，`main/proto/Login.proto`

```protobuf
syntax = "proto3";
package org.wcy123.api;
option java_outer_classname = "Protos";
message LoginRequest {
    string name = 1;
    string password = 2;
}
message LoginResponse {
    enum LoginResult {
        DEFAULT = 0;
        OK = 1;
        FAIL = 2;
    }
    LoginResult result  = 1;
}
```

然后创建一个 Bean 用于转换 JSON 到 Java 对象

```java
@Bean
ProtobufHttpMessageConverter protobufHttpMessageConverter() {
  return new ProtobufHttpMessageConverter();
}
```

然后创建 Controller

```java
@RestController
@Slf4j
public class MyRestController {
    @RequestMapping(path = "/login", method = POST)
    public ResponseEntity<Protos.LoginResponse> login(HttpServletRequest request,
            HttpServletResponse response, @RequestBody Protos.LoginRequest command) {
        log.info("input is {}", new JsonFormat().printToString(command));
        return ResponseEntity.ok().body(Protos.LoginResponse.newBuilder()
                .setResult(Protos.LoginResponse.LoginResult.OK)
                .build());
    }
}
```

`Protos.LoginRequest` 和 `Protos.LoginResponse` 是自动生成的

测试程序

```http
curl -v -s -H 'Content-Type: application/json' -H 'Accept: application/json' http://127.0.0.1:8080/login -d '{"name":"wcy123", "password":"123456"}'
*   Trying 127.0.0.1...
* Connected to 127.0.0.1 (127.0.0.1) port 8080 (#0)
> POST /login HTTP/1.1
> Host: 127.0.0.1:8080
> User-Agent: curl/7.43.0
> Content-Type: application/json
> Accept: application/json
> Content-Length: 39
>
* upload completely sent off: 39 out of 39 bytes
< HTTP/1.1 200
< Content-Type: application/json
< Transfer-Encoding: chunked
< Date: Tue, 22 Nov 2016 10:03:43 GMT
<
* Connection #0 to host 127.0.0.1 left intact
{"result": "OK"}
```
从外部看开，请求和响应都是 JSON，PB 只是内部实现。注意要加 `Content-Type:application/json` 和 `Accept: application/json` 两个 header。

## Protobuf 支持的常用数据类型和  JSON之间的转换关系。

```protobuf
syntax = "proto3";
package org.wcy123.api;
message Root {
    Obj1 obj1 = 1;
    bytes base64 = 2;
    enum AnEnum { FOO_BAR = 0 ; GOOD = 2; BYE = 3;};
    AnEnum anEnum = 3;
    repeated string anArray = 4;
    bool aBoolean = 5;
    string aString = 6;
    int32 aInt32 = 7;
    uint32 aUint32 = 8;
    fixed32 aFixed = 9;
    int64 aInt64 = 10;
    uint64 aUint64 = 11;
    fixed64 aFixed64 = 12;
    float aFloat = 13;
    double aDouble = 14;
    //Timestamp aTimestamp = 15;
    //Duration aDuaration = 16;
    oneof anOneOf {
        string eitherString = 17;
        int32 orAnInt = 18;
    }
}
message Obj1{
    string name = 1;
}
```
我写了一个例子，用于实验所有的改动，具体文档，参考 https://developers.google.com/protocol-buffers/docs/proto3#json
这个转换成  JSON 是下面这个样子。

```json
{
  "obj1": {
    "name": "YourName"
  },
  "base64": "abc123!?$*&()'-=@~",
  "aBoolean": true,
  "aString": "hello world",
  "aInt32": -32,
  "aUint32": 32,
  "aFixed": 64,
  "aInt64": -64,
  "aUint64": 64,
  "aFixed64": 128,
  "aFloat": 1,
  "aDouble": 2,
  "eitherString": "Now It is a String, not an integer"
}
```

注意到 `oneOf` 使用字段名称来表示哪一个字段被选择了。


##  细节
### 依赖关系

```xml
<dependency>
  <groupId>com.google.protobuf</groupId>
  <artifactId>protobuf-java</artifactId>
  <version>3.0.0-beta-1</version>
</dependency>
```

需要插件

```xml
<plugin>
  <groupId>org.xolstice.maven.plugins</groupId>
  <artifactId>protobuf-maven-plugin</artifactId>
  <version>0.5.0</version>
  <configuration>
    <protocExecutable>/usr/local/bin/protoc</protocExecutable>
  </configuration>
  <executions>
    <execution>
      <goals>
        <goal>compile</goal>
        <goal>test-compile</goal>
      </goals>
    </execution>
  </executions>
</plugin>
```
### 默认值的处理

注意到在 `OK` 和 `FAIL` 前面，我放了一个无用的 `DEFAULT` 值。如果没有这个值，那么 JSON 转换的时候，认为 `result` 是默认值，那么就不会包含这个字段。
* 如果要强制包含这个字段，那么填一个无用的默认值，占位，永远不用这个默认值。
* 如果想节省带宽，默认值就不传输的话，那么就保留这个行为。接收端需要判断如果字段不存在，就使用默认值。

### 不能识别的 JSON 字段

PB 忽略不能识别的字段。
