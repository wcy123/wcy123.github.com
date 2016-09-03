---
layout: post
title:  "使用 Spring Integration Framework 写入 redis 队列"
date:   2016/09/03 19:05:30
categories:
comments: true
---


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


src/main/resources/META-INF/spring/si-components.xml

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

    <int:channel id="messageChannel"> <!--<int:queue capacity="2" />--> </int:channel>
    <bean id="redisConnectionFactory"
          class="org.springframework.data.redis.connection.jedis.JedisConnectionFactory">
        <property name="port" value="7638" />
    </bean>
    <int-redis:queue-outbound-channel-adapter
            id="queue"
            channel="messageChannel" queue="a_queue"
            connection-factory="redisConnectionFactory"
            left-push="false"
    ></int-redis:queue-outbound-channel-adapter>
</beans>
```
