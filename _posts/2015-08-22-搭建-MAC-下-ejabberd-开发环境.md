---
layout: post
title:  "搭建 MAC 下 ejabberd 开发环境"
date:   2015/08/22 14:20:07
categories:
comments: true
draft: true
---


## 下载安装 erlang

 - [R18](http://www.erlang.org/download/otp_src_18.0.tar.gz)
 - [R17.5](http://www.erlang.org/download/otp_src_17.5.tar.gz)

## 依赖库

 - 安装 brew , erlang 其实不依赖这个库， 仅仅为了方便安装其他库，[参考这里](http://brew.sh)

 ```
 ruby -e "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/master/install)"
 ```

 - 安装 odbc

 ```
 brew install unixodbc
 ```

 - 安装 openssl

 ```
 brew install openssl
 ```

 - 安装 JDK ，下载，打开，下一步，下一步, ... ，略过。

## 从源代码安装 Erlang

```
export CFLAGS='-I/usr/local/Cellar/unixodbc/2.3.2_1/include/'
export LDFLAGS='-L/usr/local/Cellar/unixodbc/2.3.2_1/lib/'

# for R17
./configure --prefix=/usr/local/Cellar/erlang/R17.5 --with-odbc=/usr/local/Cellar/unixodbc/2.3.2_1 --with-ssl=/usr/local/Cellar/openssl/1.0.2d_1
# for R18
./configure --prefix=/usr/local/Cellar/erlang/R18.0 --with-odbc=/usr/local/Cellar/unixodbc/2.3.2_1 --with-ssl=/usr/local/Cellar/openssl/1.0.2d_1
```

修改 `~/.bash_profile` 方便在两个版本切换

```
alias r17='export PATH=/usr/local/Cellar/erlang/R17.5/bin:/usr/local/bin:/usr/bin:/bin:/usr/sbin:/sbin'
alias r18='export PATH=/usr/local/Cellar/erlang/R18.0/bin:/usr/local/bin:/usr/bin:/bin:/usr/sbin:/sbin'

## brew 有的时候会提示带宽不够，根据提示，设置下面的环境变量，这个串可
## 以在 github 网页上的个人帐户设置上找到export
HOMEBREW_GITHUB_API_TOKEN=b78529xxxxxxxxxxxxxxxxxxxxx4de83f
```


## 安装 rebar

官方有一个 [rebar](https://github.com/basho/rebar)， 我就用我自己 fork
的一个版本。我做了一个小修改，在 R18 下面就没有编译错误了。其他的版本
也有类似修改。

```
mkdir ~/d/working
cd ~/d/working
git clone https://github.com/wcy123/rebar.git
cd rebar
./bootstrap
sudo cp ./rebar /usr/local/bin/
```

## 安装 distel

这个库不是必须安装，仅对 emacs 开发有帮助。官方有一个版本
[distel](https://github.com/massemanet/distel) ， 我用我自己 fork 的一
个版本。 我做了一个小修改，调试的时候，`f` 键可以返回当前函数，也就是
退栈。

## 配置 emacs


{% highlight elisp %}
;; 这个用来自动监测 erl 安装在哪里了。方便切换不同的 erlang 版本，
;; 如果不在乎版本的话，直接写下面的，也是可以的。
;;    (setq erlang-root-dir "where_your_install_erlang")
;;    (add-to-list 'load-path "where_your_erlang_mode_el")
(let* ((erl-exec (locate-file "escript" exec-path)))
  (when erl-exec
    (let ((temp-file (make-temp-file "erl")))
      (with-temp-file temp-file
        (insert "#!/usr/bin/env escript
-export([main/1]).
main(_) ->
    ToolPath = code:lib_dir(tools),
    RootDir = code:root_dir(),
    io:format(\"(setq erlang-root-dir ~p)~n\",[RootDir]),
    ElangModePath = filename:join(ToolPath, \"emacs\"),
    io:format(\"(add-to-list 'load-path ~p)~n\",[ElangModePath]).
"))
      (with-temp-buffer
        (call-process-shell-command "escript"
                                    nil ;input
                                    (current-buffer) ;output
                                    nil      ;display
                                    temp-file)
        (message "%s" (buffer-string))
        (delete-file temp-file)
        (eval-buffer)
        ))))
  (require 'erlang)
  ;;  this is set properly in the detection period
  ;; (setq erlang-root-dir  "/home2/chunywan/d/local/lib/erlang")
  (setq inferior-erlang-machine-options ;; 自动生成唯一的 node name
        (list "-sname"
              (format "%s" (emacs-pid))))
  (setq erlang-compile-extra-opts
        (list '(i . "./include") ;; 根据 otp 的目录结构，一般都需要 include
              'export_all ;; 调试的时候方便
              (cons 'i (expand-file-name
                     "d/working/easemob/ejabberd/deps/p1_xml/include"
                     (getenv "HOME"))) ;; 调试 p1_xml 的时候方便
              'debug_info))
  ;; TODO: this is no good way to detect distel is installed.
  (let ((distel-root (expand-file-name "~/d/working/distel")))
    (when (file-exists-p distel-root)
      (let ((dist-el (expand-file-name "elisp" distel-root)))
        (add-to-list 'load-path dist-el)
        (require 'distel)
        (distel-setup)
        (defconst distel-shell-keys
          '(("M-i" erl-complete)
            ("M-?" erl-complete)
            ("M-." erl-find-source-under-point)
            ("M-," erl-find-source-unwind)
            ("M-*" erl-find-source-unwind)
            )
          "Additional keys to bind when in Erlang shell.")
        (defun erlang-shell-mode-hook-1 ()
          ;; add some Distel bindings to the Erlang shell
          (dolist (spec distel-shell-keys)
            (define-key erlang-shell-mode-map (read-kbd-macro (car spec)) (cadr spec))))
        (add-hook 'erlang-shell-mode-hook 'erlang-shell-mode-hook-1))))
{% endhighlight %}




## 下载 easemob/ejabberd

```
git clone https://github.com/easemob/ejabberd.git
git checkout easemob-dev
```

## 安装 libyaml

```
brew install libyaml
brew list libyaml
```

## 安装 autotools

```
brew install libtool autoconf automake
```


## 编译 ejabberd

```
sh auto_gen.sh
export CFLAGS=-I/usr/local/Cellar/libyaml/0.1.6_1/include/
export LDFLAGS='-L/usr/local/Cellar/libyaml/0.1.6_1/lib'
./configure --enable-zlib --enable-odbc --enable-mysql --enable-tools
make
make rel
cp ejabberd.yml.exmaple rel/etc/ejabberd/ejabberd.yml
rel/bin/ejabberdctrl live
```

## 错误 OpenSsl 没有加载

```
11:39:57.262 [error] Unable to load crypto library. Failed with error:
"load, Library load-call unsuccessful."
OpenSSL might not be installed on this system.
```

这个由于动态链接库的版本不一致导致的，检查动态链接库的

```
my:p1_tls wangchunye$ otool -L /usr/local/Cellar/erlang/R17.5/lib/erlang/lib/crypto-3.5/priv/lib/crypto.so
/usr/local/Cellar/erlang/R17.5/lib/erlang/lib/crypto-3.5/priv/lib/crypto.so:
/usr/local/opt/openssl/lib/libcrypto.1.0.0.dylib (compatibility version 1.0.0, current version 1.0.0)
/usr/lib/libSystem.B.dylib (compatibility version 1.0.0, current version 1213.0.0)

my:p1_tls wangchunye$ otool -L priv/lib/p1_sha.so
priv/lib/p1_sha.so:
/usr/local/opt/openssl/lib/libssl.1.0.0.dylib (compatibility version 1.0.0, current version 1.0.0)
/usr/local/opt/openssl/lib/libcrypto.1.0.0.dylib (compatibility version 1.0.0, current version 1.0.0)
/usr/lib/libSystem.B.dylib (compatibility version 1.0.0, current version 1213.0.0)
my:p1_tls wangchunye$ otool -L priv/lib/p1_tls_drv.so
priv/lib/p1_tls_drv.so:
/usr/local/opt/openssl/lib/libssl.1.0.0.dylib (compatibility version 1.0.0, current version 1.0.0)
/usr/local/opt/openssl/lib/libcrypto.1.0.0.dylib (compatibility version 1.0.0, current version 1.0.0)
/usr/lib/libSystem.B.dylib (compatibility version 1.0.0, current version 1213.0.0)

```

erlang 系统使用了 openssl `libcrypto` , `p1_sha.so`  `p1_tls_drv.so` 需要使用相同版本的 `libcrypto` 动态链接库。

在运行 `./configure` (ejabberd 目录中的) 之前，设置环境变量

```
export CFLAGS='-I/usr/local/opt/openssl/include'
export LDFLAGS='-L/usr/local/opt/penssl/lib'
```

## 配置文件


{% highlight yaml %}
###
###               ejabberd configuration file
###
###

### The parameters used in this configuration file are explained in more detail
### in the ejabberd Installation and Operation Guide.
### Please consult the Guide in case of doubts, it is included with
### your copy of ejabberd, and is also available online at
### http://www.process-one.net/en/ejabberd/docs/

### The configuration file is written in YAML.
### Refer to http://en.wikipedia.org/wiki/YAML for the brief description.
### However, ejabberd treats different literals as different types:
###
### - unquoted or single-quoted strings. They are called "atoms".
###   Example: dog, 'Jupiter', '3.14159', YELLOW
###
### - numeric literals. Example: 3, -45.0, .0
###
### - quoted or folded strings.
###   Examples of quoted string: "Lizzard", "orange".
###   Example of folded string:
###   > Art thou not Romeo,
###     and a Montague?

###   =========
###   DEBUGGING

##
## loglevel: Verbosity of log files generated by ejabberd.
## 0: No ejabberd log at all (not recommended)
## 1: Critical
## 2: Error
## 3: Warning
## 4: Info
## 5: Debug
##
loglevel: 4

##
## loghwm: High water mark of log velocity.
## The configuration is usually for lager file backend,
## and to prevent lager from being overflowed by flood messages.
## Messages will be dropped if the rate of incoming messages exceeded the limit.
##
loghwm: 100

##
## machineid: MachineId for ticktick service.
##
machineid: 0

##
## store_nodes: specific nodes for session store
##
store_nodes:
  all:
    - "ejabberd@localhost"
  muc:
    - "ejabberd1@local"
  sub:
    - "ejabberd@localhost"


##
## node_role: role of this node, db or conn, default is conn
## db: will load or create session table
## conn: will NOT load or create session table
##
node_role: db

##
## watchdog_admins: Only useful for developers: if an ejabberd process
## consumes a lot of memory, send live notifications to these XMPP
## accounts.
##
## watchdog_admins:
##   - "bob@example.com"


###   ================
###   SERVED HOSTNAMES

##
## hosts: Domains served by ejabberd.
## You can define one or several, for example:
## hosts:
##   - "example.net"
##   - "example.com"
##   - "example.org"
##
hosts:
  - "easemob.com"

##
## route_subdomains: Delegate subdomains to other XMPP servers.
## For example, if this ejabberd serves example.org and you want
## to allow communication with an XMPP server called im.example.org.
##
## route_subdomains: s2s

###   ===============
###   LISTENING PORTS

##
## listen: The ports ejabberd will listen on, which service each is handled
## by and what options to start it with.
##
listen:
  -
    port: 5222
    module: ejabberd_c2s
    ##
    ## If TLS is compiled in and you installed a SSL
    ## certificate, specify the full path to the
    ## file and uncomment this line:
    ##
    ## certfile: "/path/to/ssl.pem"
    ## starttls: true
    max_stanza_size: 65536
    shaper: c2s_shaper
    access: c2s
    zlib: true
  #-
  #  port: 5269
  #  module: ejabberd_s2s_in
  ##
  ## ejabberd_service: Interact with external components (transports, ...)
  ##
  ## -
  ##   port: 8888
  ##   module: ejabberd_service
  ##   access: all
  ##   shaper_rule: fast
  ##   ip: "127.0.0.1"
  ##   hosts:
  ##     "icq.example.org":
  ##       password: "secret"
  ##     "sms.example.org":
  ##       password: "secret"

  ##
  ## ejabberd_stun: Handles STUN Binding requests
  ##
  ## -
  ##   port: 3478
  ##   transport: udp
  ##   module: ejabberd_stun

  ##
  ## To handle XML-RPC requests that provide admin credentials:
  ##
  ## -
  ##   port: 4560
  ##   module: ejabberd_xmlrpc
  -
    port: 5280
    module: ejabberd_http
    request_handlers:
      "/api": mod_easemob_api
      "/ws" : ejabberd_http_ws
    web_admin: true
    http_poll: true
    http_bind: true
    register: true
    #captcha: true

##
## s2s_use_starttls: Enable STARTTLS + Dialback for S2S connections.
## Allowed values are: false optional required required_trusted
## You must specify a certificate file.
##
## s2s_use_starttls: optional

##
## s2s_certfile: Specify a certificate file.
##
## s2s_certfile: "/path/to/ssl.pem"

##
## domain_certfile: Specify a different certificate for each served hostname.
##
## host_config:
##   "example.org":
##     domain_certfile: "/path/to/example_org.pem"
##   "example.com":
##     domain_certfile: "/path/to/example_com.pem"

##
## S2S whitelist or blacklist
##
## Default s2s policy for undefined hosts.
##
## s2s_policy: s2s_access

##
## Outgoing S2S options
##
## Preferred address families (which to try first) and connect timeout
## in milliseconds.
##
## outgoing_s2s_families:
##   - ipv4
##   - ipv6
## outgoing_s2s_timeout: 10000

###   ==============
###   AUTHENTICATION

##
## auth_method: Method used to authenticate the users.
## The default method is the internal.
## If you want to use a different method,
## comment this line and enable the correct ones.
##
auth_method: internal

##
## Store the plain passwords or hashed for SCRAM:
## auth_password_format: plain
## auth_password_format: scram
##
## Define the FQDN if ejabberd doesn't detect it:
## fqdn: "server3.example.com"

##
## Authentication using external script
## Make sure the script is executable by ejabberd.
##
## auth_method: external
## extauth_program: "/path/to/authentication/script"

##
## Authentication using thrift rpc
## Remmember to configure the pool size for concurrency
##
##auth_method: thrift
thrift_servers:
  - "localhost:9147"
  - "localhost:9147"

thrift_pool_size: 100
thrift_bypass: false

##
## Authentication using ODBC
## Remember to setup a database in the next section.
##
## auth_method: odbc

##
## Authentication using PAM
##
## auth_method: pam
## pam_service: "pamservicename"

##
## Authentication using LDAP
##
## auth_method: ldap
##
## List of LDAP servers:
## ldap_servers:
##   - "localhost"
##
## Encryption of connection to LDAP servers:
## ldap_encrypt: none
## ldap_encrypt: tls
##
## Port to connect to on LDAP servers:
## ldap_port: 389
## ldap_port: 636
##
## LDAP manager:
## ldap_rootdn: "dc=example,dc=com"
##
## Password of LDAP manager:
## ldap_password: "******"
##
## Search base of LDAP directory:
## ldap_base: "dc=example,dc=com"
##
## LDAP attribute that holds user ID:
## ldap_uids:
##   - "mail": "%u@mail.example.org"
##
## LDAP filter:
## ldap_filter: "(objectClass=shadowAccount)"

##
## Anonymous login support:
##   auth_method: anonymous
##   anonymous_protocol: sasl_anon | login_anon | both
##   allow_multiple_connections: true | false
##
## host_config:
##   "public.example.org":
##     auth_method: anonymous
##     allow_multiple_connections: false
##     anonymous_protocol: sasl_anon
##
## To use both anonymous and internal authentication:
##
## host_config:
##   "public.example.org":
##     auth_method:
##       - internal
##       - anonymous

###   ==============
###   DATABASE SETUP

## ejabberd by default uses the internal Mnesia database,
## so you do not necessarily need this section.
## This section provides configuration examples in case
## you want to use other database backends.
## Please consult the ejabberd Guide for details on database creation.

##
## MySQL server:
##
## odbc_type: mysql
## odbc_server: "server"
## odbc_database: "database"
## odbc_username: "username"
## odbc_password: "password"
##
## If you want to specify the port:
## odbc_port: 1234
odbc_type: mysql
odbc_server: "172.16.0.48"
odbc_database: "ejabberd"
odbc_username: "ejabberd"
odbc_password: "thepushbox"
odbc_port: 3306
odbc_pool_size: 2
odbc_keepalive_interval: 90

##
## The sharding configuration will lead to 32*2048 tables among 32 host databases,
## which means databases will be numberd from 0 to 31, while tables 0 to 65535 (31*2048+31).
## Note that, number 32 will be the default configuration. (MAGIC!)
##
## odbc_shards:
##   32:
##     server: "server"
##     port: 3306
##     database: "database"
##     username: "username"
##     password: "password"
##     pool_size: 2
##   ## shards specific parameters
##   0:
##     server: "server"
##     database: "database"
##     username: "username"
##     password: "password"
##     pool_size: 10
##   11:
##     server: "server"
##     database: "database"
##     username: "username"
##     password: "password"

odbc_shards:
  32:
    server: "172.16.0.48"
    port: 3306
    database: "ejabberd"
    username: "ejabberd"
    password: "thepushbox"
    pool_size: 2
  ## shards specific parameters
  0:
    server: "172.16.0.48"
  1:
    server: "172.16.0.48"
  2:
    server: "172.16.0.48"
  4:
    server: "172.16.0.48"
  5:
    server: "172.16.0.48"
  6:
    server: "172.16.0.48"
  7:
    server: "172.16.0.48"
  8:
    server: "172.16.0.48"
  9:
    server: "172.16.0.48"
  10:
    server: "172.16.0.48"
  11:
    server: "172.16.0.48"
  12:
    server: "172.16.0.48"
  13:
    server: "172.16.0.48"
  14:
    server: "172.16.0.48"
  15:
    server: "172.16.0.48"
  16:
    server: "172.16.0.48"
  17:
    server: "172.16.0.48"
  18:
    server: "172.16.0.48"
  19:
    server: "172.16.0.48"
  20:
    server: "172.16.0.48"
  21:
    server: "172.16.0.48"
  22:
    server: "172.16.0.48"
  23:
    server: "172.16.0.48"
  24:
    server: "172.16.0.48"
  25:
    server: "172.16.0.48"
  26:
    server: "172.16.0.48"
  27:
    server: "172.16.0.48"
  28:
    server: "172.16.0.48"
  29:
    server: "172.16.0.48"
  30:
    server: "172.16.0.48"
  31:
    server: "172.16.0.48"

##
## odbc_shards_prev:
##   32:
##     server: "server"
##     port: 3306
##     database: "database"
##     username: "username"
##     password: "password"
##     pool_size: 2
##   ## shards specific parameters
##   0:
##     server: "server"
##     database: "database"
##     username: "username"
##     password: "password"
##     pool_size: 10

##
## PostgreSQL server:
##
## odbc_type: pgsql
## odbc_server: "server"
## odbc_database: "database"
## odbc_username: "username"
## odbc_password: "password"
##
## If you want to specify the port:
## odbc_port: 1234
##
## If you use PostgreSQL, have a large database, and need a
## faster but inexact replacement for "select count(*) from users"
##
## pgsql_users_number_estimate: true

##
## ODBC compatible or MSSQL server:
##
## odbc_type: odbc
## odbc_server: "DSN=ejabberd;UID=ejabberd;PWD=ejabberd"

##
## Number of connections to open to the database for each virtual host
##
## odbc_pool_size: 10

##
## Interval to make a dummy SQL request to keep the connections to the
## database alive. Specify in seconds: for example 28800 means 8 hours
##
## odbc_keepalive_interval: undefined

###   ===============
###   TRAFFIC SHAPERS

shaper:
  ##
  ## The "normal" shaper limits traffic speed to 1000 B/s
  ##
  normal: 1000

  ##
  ## The "fast" shaper limits traffic speed to 50000 B/s
  ##
  fast: 50000

  ##
  ## The "rest" shaper limit traffic speed to 1000 message/s for rest if
  ##
  rest: 1000

##
## This option specifies the maximum number of elements in the queue
## of the FSM. Refer to the documentation for details.
##
max_fsm_queue: 1000

###.   ====================
###'   ACCESS CONTROL LISTS
acl:
  ##
  ## The 'admin' ACL grants administrative privileges to XMPP accounts.
  ## You can put here as many accounts as you want.
  ##
  admin:
     user:
       - "admin": "localhost"
       - "admin": "easemob.com"
  ##
  ## Blocked users
  ##
  ## blocked:
  ##   user:
  ##     - "baduser": "example.org"
  ##     - "test"
  ## Local users: don't modify this.
  ##
  local:
    user_regexp: ""

  ##
  ## More examples of ACLs
  ##
  ## jabberorg:
  ##   server:
  ##     - "jabber.org"
  ## aleksey:
  ##   user:
  ##     - "aleksey": "jabber.ru"
  ## test:
  ##   user_regexp: "^test"
  ##   user_glob: "test*"

  ##
  ## Loopback network
  ##
  loopback:
    ip:
      - "127.0.0.0/8"

  ##
  ## Bad XMPP servers
  ##
  ## bad_servers:
  ##   server:
  ##     - "xmpp.zombie.org"
  ##     - "xmpp.spam.com"

##
## Define specific ACLs in a virtual host.
##
## host_config:
##   "localhost":
##     acl:
##       admin:
##         user:
##           - "bob-local": "localhost"

###   ============
###   ACCESS RULES
access:
  ## Maximum number of simultaneous sessions allowed for a single user:
  max_user_sessions:
    all: 10
  ## Maximum number of offline messages that users can have:
  max_user_offline_messages:
    admin: 5000
    all: 100
  ## This rule allows access only for local users:
  local:
    local: allow
  ## Only non-blocked users can use c2s connections:
  c2s:
    blocked: deny
    all: allow
  ## For C2S connections, all users except admins use the "normal" shaper
  c2s_shaper:
    admin: none
    all: normal
  ## All S2S connections use the "fast" shaper
  s2s_shaper:
    all: fast
  ## Only admins can send announcement messages:
  announce:
    admin: allow
  ## Only admins can use the configuration interface:
  configure:
    admin: allow
  ## Admins of this server are also admins of the MUC service:
  muc_admin:
    admin: allow
  ## Only accounts of the local ejabberd server can create rooms:
  muc_create:
    local: allow
  ## All users are allowed to use the MUC service:
  muc:
    all: allow
  ## Only accounts on the local ejabberd server can create Pubsub nodes:
  pubsub_createnode:
    local: allow
  ## In-band registration allows registration of any possible username.
  ## To disable in-band registration, replace 'allow' with 'deny'.
  register:
    all: allow
  ## Only allow to register from localhost
  trusted_network:
    loopback: allow
  api_admin:
    admin: allow
  ## Do not establish S2S connections with bad servers
  ## s2s_access:
  ##   bad_servers: deny
  ##   all: allow

## By default the frequency of account registrations from the same IP
## is limited to 1 account every 10 minutes. To disable, specify: infinity
## registration_timeout: 600
registration_timeout: infinity

##
## Define specific Access Rules in a virtual host.
##
## host_config:
##   "localhost":
##     access:
##       c2s:
##         admin: allow
##         all: deny
##       register:
##         all: deny

###   ================
###   DEFAULT LANGUAGE

##
## language: Default language used for server messages.
##
language: "en"

##
## Set a different default language in a virtual host.
##
## host_config:
##   "localhost":
##     language: "ru"

###   =======
###   CAPTCHA

##
## Full path to a script that generates the image.
##
## captcha_cmd: "/lib/ejabberd/priv/bin/captcha.sh"

##
## Host for the URL and port where ejabberd listens for CAPTCHA requests.
##
## captcha_host: "example.org:5280"

##
## Limit CAPTCHA calls per minute for JID/IP to avoid DoS.
##
## captcha_limit: 5

###   =======
###   MODULES

##
## Modules enabled in all ejabberd virtual hosts.
##
modules:
  ## mod_adhoc: {}
  ## mod_announce: # recommends mod_adhoc
  ##   access: announce
  mod_blocking: {} # requires mod_privacy
  mod_caps: {}
  mod_configure: {} # requires mod_adhoc
  mod_disco: {}
  ##  mod_echo: {}
  ##  mod_irc: {}
  mod_http_bind: {}
  ## mod_http_fileserver:
  ##   docroot: "/var/www"
  ##   accesslog: "/var/log/ejabberd/access.log"
  ##  mod_last: {}
  mod_easemob_cache:
    pool_size: 10
    redis_host: "172.16.0.48"
  mod_muc:
    ## host: "conference.@HOST@"
    access: muc
    access_create: muc_create
    access_persistent: muc_create
    access_admin: muc_admin
    max_user_conferences: 500
    min_message_interval: 50
    history_size: 10
    db_type: redis
  mod_muc_admin: {}
  mod_muc_log:
     dirname: room_jid
     dirtype: plain
     file_format: html
     outdir: "/opt/ejabberd/var/log/ejabberd/muc"
     timezone: universal
  ##mod_offline:
  # access_max_user_messages: max_user_offline_messages
  #   db_type: odbc
  mod_offline_shared: {}
  mod_message_log_redis:
    redis_topic: "ejabberd:chat:messages"
    redis_pool_size: 10
    redis_host: "172.16.0.48"
    redis_port: 6379
  mod_message_log:
    kafka_message_topic: "ejabberd-chat-messages"
    kafka_status_topic: "ejabberd-user-status-change"
    kafka_broker_host: "localhost"
    kafka_broker_port: 9091
    kafka_per_partition_workers: 2
  #mod_ping: {}
  ## mod_pres_counter:
  ##   count: 5
  ##   interval: 60
  mod_privacy:
    db_type: redis
  ## mod_private: {}
  ## mod_proxy65: {}
  ## mod_pubsub:
  ##   access_createnode: pubsub_createnode
  ## ## reduces resource comsumption, but XEP incompliant
  ##  ignore_pep_from_offline: true
  ## ## XEP compliant, but increases resource comsumption
  ##  ignore_pep_from_offline: false
  ##  last_item_cache: false
  ##  plugins:
  ##    - "flat"
  ##    - "hometree"
  ##    - "pep" # pep requires mod_caps
  mod_register:
    access: register
    access_from: register
  mod_roster:
    versioning: true
    store_current_id: true
  ## mod_shared_roster: {}
  ## mod_stats: {}
  mod_easemob_api: {}
  mod_time: {}
  ##mod_vcard: {}
  mod_version: {}
  #mod_easemob_sendmsg:
  #  redis_chat_topic: "im:rest:chat"
  #  redis_groupchat_topic: "im:rest:groupchat"
  #  redis_host: "172.16.0.48"
  #  redis_port: 6379
  mod_statsdx:
    enable: false
    hooks: traffic
  mod_stats2file:
    enable: false
    interval: 60
    type: txt
    split: false
    basefilename: "/opt/ejabberd/var/log/ejabberd/stats"
    hosts:
      - "easemob.com"
  #  mod_roster_cache:
  #  redis_host: "172.16.0.48"
  #  redis_port: 6379
  #  redis_db: 0
  #  pool_size: 4
  mod_privacy_cache:
    redis_host: "172.16.0.48"
    redis_port: 6379
    redis_db: 0
    pool_size: 4
  mod_message_store: {}
  mod_message_cache:
    redis_host: "172.16.0.48"
    redis_port: 6379
    redis_db: 0
    pool_size: 4
  mod_message_index_cache:
    redis_host: "172.16.0.48"
    redis_port: 6379
    redis_db: 0
    pool_size: 4
  #mod_media_ctl:
  #  servers:
  #    - "121.41.106.222:6996"
  #  pool_size: 4
  #  turnservers:
  #    - "121.41.106.222:400"
##  mod_keyword_scan:
##    servers:
##      - "123.57.11.157:8090"
##      - "123.57.11.157:8090"
##    pool_size: 4


##
## Enable modules with custom options in a specific virtual host
##
## host_config:
##   "localhost":
##     add:
##       modules:
##         mod_echo:
##           host: "mirror.localhost"

### Local Variables:
### mode: yaml
### End:
### vim: set filetype=yaml tabstop=8
{% endhighlight %}


## 注册用户

```
$ rel/ejabberd/bin/ejabberdctl register easemob-demo#chatdemoui_wcy001 easemob.com 123
```

## 启动手机客户端

## 抓包

### 登陆过程

```
c->s: <?xml version='1.0'?>
c->s: <stream:stream xmlns='jabber:client' xmlns:stream='http://etherx.jabber.org/streams' version='1.0' to='easemob.com'>


s->c: <?xml version='1.0'?>
      <stream:stream xmlns='jabber:client'
               xmlns:stream='http://etherx.jabber.org/streams'
               id='2799737364'
               from='easemob.com'
               version='1.0'
               xml:lang='en'>
s->c:
   <stream:features>
    <compression xmlns='http://jabber.org/features/compress'>
      <method>zlib</method>
    </compression>
    <mechanisms xmlns='urn:ietf:params:xml:ns:xmpp-sasl'>
      <mechanism>PLAIN</mechanism>
      <mechanism>DIGEST-MD5</mechanism>
      <mechanism>SCRAM-SHA-1</mechanism>
    </mechanisms>
    <c xmlns='http://jabber.org/protocol/caps'
       hash='sha-1' node='http://www.easemob.com/'
       ver='xdKpmWJymL6/AYCPfXBXAYv2tGE='/>
    <register xmlns='http://jabber.org/features/iq-register'/>
  </stream:features>

c->s:  <compress xmlns="http://jabber.org/protocol/compress">
       <method>zlib</method>
       </compress>
s->c: <compressed xmlns='http://jabber.org/protocol/compress'/>

```

从此以后，就是压缩数据了，这里显示的时解压缩之后的数据。


```
c->s: <stream:stream xmlns='jabber:client'
               xmlns:stream='http://etherx.jabber.org/streams'
               version='1.0'
               to='easemob.com'>
s->c:
     <?xml version='1.0'?>
     <stream:stream xmlns='jabber:client'
                    xmlns:stream='http://etherx.jabber.org/streams'
                    id='1759795113' from='easemob.com'
                    version='1.0'
                    xml:lang='en'>

```
