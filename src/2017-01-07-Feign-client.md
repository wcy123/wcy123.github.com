# Feign client


1. `@EnableDiscoveryClient`
2. pom 中包含

```xml
<dependency>
<groupId>org.springframework.cloud</groupId>
<artifactId>spring-cloud-starter-consul-discovery</artifactId>
</dependency>
```

3. 打开参数 `spring.cloud.discovery.enabled=true`。
参考 `EnableDiscoveryClientImportSelector::isEnabled`

4. 打开 `spring.cloud.consul.discovery.enabled=true` 参考 ConsulDiscoveryProperties

5. 打开 `spring.cloud.consul.enabled=true` 参考 ConsulProperties

6. 记得加

```text
headers = {"Accept = application/json", "Content-Type = application/json"}
```

否则返回值会无法解码

# enable debug

```java
@Bean
Logger.Level logLevel() {
    return Logger.Level.FULL;
}
```

官方参考手册 http://cloud.spring.io/spring-cloud-netflix/spring-cloud-netflix.html

```text
logging.level.<YOURINTERFACE>=trace
```

如果看到下面的日志，可以调试 rest 输入输出。

```log
2016-12-29 19:20:49.460 DEBUG 56282 --- [CrmRestServer-2] com.easemob.weichat.rest.mvc.dep.CrmApi  : [CrmApi#queryCustomerInternal] ---> GET http://CrmRestServer/v1/crm/tenants/1/agents/aa986a74-bf25-4f64-85be-e1fcf9726069/customers HTTP/1.1
2016-12-29 19:20:49.462 DEBUG 56282 --- [CrmRestServer-2] com.easemob.weichat.rest.mvc.dep.CrmApi  : [CrmApi#queryCustomerInternal] Accept : application/json
2016-12-29 19:20:49.462 DEBUG 56282 --- [CrmRestServer-2] com.easemob.weichat.rest.mvc.dep.CrmApi  : [CrmApi#queryCustomerInternal] Content-Type : application/json
2016-12-29 19:20:49.463 DEBUG 56282 --- [CrmRestServer-2] com.easemob.weichat.rest.mvc.dep.CrmApi  : [CrmApi#queryCustomerInternal] Content-Type: application/json;charset=UTF-8
2016-12-29 19:20:49.463 DEBUG 56282 --- [CrmRestServer-2] com.easemob.weichat.rest.mvc.dep.CrmApi  : [CrmApi#queryCustomerInternal] Content-Length: 94
2016-12-29 19:20:49.463 DEBUG 56282 --- [CrmRestServer-2] com.easemob.weichat.rest.mvc.dep.CrmApi  : [CrmApi#queryCustomerInternal]
2016-12-29 19:20:49.463 DEBUG 56282 --- [CrmRestServer-2] com.easemob.weichat.rest.mvc.dep.CrmApi  : [CrmApi#queryCustomerInternal] {"page":"1","size":"20","userTagIds":"221312","visitorName":"abc","beginDate":"","endDate":""}
2016-12-29 19:20:49.464 DEBUG 56282 --- [CrmRestServer-2] com.easemob.weichat.rest.mvc.dep.CrmApi  : [CrmApi#queryCustomerInternal] ---> END HTTP (94-byte body)



2016-12-29 19:20:52.200 DEBUG 56282 --- [CrmRestServer-2] com.easemob.weichat.rest.mvc.dep.CrmApi  : [CrmApi#queryCustomerInternal] <--- HTTP/1.1 404 Not Found (2736ms)
2016-12-29 19:20:52.202 DEBUG 56282 --- [CrmRestServer-2] com.easemob.weichat.rest.mvc.dep.CrmApi  : [CrmApi#queryCustomerInternal] Transfer-Encoding: chunked
2016-12-29 19:20:52.202 DEBUG 56282 --- [CrmRestServer-2] com.easemob.weichat.rest.mvc.dep.CrmApi  : [CrmApi#queryCustomerInternal] Server: Apache-Coyote/1.1
2016-12-29 19:20:52.202 DEBUG 56282 --- [CrmRestServer-2] com.easemob.weichat.rest.mvc.dep.CrmApi  : [CrmApi#queryCustomerInternal] X-Application-Context: CrmRestServer:8590
2016-12-29 19:20:52.202 DEBUG 56282 --- [CrmRestServer-2] com.easemob.weichat.rest.mvc.dep.CrmApi  : [CrmApi#queryCustomerInternal] Date: Thu, 29 Dec 2016 11:20:52 GMT
2016-12-29 19:20:52.202 DEBUG 56282 --- [CrmRestServer-2] com.easemob.weichat.rest.mvc.dep.CrmApi  : [CrmApi#queryCustomerInternal] Content-Type: text/json;charset=UTF-8
2016-12-29 19:20:52.203 DEBUG 56282 --- [CrmRestServer-2] com.easemob.weichat.rest.mvc.dep.CrmApi  : [CrmApi#queryCustomerInternal]
2016-12-29 19:20:52.360 DEBUG 56282 --- [CrmRestServer-2] com.easemob.weichat.rest.mvc.dep.CrmApi  : [CrmApi#queryCustomerInternal] {"timestamp":1483010451922,"status":404,"error":"Not Found","message":"No message available","path":"/v1/crm/tenants/1/agents/aa986a74-bf25-4f64-85be-e1fcf9726069/customers"}
2016-12-29 19:20:52.361 DEBUG 56282 --- [CrmRestServer-2] com.easemob.weichat.rest.mvc.dep.CrmApi  : [CrmApi#queryCustomerInternal] <--- END HTTP (174-byte body)
```

```config
logging.level.com.netflix.loadbalancer=debug
```

例如，可以看到下面的东西，就是错的。很有可能 spring.cloud.consul.enabled=true 没有配置

```log
2016-12-29 19:17:00.202 DEBUG 55945 --- [CrmRestServer-2] c.n.l.DynamicServerListLoadBalancer      : List of Servers for CrmRestServer obtained from Discovery client: []
```

如果看到下面这样，那么 consul 就是对的。

```log
2016-12-29 19:20:51.015 DEBUG 56282 --- [erListUpdater-0] c.n.l.DynamicServerListLoadBalancer      : Filtered List of Servers for CrmRestServer obtained from Discovery client: [172.17.1.201:8590]
```

设置超时阈值

```config
hystri‌x.command.default.ex‌ecution.isolation.th‌read.timeoutInMillis‌econds=5000
```

这个只供调试用。因为把默认的超时都改成了 5s

```config
hystri‌x.command.<methodName>.ex‌ecution.isolation.th‌read.timeoutInMillis‌econds=30000
```

设置参数使Fallback可用

```config
feign.hystrix.enabled=true
```
