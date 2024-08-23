





### 问题记录

1、NoValidConnectionsError

> paramiko版本：2.4

报错代码：

- `paramiko.SSHClient().connect`
  - 实际上封装了2种异常：`ECONNREFUSED`，`EHOSTUNREACH`

报错：socket.error

```
paramiko.ssh_exception.NoValidConnectionsError: [Errno None] Unable to connect to port 22 
```

查看源码Doc，抛异常部分：

- `socket.error`：
  - if a socket error (other than connection-refused or host-unreachable) occurred while connecting.
- `NoValidConnectionsError(socket.error)`：继承错误
  - Multiple connection attempts were made and no families succeeded.
  - This exception class wraps multiple "real" underlying connection errors, all of which represent failed connection attempts. Because these errors are not guaranteed to all be of the same error type (i.e. different errno, `socket.error` subclass, message, etc) we expose a single unified error message and a ``None`` errno so that instances of this class match most normal handling of `socket.error` objects.
  - To see the wrapped exception objects, access the ``errors`` attribute. ``errors`` is a dict whose keys are address tuples (e.g. ``('127.0.0.1', 22)``) and whose values are the exception encountered trying to connect to that address.
  - It is implied/assumed that all the errors given to a single instance of this class are from connecting to the same hostname + port (and thus that the differences are in the resolution of the hostname - e.g. IPv4 vs v6).

出现场景：节点上的22端口没打开（重启等）

2、shell命令带&，导致永久hang

代码：`SSHClient().exec_command`，卡死在其返还值时，读取 `stdout.read()`

现象：执行 `iostat -xtm 1 > /home/out.txt &`

解决：修改为 `nohup iostat -xtm 1 >& /home/out.txt < /dev/null &`

- 参考：https://docs.fabfile.org/en/0.9.3/faq.html#why-can-t-i-run-programs-in-the-background-with-it-makes-fabric-hang



