地址：https://github.com/apache/zookeeper





## ch01 脚本文件

### 1.1 （✔）zkEnv.sh

（，目的：用于被其他zk脚本source而设置环境变量）This script should be sourced into other zookeeper scripts to setup the env variables

（，）We use `ZOOCFGDIR` if defined, otherwise we use `/etc/zookeeper` or the conf directory that is **a sibling of this script's directory（同一级目录下）**. Or you can specify the ZOOCFGDIR using the '--config' option in the command line.

---

获取（变量，zk的bin目录）（实际就是脚本文件目录），通过环境变量（无则设置为 `/usr/bin`目录），得到（变量，zk项目目录，`ZOOKEEPER_PREFIX`），bin目录上一次；

> shell默认值表达式：参考 附录A-二
>
> ```sh
> ZOOBINDIR="${ZOOBINDIR:-/usr/bin}"
> ZOOKEEPER_PREFIX="${ZOOBINDIR}/.."
> ```
>
> 

参数处理：check to see if the conf dir is given as an optional argument

- 只有当参数个数 `$#` 大于1个才处理，否则无效参数跳过这个步骤。
- 只有第一个参数是 `--config`才处理。并通过 shift命令去的这个参数
- 获取具体目录赋值到（变量，zk的conf文件目录，`ZOOCFGDIR`），再次shift完全去的。

> 问：为什么需要中间一个变量？
>
> ```
> 	      shift
> 	      confdir=$1
> 	      shift
> 	      ZOOCFGDIR=$confdir
> ```
>
> 自我解答：因为需要2个参数 `--config <conf-dir>` ，需要进行2次shit，这里 confdir只是零散变量而已。

判断（变量，zk的conf文件目录）是否有设置；没有设置的话，通过如下流程设置：

- 如果（（变量，zk项目目录）的conf目录存在。（注：只有2种可能，1、设置了变量的conf目录 2、`/usr/conf`目录 （不存在））
  - 则（变量，zk的conf文件目录）设置为这个。
- 否则：设置为 `/usr/etc/zookeeper` 目录

> bash技巧：防止无变量时的语法报错；
>
> - 目的： a common technique in Bash to avoid potential issues when the variable is empty or unset. If you just use `[ "$ZOOCFGDIR" = "" ]`, and if `$ZOOCFGDIR` is unset or empty, the test would become `[ = "" ]`, which is a **syntax error**.
>
> ```sh
> if [ "x$ZOOCFGDIR" = "x" ]
> ```
>
> 

---

如果conf目录下有 `zookeeper-env.sh`文件，source引入

如果（变量，zk配置文件名，`ZOOCFG`）未设置，则设为 `zoo.cfg` 默认名

【🤢🤢🤢】

---

设置（变量，JAVA，java bin路径）：

- 如果 设置了（变量，JAVA_HOME）,并且 `$JAVA_HOME/bin/java`是可执行文件：
  - （java bin路径）设置为这个
- 否则检查 java是否在PATH变量里，
  - （java bin路径）设置为这个
- 否则：没有java存在，通过 `1>&2`打印错误，并且脚本退出；

> bash技巧：
>
> ·1、The ‘-z’ test operator returns true if the length of the string is zero. In other words, it checks if a variable is unset or set to an empty string. On the other hand, the ‘-n’ operator returns true if the string’s length is non-zero, meaning it checks if a variable is set to some string.
>
> 2、 -x判断文件是否可执行
>
>  3、checks whether the `java` executable is available in the system's PATH using the `type -p` command. 
>
> 4、`1>&2` is a redirection in Bash that means "redirect standard output (file descriptor 1) to the same location as standard error (file descriptor 2)."



---

修改 `CLASSPATH` 环境变量：

1、add the zoocfg dir to classpath

2、遍历zk项目的zk server里的lib目录的jar文件，加入到 （类路径）的前部

```sh
for i in "$ZOOBINDIR"/../zookeeper-server/src/main/resources/lib/*.jar
do
    CLASSPATH="$i:$CLASSPATH"
done
```

---

各种开发者相关的目录下的jar文件，class文件加入到（类路径里）：make it work for developers

【🤢🤢🤢】

---

cygwin或mingwin环境（uname判断）：转化类路径

```sh
case "`uname`" in
    CYGWIN*|MINGW*) cygwin=true ;;
    *) cygwin=false ;;
esac
```

> cygpath -wp工具：
>
> Convert Unix and Windows format paths, or output system path information

---

default heap for zookeeper server（1000M），可以通过环境变量覆盖和JVM标志变量

default heap for zookeeper client（256M）

```sh
ZK_SERVER_HEAP="${ZK_SERVER_HEAP:-1000}"
export SERVER_JVMFLAGS="-Xmx${ZK_SERVER_HEAP}m $SERVER_JVMFLAGS"

ZK_CLIENT_HEAP="${ZK_CLIENT_HEAP:-256}"
export CLIENT_JVMFLAGS="-Xmx${ZK_CLIENT_HEAP}m $CLIENT_JVMFLAGS"
```

### 1.2 zkServer.sh

src：

- 先获取这个脚本的相对路径名。（参考：附录A-三、四）文章。
  - 默认值目的：BASH_SOURCE是新版本加入的，如果不支持则用 $0获取。
- 通过dirname获取当前这个相对路径，所对应的目录（相对）
- 切换自身目录（脚本目录），再获取绝对路径；（目的：为了source 这个目录的脚本）
- source 【1.1节 zkEnv.sh】，包含环境信息，java的bin，参数等等。。。

```sh
ZOOBIN="${BASH_SOURCE-$0}"
ZOOBIN="$(dirname "${ZOOBIN}")"
ZOOBINDIR="$(cd "${ZOOBIN}"; pwd)"

if [ -e "$ZOOBIN/../libexec/zkEnv.sh" ]; then
  . "$ZOOBINDIR"/../libexec/zkEnv.sh
else
  . "$ZOOBINDIR"/zkEnv.sh
fi
```

> 脚本例子：
>
> ```sh
> Administrator@PC-20221207UZLY MINGW64 /d/myProject/Demo_C_C++ (master)
> $ bash shell/test.sh
> shell/test.sh
> shell/test.sh
> 
> Administrator@PC-20221207UZLY MINGW64 /d/myProject/Demo_C_C++ (master)
> $ dirname shell/test.sh 
> shell
> ```
>
> 

【🤢🤢🤢】

src-zk-server的启动类：

```
ZOOMAIN=xxx---> org.apache.zookeeper.server.quorum.QuorumPeerMain
```



src-end：可以支持1个可选参数（前面环境脚本会检查，并shift退出）

- 最终的`$1` ，就是zk具体执行的动作。
- start：后台的方式启动zk（nohup）
- start-foreground：前台的方式启动。
- 。。。
- 动作不匹配：打印帮助信息。

```sh
case $1 in
start)
  。。。
  ;;
stop)
  ;;
*)
    echo "Usage: $0 [--config <conf-dir>] {start|start-foreground|stop|version|restart|status|print-cmd}" >&2

esac
```



### 1.3 （✔）zkCli.sh

前置部分同（1.2节）

- zk客户端类：`org.apache.zookeeper.ZooKeeperMain`

```sh
ZOO_LOG_FILE=zookeeper-$USER-cli-$HOSTNAME.log

"$JAVA" "-Dzookeeper.log.dir=${ZOO_LOG_DIR}" "-Dzookeeper.log.file=${ZOO_LOG_FILE}" \
     -cp "$CLASSPATH" $CLIENT_JVMFLAGS $JVMFLAGS \
     org.apache.zookeeper.ZooKeeperMain "$@"
```



# 附录A：《各种各样》

## 一、《ZooKeeper Wiki》

（，是一个中心化服务用于维护配置信息，命名，提供分布式同步，并提供分组服务；）ZooKeeper is a centralized service for maintaining configuration information, naming, providing **distributed synchronization**, and providing group services. All of these kinds of services are used in some form or another by distributed applications. Each time they are implemented there is a lot of work that goes into fixing the bugs and race conditions that are inevitable. Because of the difficulty of implementing these kinds of services, applications initially usually skimp on them ,which make them brittle in the presence of change and difficult to manage. Even when done correctly, different implementations of these services lead to management complexity when the applications are deployed.

ZooKeeper aims at distilling the essence of these different services into a very simple interface to a centralized coordination service. The service itself is distributed and highly reliable. Consensus, group management, and presence protocols will be implemented by the service so that the applications do not need to implement them on their own. Application specific uses of these will consist of a mixture of specific components of Zoo Keeper and application specific conventions. [ZooKeeper Recipes](http://hadoop.apache.org/zookeeper/docs/current/recipes.html) shows how this simple service can be used to build much more powerful abstractions.

We have Java and C interfaces to Zoo Keeper for the applications themselves. A variety of client bindings is available for a number of languages including Python, Ruby and Go.

### 1.1 ZooKeeper概览

（，zk允许分布式进程通过一个共享层级名空间的数据注册（称为znodes）而互相协作；）ZooKeeper allows distributed processes to coordinate with each other through a shared hierarchical name space of data registers (we call these registers znodes), much like a file system. Unlike normal file systems ZooKeeper provides its clients with high throughput, low latency, highly available, strictly ordered access to the znodes. The performance aspects of ZooKeeper allow it to be used in large distributed systems. The reliability aspects prevent it from becoming the single point of failure in big systems. Its strict ordering allows sophisticated synchronization primitives to be implemented at the client.

The name space provided by ZooKeeper is much like that of a standard file system. A name is a sequence of path elements separated by a slash ("/"). Every znode in ZooKeeper's name space is identified by a path. And every znode has a parent whose path is a prefix of the znode with one less element; the exception to this rule is root ("/") which has no parent. Also, exactly like standard file systems, a znode cannot be deleted if it has any children.

（， 。。。zk是被设计用来存放**协作数据**：状态信息，配置，分布信息等；）The main differences between ZooKeeper and standard file systems are that every znode can have data associated with it (every file can also be a directory and vice-versa) and znodes are limited to the amount of data that they can have. ZooKeeper was designed to store **coordination data**: status information, configuration, location information, etc. This kind of meta-information is usually measured in kilobytes, if not bytes. <u>ZooKeeper has a built-in sanity check of 1M</u>, to prevent it from being used as a large data store, but in general it is used to store much smaller pieces of data.

![img](https://cwiki.apache.org/confluence/download/attachments/24193436/service.png?version=1&modificationDate=1295027310000&api=v2)

The service itself is replicated over a set of machines that comprise the service. These machines maintain an in-memory image of the data tree along with a transaction logs and snapshots in a persistent store. Because the data is kept in-memory, ZooKeeper is able to get very high throughput and low latency numbers. The downside to an in-memory database is that the size of the database that ZooKeeper can manage is limited by memory. This limitation is further reason to keep the amount of data stored in znodes small.

The servers that make up the ZooKeeper service must all know about each other. As long as a majority of the servers are available, the ZooKeeper service will be available. Clients must also know the list of servers. The clients create a handle to the ZooKeeper service using this list of servers.

Clients only connect to a single ZooKeeper server. The client maintains a TCP connection through which it sends requests, gets responses, gets watch events, and sends heartbeats. If the TCP connection to the server breaks, the client will connect to a different server. When a client first connects to the ZooKeeper service, the first ZooKeeper server will setup a session for the client. If the client needs to connect to another server, this session will get reestablished with the new server.

Read requests sent by a ZooKeeper client are processed locally at the ZooKeeper server to which the client is connected. If the read request registers a watch on a znode, that watch is also tracked locally at the ZooKeeper server. Write requests are forwarded to other ZooKeeper servers and go through consensus before a response is generated. Sync requests are also forwarded to another server, but do not actually go through consensus. Thus, the throughput of read requests scales with the number of servers and the throughput of write requests decreases with the number of servers.

Order is very important to ZooKeeper; almost bordering on obsessive–compulsive disorder. All updates are totally ordered. ZooKeeper actually stamps each update with a number that reflects this order. We call this number the zxid (ZooKeeper Transaction Id). Each update will have a unique zxid. Reads (and watches) are ordered with respect to updates. Read responses will be stamped with the last zxid processed by the server that services the read.

### 1.2 教程

**Tutorial**

**1、Programming with ZooKeeper - A quick tutorial**

In this tutorial, we show simple implementations of barriers and producer-consumer queues using !ZooKeeper. We call the respective classes Barrier and Queue. These examples assume that you have at least one !ZooKeeper server running.

Both primitives use the following common excerpt of code:

```java
static ZooKeeper zk = null;
static final Object mutex = new Object();
 
String root;
 
SyncPrimitive(String address)
throws KeeperException, IOException {
    if(zk == null){
            System.out.println("Starting ZK:");
            zk = new ZooKeeper(address, 3000, this);
            System.out.println("Finished starting ZK: " + zk);
    }
}
 
public void process(WatcherEvent event) {
    synchronized (mutex) {
        mutex.notify();
    }
}
```

Both classes extend !SyncPrimitive. In this way, we execute steps that are common to all primitives in the constructor of !SyncPrimitive. To keep the examples simple, we create a !ZooKeeper object the first time we instantiate either a barrier object or a queue object, and we declare a static variable that is a reference to this object. The subsequent instances of Barrier and Queue check whether a !ZooKeeper object exists. Alternatively, we could have the application creating a !ZooKeeper object and passing it to the constructor of Barrier and Queue.

We use the process() method to process notifications triggered due to watches. In the following discussion, we present code that sets watches. A watch is internal structure that enables !ZooKeeper to notify a client of a change to a node. For example, if a client is waiting for other clients to leave a barrier, then it can set a watch and wait for modifications to a particular node, which can indicate that it is the end of the wait. This point becomes clear once we go over the examples.

**2、Barriers**

A barrier is a primitive that enables a group of processes to synchronize the beginning and the end of a computation. The general idea of this implementation is to have a barrier node that serves the purpose of being a parent for individual process nodes. Suppose that we call the barrier node "/b1". Each process "p" then creates a node "/b1/p". Once enough processes have created their corresponding nodes, joined processes can start the computation.

In this example, each process instantiates a Barrier object, and its constructor takes as parameters:

- - - - The address of a !ZooKeeper server (e.g., "zoo1.foo.com:2181");
      - The path of the barrier node on !ZooKeeper (e.g., "/b1");
      - The size of the group of processes.

The constructor of Barrier passes the address of the Zookeeper server to the constructor of the parent class. The parent class creates a !ZooKeeper instance if one does not exist. The constructor of Barrier then creates a barrier node on !ZooKeeper, which is the parent node of all process nodes, and we call root (obs: this is not the !ZooKeeper root "/").

```java
      /**
******* Barrier constructor
*******
******* @param address
******* @param name
******* @param size
*******/
        Barrier(String address, String name, int size)
        throws KeeperException, InterruptedException, UnknownHostException {
            super(address);
            this.root = name;
            this.size = size;
 
            // Create barrier node
            if (zk != null) {
                    Stat s = zk.exists(root, false);
                    if (s == null) {
                        zk.create(root, new byte[0], Ids.OPEN_ACL_UNSAFE, 0);
                    }
            }
 
            // My node name
            name = new String(InetAddress.getLocalHost().getCanonicalHostName().toString());
        }
```

To enter the barrier, a process calls enter(). The process creates a node under the root to represent it, using its host name to form the node name. It then wait until enough processes have entered the barrier. A process does it by checking the number of children the root node has with "getChildren()", and waiting for notifications in the case it does not have enough. To receive a notification when there is a change to the root node, a process has to set a watch, and does it through the call to "getChildren()". In the code, we have that "getChildren()" has two parameters. The first one states the node to read from, and the second is a boolean flag that enables the process to set a watch. In the code the flag is true.

```java
    /**
******* Join barrier
*******
******* @return
******* @throws KeeperException
******* @throws InterruptedException
*******/
 
        boolean enter() throws KeeperException, InterruptedException{
            zk.create(root + "/" + name, new byte[0], Ids.OPEN_ACL_UNSAFE,
                    CreateFlags.EPHEMERAL);
            while (true) {
                synchronized (mutex) {
                    ArrayList<String> list = zk.getChildren(root, true);
 
                    if (list.size() < size) {
                        mutex.wait();
                    } else {
                        return true;
                    }
                }
            }
        }
```

Note that enter() throws both !KeeperException and !InterruptedException, so it is reponsibility of the application to catch and handle such exceptions.

Once the computation is finished, a process calls leave() to leave the barrier. First it deletes its corresponding node, and then it gets the children of the root node. If there is at least one child, then it waits for a notification (obs: note that the second parameter of the call to getChildren() is true, meaning that !ZooKeeper has to set a watch on the the root node). Upon reception of a notification, it checks once more whether the root node has any child.

```java
     /**
******* Wait until all reach barrier
*******
******* @return
******* @throws KeeperException
******* @throws InterruptedException
*******/
 
        boolean leave() throws KeeperException, InterruptedException{
            zk.delete(root + "/" + name, 0);
            while (true) {
                synchronized (mutex) {
                    ArrayList<String> list = zk.getChildren(root, true);
                        if (list.size() > 0) {
                            mutex.wait();
                        } else {
                            return true;
                        }
                    }
                }
        }
```

**3、Producer-Consumer Queues**

A producer-consumer queue is a distributed data structure that group of processes use to generate and consume items. Producer processes create new elements and add them to the queue. Consumer processes remove elements from the list, and process them. In this implementation, the elements are simple integers. The queue is represented by a root node, and to add an element to the queue, a producer process creates a new node, a child of the root node.

The following excerpt of code corresponds to the constructor of the object. As with Barrier objects, it first calls the constructor of the parent class, !SyncPrimitive, that creates a !ZooKeeper object if one doesn't exist. It then verifies if the root node of the queue exists, and creates if it doesn't.

```java
     /**
******* Constructor of producer-consumer queue
*******
******* @param address
******* @param name
*******/
        Queue(String address, String name)
        throws KeeperException, InterruptedException {
            super(address);
            this.root = name;
            // Create ZK node name
            if (zk != null) {
                    Stat s = zk.exists(root, false);
                    if (s == null) {
                        zk.create(root, new byte[0], Ids.OPEN_ACL_UNSAFE, 0);
                    }
            }
        }
```

A producer process calls "produce()" to add an element to the queue, and passes an integer as an argument. To add an element to the queue, the method creates a new node using "create()", and uses the SEQUENCE flag to instruct !ZooKeeper to append the value of the sequencer counter associated to the root node. In this way, we impose a total order on the elements of the queue, thus guaranteeing that the oldest element of the queue is the next one consumed.

```java
    /**
******* Add element to the queue.
*******
******* @param i
******* @return
*******/
 
        boolean produce(int i) throws KeeperException, InterruptedException{
            ByteBuffer b = ByteBuffer.allocate(4);
            byte[] value;
 
            // Add child with value i
            b.putInt(i);
            value = b.array();
            zk.create(root + "/element", value, Ids.OPEN_ACL_UNSAFE,
                        CreateFlags.SEQUENCE);
 
            return true;
        }
```

To consume an element, a consumer process obtains the children of the root node, reads the node with smallest counter value, and returns the element. Note that if there is a conflict, then one of the two contending processes won't be able to delete the node and the delete operation will throw an exception.

A call to getChildren() returns the list of children in lexicographic order. As lexicographic order does not necessary follow the numerical order of the counter values, we need to decide which element is the smallest. To decide which one has the smallest counter value, we traverse the list, and remove the prefix "element" from each one.

```java
     /**
******* Remove first element from the queue.
*******
******* @return
******* @throws KeeperException
******* @throws InterruptedException
*******/
        int consume() throws KeeperException, InterruptedException{
            int retvalue = -1;
            Stat stat = null;
 
            // Get the first element available
            while (true) {
                synchronized (mutex) {
                    ArrayList<String> list = zk.getChildren(root, true);
                    if (list.isEmpty()) {
                        System.out.println("Going to wait");
                        mutex.wait();
                    } else {
                        Integer min = new Integer(list.get(0).substring(7));
                        for(String s : list){
                            Integer tempValue = new Integer(s.substring(7));
                            if(tempValue < min) min = tempValue;
                        }
                        System.out.println("Temporary value: " + root + "/element" + min);
                        byte[] b = zk.getData(root + "/element" + min, false, stat);
                        zk.delete(root + "/element" + min, 0);
                        ByteBuffer buffer = ByteBuffer.wrap(b);
                        retvalue = buffer.getInt();
 
                        return retvalue;
                    }
                }
            }
        }
```

### 1.3 FAQ

#### 1、zk的状态转移是什么？

**What are the state transitions of ZooKeeper?**

```
                  AUTH request failed, AUTH_FAILED
                  event, all pending requests return
                  AUTH_FAILED

               +-------------------+    +--------+
               |                   |    | start  |
               |                   |    |        |
               |                   |    +--+-----+        Successful Connection.
               v                   |       |              triggers CONNECTED event
+--------------+----------+        |       |
|                         |       ++-------v---------------+                    +----------------+
| AUTH_FAILED             |       |   CONNECTING_STATE     | +--------------->  |CONNECT_STATE   |
| any requests will return|       |   requests queued.     |                    |                |
| with AUTH_FAILED        |       |   if connection attempt|                    |requests sent   |
|                         |       |   fails, any queued    |                    |to the server   |
+---+---------------------+       |   requests fail with   | <---------------+  |                |
    |                             +---+------+-------------+                    |                |
    |                                 |      |         Connection to server lost+--------+-------+
    |                                 |      |         DISCONNECTED event, pending       |
    |                                 |      |         requests retum CONNECTION_LOSS    |
    |                Session expires, |      |                                           | called close,
    |                SESSION_EXPIRED  |      | called close.                             | pending
    |                event, pending   |      | pending                                   | requests
    |                requests return  |      | requests                                  | retum
    |                SESSION_EXPIRED  |      | retum                                     | CONNECTION_LOSS
    |                                 v      v CONNECTION_LOSS                           v
    v                            +----+------+-------------------------------------------+-------+
+---+----+                       |                     CLOSED_STATE                              |
|        |                       |                     any requests will return                  |
|  End   | <---------------------+                     with SESSION_EXPIRED                      |
|        |                       |                                                               |
+--------+                       +---------------------------------------------------------------+

```

#### 2、如何处理连接丢弃错误？ 

**How should I handle the CONNECTION_LOSS error?**

（，意味着在客户端与服务端之间的链接是坏的；这不一定意味着请求失败。如果你做一个创建请求，1、并且链接破坏的时期：在请求到达服务器之后，在响应返回之前，则创建请求将会**成功**；2、如果链接在包进入到线缆之前，则请求**失败**；）CONNECTION_LOSS means the link between the client and server was broken. It doesn't necessarily mean that the request failed. If you are doing a create request and the link was broken after the request reached the server and before the response was returned, the create request will succeed. If the link was broken before the packet went onto the wire, the create request failed. Unfortunately, there is no way for the client library to know, so it returns CONNECTION_LOSS. The programmer must figure out if the request succeeded or needs to be retried. Usually this is done in an application specific way. Examples of success detection include checking for the presence of a file to be created or checking the value of a znode to be modified.

When a client (session) becomes partitioned from the ZK serving cluster it will begin searching the list of servers that were specified during session creation. Eventually, when connectivity between the client and at least one of the servers is re-established, the session will either again transition to the "connected" state (if reconnected within the session timeout value) or it will transition to the "expired" state (if reconnected after the session timeout). The ZK client library will handle reconnect for you automatically. In particular we have heuristics built into the client library to handle things like "herd effect", etc... Only create a new session when you are notified of session expiration (mandatory).

#### How should I handle SESSION_EXPIRED?

SESSION_EXPIRED automatically closes the ZooKeeper handle. In a correctly operating cluster, you should never see SESSION_EXPIRED. It means that the client was partitioned off from the ZooKeeper service for more the the session timeout and ZooKeeper decided that the client died. Because the ZooKeeper service is ground truth, the client should consider itself dead and go into recovery. If the client is only reading state from ZooKeeper, recovery means just reconnecting. In more complex applications, recovery means recreating ephemeral nodes, vying for leadership roles, and reconstructing published state.

Library writers should be conscious of the severity of the expired state and not try to recover from it. Instead libraries should return a fatal error. Even if the library is simply reading from ZooKeeper, the user of the library may also be doing other things with ZooKeeper that requires more complex recovery.

Session expiration is managed by the ZooKeeper cluster itself, not by the client. When the ZK client establishes a session with the cluster it provides a "timeout" value. This value is used by the cluster to determine when the client's session expires. Expirations happens when the cluster does not hear from the client within the specified session timeout period (i.e. no heartbeat). At session expiration the cluster will delete any/all ephemeral nodes owned by that session and immediately notify any/all connected clients of the change (anyone watching those znodes). At this point the client of the expired session is still disconnected from the cluster, it will not be notified of the session expiration until/unless it is able to re-establish a connection to the cluster. The client will stay in disconnected state until the TCP connection is re-established with the cluster, at which point the watcher of the expired session will receive the "session expired" notification.

Example state transitions for an expired session as seen by the expired session's watcher:

1. 'connected' : session is established and client is communicating with cluster (client/server communication is operating properly)
2. .... client is partitioned from the cluster
3. 'disconnected' : client has lost connectivity with the cluster
4. .... time elapses, after 'timeout' period the cluster expires the session, nothing is seen by client as it is disconnected from cluster
5. .... time elapses, the client regains network level connectivity with the cluster
6. 'expired' : eventually the client reconnects to the cluster, it is then notified of the expiration

#### Is there an easy way to expire a session for testing?

Yes, a ZooKeeper handle can take a session id and password. This constructor is used to recover a session after total application failure. For example, an application can connect to ZooKeeper, save the session id and password to a file, terminate, restart, read the session id and password, and reconnect to ZooKeeper without loosing the session and the corresponding ephemeral nodes. It is up to the programmer to ensure that the session id and password isn't passed around to multiple instances of an application, otherwise problems can result.

In the case of testing we want to cause a problem, so to explicitly expire a session an application connects to ZooKeeper, saves the session id and password, creates another ZooKeeper handle with that id and password, and then closes the new handle. Since both handles reference the same session, the close on second handle will invalidate the session causing a SESSION_EXPIRED on the first handle.

#### Why doesn't the NodeChildrenChanged and NodeDataChanged watch events return more information about the change?

When a ZooKeeper server generates the change events, it knows exactly what the change is. In our initial implementation of ZooKeeper we returned this information with the change event, but it turned out that it was impossible to use correctly. There may be a correct way to use it, but we have never seen a case of correct usage. The problem is that watches are used to find out about the latest change. (Otherwise, you would just do periodic gets.) The thing that most programmers seem to miss, when they ask for this feature, is that watches are one time triggers. Observe the following case of data change: a process does a getData on "/a" with watch set to true and gets "v1", another process changes "/a" to "v2" and shortly there after changes "/a" to "v3". The first process would see that "/a" was changed to "v2", but wouldn't know that "/a" is now "/v3".

#### What are the options-process for upgrading ZooKeeper?

There are two primary ways of doing this; 1) full restart or 2) rolling restart.

In the full restart case you can stage your updated code/configuration/etc..., stop all of the servers in the ensemble, switch code/configuration, and restart the ZooKeeper ensemble. If you do this programmatically (scripts typically, ie not by hand) the restart can be done on order of seconds. As a result the clients will lose connectivity to the ZooKeeper cluster during this time, however it looks to the clients just like a network partition. All existing client sessions are maintained and re-established as soon as the ZooKeeper ensemble comes back up. Obviously one drawback to this approach is that if you encounter any issues (it's always a good idea to test/stage these changes on a test harness) the cluster may be down for longer than expected.

The second option, preferable for many users, is to do a "rolling restart". In this case you upgrade one server in the ZooKeeper ensemble at a time; bring down the server, upgrade the code/configuration/etc..., then restart the server. The server will automatically rejoin the quorum, update it's internal state with the current ZK leader, and begin serving client sessions. As a result of doing a rolling restart, rather than a full restart, the administrator can monitor the ensemble as the upgrade progresses, perhaps rolling back if any issues are encountered.

#### How do I size a ZooKeeper ensemble (cluster)?

In general when determining the number of ZooKeeper serving nodes to deploy (the size of an ensemble) you need to think in terms of reliability, and not performance.

Reliability:

A single ZooKeeper server (standalone) is essentially a coordinator with no reliability (a single serving node failure brings down the ZK service).

A 3 server ensemble (you need to jump to 3 and not 2 because ZK works based on simple majority voting) allows for a single server to fail and the service will still be available.

So if you want reliability go with at least 3. We typically recommend having 5 servers in "online" production serving environments. This allows you to take 1 server out of service (say planned maintenance) and still be able to sustain an unexpected outage of one of the remaining servers w/o interruption of the service.

Performance:

Write performance actually *decreases* as you add ZK servers, while read performance increases modestly: http://zookeeper.apache.org/doc/current/zookeeperOver.html#Performance

See [this page](http://bit.ly/4ekN8G) for a survey Patrick Hunt (http://twitter.com/phunt) did looking at operational latency with both standalone server and an ensemble of size 3. You'll notice that a single core machine running a standalone ZK ensemble (1 server) is still able to process 15k requests per second. This is orders of magnitude greater than what most applications require (if they are using ZooKeeper correctly - ie as a coordination service, and not as a replacement for a database, filestore, cache, etc...)

#### Can I run an ensemble cluster behind a load balancer?

There are two types of servers failures in distributed system from socket I/O perspective.

1. server down due to hardware failures and OS panic/hang, Zookeeper daemon hang, temporary/permanent network outage, network switch anomaly, etc: client cannot figure out failures immediately since there is no responding entities. As a result, zookeeper clients must rely on timeout to identify failures.
2. Dead zookeeper process (daemon): since OS will respond to closed TCP port, client will get "connection refused" upon socket connect or "peer reset" on socket I/O. Client immediately notice that the other end failed.

Here's how ZK clients respond to servers in each case.

1. In this case (former), ZK client rely on heartbeat algorithm. ZK clients detects server failures in 2/3 of recv timeout (Zookeeper_init), and then it retries the same IP at every recv timeout period if only one of ensemble is given. If more than two ensemble IP are given, ZK clients will try next IP immediately.
2. In this scenario, ZK client will immediately detect failure, and will retry connecting every second assuming only one ensemble IP is given. If multiple ensemble IP is given (most installation falls into this category), ZK client retries next IP immediately.

Notice that in both cases, when more than one ensemble IP is specified, ZK clients retry next IP immediately with no delay.

On some installations, it is preferable to run an ensemble cluster behind a load balancer such as hardware L4 switch, TCP reverse proxy, or DNS round-robin because such setup allows users to simply use one hostname or IP (or VIP) for ensemble cluster, and some detects server failures as well.

But there are subtle differences on how these load balancers will react upon server failures.

- Hardware L4 load balancer: this setup involves one IP and a hostname. L4 switch usually does heartbeat on its own, and thus removes non-responding host from its IP list. But this also relies on the same timeout scheme for fault detection. L4 may redirect you to a unresponsive server. If hardware LB detect server failures fast enough, this setup will always redirect you to live ensemble server.
- DNS round robin: this setup involves one hostname and a list of IPs. ZK clients correctly make used of a list of IPs returned by DNS query. Thus this setup works the same way as multiple hostname (IP) argument to zookeeper_init. The drawback is that when an ensemble cluster configuration changes like server addition/removal, it may take a while to propagate the DNS entry change in all DNS servers and DNS client caching (nscd for example) TTL issue.

In conclusion, DNS RR works as good as a list of ensemble IP arguments except cluster reconfiguration case.
It turns out that there is a minor problem with DNS RR. If you are using a tool such as zktop.py, it does not take care of a list of host IP returned by a DNS server.

#### What happens to ZK sessions while the cluster is down?

Imagine that a client is connected to ZK with a 5 second session timeout, and the administrator brings the entire ZK cluster down for an upgrade. The cluster is down for several minutes, and then is restarted.

In this scenario, the client is able to reconnect and refresh its session. Because session timeouts are tracked by the leader, the session starts counting down again with a fresh timeout when the cluster is restarted. So, as long as the client connects within the first 5 seconds after a leader is elected, it will reconnect without an expiration, and any ephemeral nodes it had prior to the downtime will be maintained.

The same behavior is exhibited when the leader crashes and a new one is elected. In the limit, if the leader is flip-flopping back and forth quickly, sessions will never expire since their timers are getting constantly reset.

#### Why is the myid file required?

The [myid file](https://zookeeper.apache.org/doc/current/zookeeperAdmin.html#sc_zkMulitServerSetup) contains an integer in human-readable ASCII text that represents a unique server id. When a ZooKeeper server starts, it reads its id from the myid file and then uses the id to find the port numbers, which it should listen to, from the configuration file. We do not recommend using the IP addresses of ZooKeeper servers to become the unique server ids as IP addresses can be moved from one server to another.

### 1.5 有用的工具

#### Useful User Contributed Tools

- [zkconf](http://bit.ly/mBEcF) - generate configuration for a ZooKeeper ensemble
- [zk-smoketest](http://bit.ly/2Cs6Ee) - smoketest or latencytest a ZooKeeper ensemble (uses zkpython)
- [zookeeper_dashboard](http://bit.ly/1kjQy0) - web dashboard for ZooKeeper ensemble (uses zkpython & django)
- [zktop](http://bit.ly/1iMZdg) - monitor ZooKeeper in realtime
- [zkexamples](http://bit.ly/43Emx3) - phunt's "random examples of useful bits of ZooKeeper ephemera"
- [SPM for ZooKeeper](http://sematext.com/spm) - Performance Monitoring and Alerting for ZooKeeper

#### Wrappers around the ZooKeeper Client API

- curator - Components:
  - Client - A replacement for the bundled ZooKeeper class that takes care of
    some low-level housekeeping and provides some useful utilities
  - Framework - The Curator Framework is a high-level API that greatly
    simplifies using ZooKeeper. It adds many features that build on ZooKeeper
    and handles the complexity of managing connections to the ZooKeeper
    cluster and retrying operations.
  - Recipes - Implementations of some of the common ZooKeeper "recipes". The
    implementations are built on top of the Curator Framework
  - Utilities - Various utilities that are useful when using ZooKeeper.
- [Cages](http://code.google.com/p/cages) - distributed synchronization library
- [ZKClient](http://github.com/sgroschupf/zkclient)
- [KeptCollections](https://github.com/anthonyu/KeptCollections)





## 二、《bash中参数表达式带默认值》

**How to Assign Default Value to a Variable Using Bash**

- https://www.baeldung.com/linux/bash-variable-assign-default

### 1. Overview

When dealing with variables in [shell](https://www.baeldung.com/linux/sh-vs-bash#what-is-a-shell) [scripting](https://www.baeldung.com/linux/linux-scripting-series), it’s important to account for the fact that a variable may have one of several states:

- assigned value
- [unset](https://www.baeldung.com/linux/delete-shell-env-variable#using-the-unset-command)
- [null string](https://www.baeldung.com/linux/zero-null-command-line)

Thus, to avoid unexpected errors, we can use the *${param:-param}* expression along with similar expressions to assign a default value to a variable under specific conditions.

In this tutorial, we’ll explore how to assign a default value to a variable using *${param:-param}* and similar expressions. To test our methods, we’ll use the *[echo](https://www.baeldung.com/linux/echo-command)* command along with basic shell variable assignments.

### 2. Parameter Expansion of *${param:-param}*

**If a variable, x, is unset or is the [null](https://www.baeldung.com/linux/zero-null-command-line#zero-and-the-null-character) string, then `${x:-default_value}` expands to the default_value string**. Importantly, no assignment to the *x* variable takes place, and the variable retains its original value.

#### 2.1. Non-null

To test the expression, let’s start with the case when *x* takes on a non-null value:

```bash
$ x=value
$ echo "${x:-default_value}"
value
```

Since *x* is set and isn’t the null string, the [parameter expansion](https://www.baeldung.com/linux/parameter-expansion-vs-command-substitution#brace-parameter-expansion) of *${x:-default_value}* returns the value of *x*.

#### 2.2. Null and Unset

Next, let’s consider the case when we set *x* to the null string:

```bash
$ x=''
$ echo "${x:-default_value}"
default_value
```

In this case, the expression expands to the *default_value* string shown on the right-hand side of the *:-* syntax.

Finally, let’s consider the case when *x* is unset:

```bash
$ unset x
$ echo "${x:-default_value}"
default_value
```

Here, the *${x:-default_value}* expression expands to the *default_value* string since *x* is unset. The *x* variable remains unset and no assignment takes place.

#### 2.3. Assign to Variable

Notably, however, we can assign the result of the entire expression to another variable:

```bash
$ y="${x:-default_value}"
$ echo "$y"
default_value
```

The *y* variable is assigned the result of the *${x:-default_value}* expression which, in this case, expands to the *default_value* string since *x* is unset.

#### 2.4. Variable Values

Moreover, the right-hand side of the *${param:-param}* expression can represent the value of some other variable, say *z*:

```bash
$ z=0
$ echo "${x:-$z}"
0
```

Here, the expression expands to the value of the *z* variable since *x* is unset.

#### 2.5. Substring Expansion Modifications

Notably, the *${param:-param}* syntax differs from that of [substring expansion](https://www.baeldung.com/linux/bash-substring#3-using-bashs-substring-expansion):

```bash
$ x='string'
$ echo "${x: -4}"
ring
$ echo "${x:(-4)}"
ring
```

In this case, we extract a substring from the *x* variable starting at index *4*, counting from the end. **To distinguish the negative indexing used in variable subsetting from the \*${param:-param}\* expression, we either introduce a space between the colon and the negative sign, or place the negative index within parentheses**.

### 3. Parameter Expansion of *${param-param}*

**If a variable, \*x\*, is unset, then \*${x-default_value}\* expands to the \*default_value\* string**. Importantly, no assignment to the *x* variable takes place, and the variable retains its original value.

The difference between this scenario and the one from earlier is that when *x* is set to the null string, the expression doesn’t expand to the *default_value* string.

#### 3.1. Non-null and Null

Let’s test the expression beginning with the case when *x* assumes a non-null value:

```bash
$ x=value
$ echo "${x-default_value}"
value
```

We see that the expression expands to the value of the *x* variable since the variable is set.

If *x* is the null string, the *${x-default_value}* expression also expands to the value of *x*:

```bash
$ x=''
$ echo "${x-default_value}"

$
```

Now, let’s see what happens in the third case.

#### 3.2. Unset

However, when *x* is unset, the expression expands to the *default_value* string provided in the right-hand side:

```bash
$ unset x
$ echo "${x-default_value}"
default_value
```

Notably, no assignment occurs, and the *x* variable remains unset. Of course, we can also substitute the constant string with a variable.

### 4. Parameter Expansion of *${param:=param}*

**The expansion of  `${param:=param} ` is very similar to that of `${param:-param}` with one difference: variable assignment occurs in this case, specifically when the variable is unset or is the null string**.

#### 4.1. Non-null

Let’s test the expression when *x* takes on a non-null value:

```bash
$ x=value
$ echo "${x:=default_value}"
value
```

The expression expands to the value of *x* as expected. No variable assignment takes place in this case, and *x* retains its original value.

#### 4.2. Null and Unset

However, when *x* is the null string, the *${x:=default_value}* expression assigns the *default_value* string to *x*, and the expression expands to the new value:

```bash
$ x=''
$ echo "${x:=default_value}"
default_value
$ echo "$x"
default_value
```

We see that both, the *x* variable and the expression, expand to the *default_value* string.

Likewise, if *x* is unset, we obtain the same result as with the null string case. Therefore, the *default_value* string assigns to *x*, and the expression expands to the new value of *x*:

```bash
$ unset x
$ echo "${x:=default_value}"
default_value
$ echo "$x"
default_value
```

We see that **the expression triggers variable assignment in this case**, and both *x* and the *${x:=default_value}* expression expand to the *default_value* string.

### 5. Parameter Expansion of *${param=param}*

**The expansion of the \*${param=param}\* expression is very similar to that of \*${param-param}\* with one difference: variable assignment occurs in this case, specifically when the variable is unset**.

#### 5.1. Non-null and Null

If we set *x* to a non-null value, the *${x=default_value}* expression expands to the value of *x*, and no new assignment takes place:

```bash
$ x=value
$ echo "${x=default_value}"
value
```

Similarly, if we set *x* to the null string, the *${x=default_value}* expression expands to the value of *x*, which is the null string, and no assignment takes place:

```bash
$ x=''
$ echo "${x=default_value}"

$
```

Now, let’s see what happens when *x* isn’t set.

#### 5.2. Unset

Finally, if we unset *x*, then *${x=default_value}* assigns the *default_value* string to *x*, and the expression expands to the newly assigned value:

```bash
$ unset x
$ echo "${x=default_value}"
default_value
$ echo "$x"
default_value
```

In this case, the interpreter assigns *x* the *default_value* string.

### 6. Tabular Summary

We can summarize the parameter expansion results for the different expressions in a table:

|       Expression        | *x* is set and isn’t null | *x* is set to the null string |        *x* is unset        |
| :---------------------: | :-----------------------: | :---------------------------: | :------------------------: |
| **${x:-default_value}** |           *$x*            |  *default_value* (不会修改x)  | *default_value*(不会修改x) |
| **${x-default_value}**  |           *$x*            |      *$x (null string)*       |      *default_value*       |
|   ${x:=default_value}   |           *$x*            |   *default_value* (会修改x)   | *default_value* (会修改x)  |
|   ${x=default_value}    |           *$x*            |      *$x (null string)*       |      *default_value*       |



## 三、《What is $0 in a Bash Script?》

- https://linuxhandbook.com/bash-dollar-0/

As you know, the `$` sign in the bash is used to indicate the variables. This is a variable too but a different one.

The `$0` is one of [the special variables you get in bash](https://linuxhandbook.com/bash-special-variables/) and is used to print the filename of the script that is currently being executed.

The `$0` variable can be used in two ways in Linux:

- Use `$0` to find the [logged-in shell](https://linuxhandbook.com/login-shell/)
- Use `$0` to print the name of the script that is being executed.

So let's start with the first one.

### 1、Find the logged-in shell using the $0

When not used in the script, the `$0` holds the information of which shell you are currently using. So if you print the value of the `$0`, it will show you which shell you are currently logged in.

For example, here, I was logged in to bash in one window while in the other, I was using the zsh and printing the value of `$0` [using echo command](https://linuxhandbook.com/echo-command/) reflected the names of the logged-in shells:

```
$ echo $0
/usr/bin/bash


LHB% echo $0
zsh
LHB%

```

Now, let's jump to how it can be used in the scripts.

### 2、Use $0 in shell scripts

To use the `$0` variable in the script, all you have to do is use a simple echo statement with this variable and it will print the filename.

For example, here I will be using a simple hello world script:

```
#!/bin/bash
echo "Hello World"
```

So if I have to use the `$0` variable here, all I have to do is add the following line at the end:

```
echo $0
```

To make it more human-readable, you can also use something like this:

```
echo "The name of the script is: " $0
```

So the final result would look like this:

```
#!/bin/bash
echo "Hello World"
echo "The name of the script is: " $0
```

And this is the output it gave me when I executed the script:

## 四、《BASH_SOURCE 与 $0之间的差别》

- https://stackoverflow.com/questions/35006457/choosing-between-0-and-bash-source

（，差别点：当被source引入的时候，BASH_SOURCE还是文件名，但是$0则不是文件名了；）

`${BASH_SOURCE[0]}` (or, more simply, `$BASH_SOURCE`[1] ) contains the (potentially relative) path of the containing script in *all* invocation scenarios, notably also when the script is *sourced*, which is not true for `$0`.

Furthermore, as [Charles Duffy](https://stackoverflow.com/users/14122/charles-duffy) points out, `$0` can be set to an *arbitrary* value by the caller.
On the flip side, `$BASH_SOURCE` *can* be empty, if no named file is involved; e.g.:
`echo 'echo "[$BASH_SOURCE]"' | bash`

The following example illustrates this:

Script `foo`:

```bash
#!/bin/bash
echo "[$0] vs. [${BASH_SOURCE[0]}]"
```

------

```bash
$ bash ./foo
[./foo] vs. [./foo]

$ ./foo
[./foo] vs. [./foo]

$ . ./foo
[bash] vs. [./foo]
```

`$0` is part of the POSIX shell specification, whereas `BASH_SOURCE`, as the name suggests, is Bash-specific.

------

[1] **Optional reading: `${BASH_SOURCE[0]}` vs. `$BASH_SOURCE`**:

Bash allows you to reference element `0` of an *array* variable using *scalar* notation: instead of writing `${arr[0]}`, you can write `$arr`; in other words: if you reference the variable *as if it were a scalar*, you get the element at index `0`.

Using this feature obscures the fact that `$arr` is an array, which is why popular shell-code linter [shellcheck.net](http://shellcheck.net/) issues the following warning (as of this writing):

> SC2128: Expanding an array without an index only gives the first element.

On a side note: While this warning is helpful, it could be more precise, because you won't necessarily get the *first* element: It is specifically the element at index `0` that is returned, so if the first element has a higher index - which is possible in Bash - you'll get the empty string; try `a[1]='hi'; echo "$a"`.
(By contrast, `zsh`, ever the renegade, returns *all* elements as a single string, separated with the first char. stored in `$IFS`, which is a *space* by default).

You may choose to eschew this feature due to its obscurity, but it works predictably and, pragmatically speaking, you'll rarely, if ever, need to access indices *other* than `0` of array variable `${BASH_SOURCE[@]}`.

------

**Optional reading, part 2: Under what conditions does the `BASH_SOURCE` array variable actually contain \*multiple\* elements?**:

`BASH_SOURCE` only has multiple entries *if function calls are involved*, in which case its elements parallel the `FUNCNAME` array that contains all function names *currently on the call stack*.

That is, inside a function, `${FUNCNAME[0]}` contains the name of the executing function, and `${BASH_SOURCE[0]}` contains the path of the script file in which that function is defined, `${FUNCNAME[1]}` contains the name of the function from which the currently executing function was called, if applicable, and so on.

If a given function was invoked directly from the top-level scope in the script file that defined the function at level `$i` of the call stack, `${FUNCNAME[$i+1]}` contains:

- `main` (a pseudo function name), if the script file was invoked *directly* (e.g., `./script`)
- `source` (a pseudo function name), if the script file was *sourced* (e.g. `source ./script` or `. ./script`).

