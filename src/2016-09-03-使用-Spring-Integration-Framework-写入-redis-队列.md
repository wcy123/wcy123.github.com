# 使用 Spring Integration Framework 写入 redis 队列



[Spring Integration Framework][] 提供了应用集成的一种方式。各个应用程
序，组件，通过 Channel, Message 松散耦合在一起。

[Spring Integration Framework]: http://docs.spring.io/spring-integration/reference/html/redis.html

这个例子演示了如何从 stdin 读取用户输入，然后写入 redis 队列的。

```xml
<?xml version="1.0" encoding="UTF-8"?>
<project xmlns="http://maven.apache.org/POM/4.0.0" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
	xsi:schemaLocation="http://maven.apache.org/POM/4.0.0 http://maven.apache.org/xsd/maven-4.0.0.xsd">
	<modelVersion>4.0.0</modelVersion>

	<groupId>com.wcy123.example.spring.integration.redis</groupId>
	<artifactId>si-redis</artifactId>
	<version>0.0.1-SNAPSHOT</version>
	<packaging>jar</packaging>

	<name>si-redis</name>
	<description>Demo project for Spring Boot</description>

	<parent>
		<groupId>org.springframework.boot</groupId>
		<artifactId>spring-boot-starter-parent</artifactId>
		<version>1.4.0.RELEASE</version>
		<relativePath/> <!-- lookup parent from repository -->
	</parent>

	<properties>
		<project.build.sourceEncoding>UTF-8</project.build.sourceEncoding>
		<project.reporting.outputEncoding>UTF-8</project.reporting.outputEncoding>
		<java.version>1.8</java.version>
	</properties>

	<dependencies>
		<dependency>
			<groupId>org.springframework.boot</groupId>
			<artifactId>spring-boot-starter-data-redis</artifactId>
		</dependency>
		<dependency>
			<groupId>org.springframework.boot</groupId>
			<artifactId>spring-boot-starter-integration</artifactId>
		</dependency>

		<dependency>
			<groupId>org.springframework.boot</groupId>
			<artifactId>spring-boot-starter-test</artifactId>
			<scope>test</scope>
		</dependency>
        <dependency>
            <groupId>org.springframework.integration</groupId>
            <artifactId>spring-integration-stream</artifactId>
        </dependency>
        <dependency>
            <groupId>org.springframework.integration</groupId>
            <artifactId>spring-integration-redis</artifactId>
        </dependency>
	</dependencies>
	<build>
		<plugins>
			<plugin>
				<groupId>org.springframework.boot</groupId>
				<artifactId>spring-boot-maven-plugin</artifactId>
			</plugin>
		</plugins>
	</build>
</project>
```

上面大部分是自动生成的，自己加的只有

```xml
        <dependency>
            <groupId>org.springframework.integration</groupId>
            <artifactId>spring-integration-stream</artifactId>
        </dependency>
        <dependency>
            <groupId>org.springframework.integration</groupId>
            <artifactId>spring-integration-redis</artifactId>
        </dependency>
```

引入 `spring-integration-stream` ，我们可以解析 xml 中的 namespace

```xml
xmlns:int-stream="http://www.springframework.org/schema/integration/stream"
```

`spring-integration-stream` 的引入，我们可以解析 xml 中的 namespace

```xml
xmlns:int-redis="http://www.springframework.org/schema/integration/redis">
```

下面的 spring boot 生成的程序框架。

```java
// src/main/java/com/wcy123/example/spring/integration/redis/SiRedisApplication.java
package com.wcy123.example.spring.integration.redis;

import org.springframework.boot.SpringApplication;
import org.springframework.boot.autoconfigure.SpringBootApplication;
import org.springframework.context.annotation.ImportResource;
import org.springframework.integration.config.EnableIntegration;

@SpringBootApplication
@EnableIntegration
@ImportResource("classpath:/META-INF/spring/si-components.xml")
public class SiRedisApplication {

	public static void main(String[] args) {
		SpringApplication.run(SiRedisApplication.class, args);
	}
}
```


`@EnableIntegration` 标注一个 Configuration 类是 Spring Integration 的配置。

```xml
@ImportResource("classpath:/META-INF/spring/si-components.xml")
```

用来引入 spring integration 的 beam 定义。目前 4.3.0 中 ，不是所有的
spring integration 组件都支持 java annotation 的配置，很多还是依赖 xml
的配置。




src/main/resources/META-INF/spring/si-components.xml 的内容

```xml
<?xml version="1.0" encoding="UTF-8"?>
<beans xmlns="http://www.springframework.org/schema/beans"
       xmlns:int="http://www.springframework.org/schema/integration"
       xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
       xmlns:int-stream="http://www.springframework.org/schema/integration/stream"
       xsi:schemaLocation="
	http://www.springframework.org/schema/beans http://www.springframework.org/schema/beans/spring-beans.xsd
	http://www.springframework.org/schema/integration http://www.springframework.org/schema/integration/spring-integration.xsd
	http://www.springframework.org/schema/integration/stream http://www.springframework.org/schema/integration/stream/spring-integration-stream.xsd
	http://www.springframework.org/schema/integration/redis  http://www.springframework.org/schema/integration/redis/spring-integration-redis.xsd"
       xmlns:int-redis="http://www.springframework.org/schema/integration/redis">

    <int-stream:stdin-channel-adapter id="producer" channel="messageChannel"/>

    <int:poller id="defaultPoller" default="true"
                max-messages-per-poll="5" fixed-rate="200"/>

    <int:channel id="messageChannel"/>
    <bean id="redisConnectionFactory"
          class="org.springframework.data.redis.connection.jedis.JedisConnectionFactory">
    </bean>
    <int-redis:queue-outbound-channel-adapter
            id="queue"
            channel="messageChannel" queue="a_queue"
            connection-factory="redisConnectionFactory"
            left-push="false"
    ></int-redis:queue-outbound-channel-adapter>
</beans>
```


```xml
    <int-stream:stdin-channel-adapter id="producer" channel="messageChannel"/>

    <int:poller id="defaultPoller" default="true"
                max-messages-per-poll="5" fixed-rate="200"/>
```

创建了一个 endpoint ，每隔 200 ms ，从标准输入读取数据，然后扔到
`messageChannel` 中。

```xml
<int:channel id="messageChannel"/>
```

创建了一个 `messageChannel` 的 channel。

```xml
    <int-redis:queue-outbound-channel-adapter
            id="queue"
            channel="messageChannel" queue="a_queue"
            connection-factory="redisConnectionFactory"
            left-push="false"
    ></int-redis:queue-outbound-channel-adapter>

```

会从 `messageChannel` 中读取 Message , 然后用 `rpush` 命令，把消息压到
消息队列 `a_queue` 中。

`queue-outbound-channel-adapter` 需要一个 redis connection factory

```xml
    <bean id="redisConnectionFactory"
          class="org.springframework.data.redis.connection.jedis.JedisConnectionFactory" />
```

这里创建了一个 redis connection factory ， 这个 bean 有很多属性，用于
表明连接的主机名称，密码，端口号，是否使用连接池等等。
