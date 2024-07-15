项目地址：https://github.com/fabric/fabric

描述：Simple, Pythonic remote execution and deployment.

项目依赖：

- 《Invoke项目》： https://github.com/pyinvoke/invoke

介绍：

Fabric is a high level Python (2.7, 3.4+) library designed to execute shell commands remotely over SSH, yielding useful Python objects in return. It builds on top of [Invoke](https://pyinvoke.org/) (subprocess command execution and command-line features) and [Paramiko](https://paramiko.org/) (SSH protocol implementation), extending their APIs to complement one another and provide additional functionality.



## ch0 记录



阅读记录：

- 这个仓库的python代码，如果一个函数的参数个数十分多（如，init）则会一个参数一行写法。避免调整时的对齐调整。
- 我的阅读代码问题：对于一个python项目来说，其使用了很多python的语言特性与功能，但是我几乎没有使用过，也未曾阅读多。导致我看的时候，面对一个基础的部分，却也是看一点就阻塞，不得不查找基础知识。效率十分低下



第一次&&重要：

- part2：
  - ch01：
    - 1.1：**提前引用（forward reference）**



> 《**The Zen of Python**》
>
> 1、Beautiful is better than ugly.
> 2、Explicit is better than implicit.
> 3、Simple is better than complex.
> 4、Complex is better than complicated.
> 5、Flat is better than nested.
> 6、Sparse is better than dense.
> 7、Readability counts.
> 8、Special cases aren't special enough to break the rules.
> 9、Although practicality beats purity.
> 10、Errors should never pass silently.
> 11、Unless explicitly silenced.
> 12、**（面对模糊性，拒绝去猜测的诱惑）In the face of ambiguity, refuse the temptation to guess.**
> There should be one-- and preferably only one --obvious way to do it.
> Although that way may not be obvious at first unless you're Dutch.
> Now is better than never.
> Although never is often better than *right* now.
> If the implementation is hard to explain, it's a bad idea.
> If the implementation is easy to explain, it may be a good idea.
> Namespaces are one honking great idea -- let's do more of those!



## ch02 runners

- 项目依赖：《Invoke项目》



### 1、Remote

【类，Remote，继承runner】

（，在一个SSH连接上运行一个shell命令；这个类是runner的子类；。。。）Run a shell command over an SSH connection. This class subclasses `invoke.runners.Runner`; please see its documentation for most public API details.

`__init__(self, *args, **kwargs)`：（，是对于父类的init的包装器；相比于父类，其额外的参数被定义会在下方列出来）Thin wrapper for superclass ``__init__``; please see it for details. Additional keyword arguments defined here are listed below.

- 出栈去除inline_env的参数名
- 调用父类的init



kill：Just close the channel immediately, which is about as close as we can get to a local SIGKILL unfortunately. TODO: consider _also_ calling .send_interrupt() and only doing this after another few seconds; but A) kinda fragile/complex and B) would belong in invoke.Runner anyways?



## ch02 config

【Config类，是invoke里的config的子类】：An `invoke.config.Config` subclass with extra Fabric-related behavior.

This class behaves like `invoke.config.Config` in every way, with the following exceptions:



## ch03 Connection

【类，Connection继承invoke的Context类】

A connection to an SSH daemon, with methods for commands and file transfer.

**Basics**

This class inherits from Invoke’s `Context`, as it is a context within which commands, tasks etc can operate. It also encapsulates a Paramiko `SSHClient` instance, performing useful high level operations with that `SSHClient` and `Channel` instances generated from it.

> Note
>
> Many SSH specific options – such as specifying private keys and passphrases, timeouts, disabling SSH agents, etc – are handled directly by Paramiko and should be specified via the [connect_kwargs argument](https://docs.fabfile.org/en/latest/api/connection.html#connect-kwargs-arg) of the constructor.

**Lifecycle**

（，连接的基本生命周期：创建，连接/打开，工作，断开/关闭）`Connection` has a basic “`create`, `connect/open`, `do work`, `disconnect/close`” lifecycle:

- `Instantiation` imprints the object with its connection parameters (but does **not** actually initiate the network connection).
  - An alternate constructor exists for users upgrading piecemeal from Fabric 1: `from_v1`
- Methods like `run`, `get` etc automatically trigger a call to `open` if the connection is not active; users may of course call `open` manually if desired.
- It’s best to explicitly close your connections when done using them. This can be accomplished by manually calling `close`, or by using the object as a contextmanager:

```c
with Connection('host') as c:
   c.run('command')
   c.put('file')
```

> Warning
>
> While Fabric (and Paramiko) attempt to register connections for automatic garbage collection, it’s not currently safe to rely on that feature, as it can lead to end-of-process hangs and similar behavior.
>
> Note
>
> This class rebinds `invoke.context.Context.run` to `local` so both remote and local command execution can coexist.



。。。

。。。



**run**(*command*, ***kwargs*)

Execute a shell command on the remote end of this connection.

This method wraps an SSH-capable implementation of `invoke.runners.Runner.run`; see its documentation for details.

> Warning
>
> There are a few spots where Fabric departs from Invoke’s default settings/behaviors; they are documented under `Config.global_defaults`.

*New in version 2.0.*

。。。



**sudo**(*command*, ***kwargs*)

Execute a shell command, via `sudo`, on the remote end.

（，？？？如何配置？）This method is identical to `invoke.context.Context.sudo` in every way, except in that – like `run` – it honors per-host/per-connection configuration overrides in addition to the generic/global ones. Thus, for example, **per-host sudo passwords may be configured**.

*New in version 2.0.*

> - TODO: allow subclassing for 'get the password' so users who REALLY want lazy runtime prompting can have it easily implemented.
> - TODO: want to print a "cleaner" echo with just 'sudo <command>'; but hard to do as-is, obtaining config data from outside a Runner one holds is currently messy (could fix that), if instead we manually inspect the config ourselves that duplicates logic. NOTE: once we figure that out, there is an existing, would-fail-if-not-skipped test for this behavior in test/context.py.
> - TODO: once that is done, though: how to handle "full debug" output exactly (display of actual, real full sudo command w/ -S and -p), in terms of API/config? Impl is easy, just go back to passing echo through to 'run'...

- 传入自身的 remote runner对象，命令字符串，和kwargs参数
- 检查自身配置的 `sudo.prompt`
- 从kwargs里出栈：
  - password参数，无则从自身配置里取。
  - user参数，无则从自身配置里取。
    - 如果存在user，则sudo执行需要命令参数 （例子：`-H -u root` ）
- 从kwargs里获取env参数，无则用默认值 `{}`
  - 有则，sudo执行需要参数 （例子：--preserve-env=环境变量名逗号分隔）
- 构造命令字符串：`sudo -S -p xxx -H -u root --preserve-env=环境变量名逗号分隔  具体命令 ` 
- 构建自身的 FailingResponder（Variant of `Responder` which is capable of <font color="#F9DDA2">detecting incorrect responses</font>.）
  - pattern：通过 re的escape，对自定义的sudo的prompt提示符进行元字符转移，确保完整匹配。
  - response：响应的是密码加换行符。
  - sentinel：`"Sorry, try again.\n"`
- Ensure we merge any user-specified watchers with our own.
  - NOTE: If there are config-driven watchers, we pull those up to the kwarg level; that lets us merge cleanly without needing complex config-driven "override vs merge" semantics.
  - TODO: if/when those semantics are implemented, use them instead.
  - NOTE: config value for watchers defaults to an empty list; and we want to clone it to avoid actually mutating the config
- 从kwargs里出栈 watchers参数，无则从配置中取。
- 将上面的FailingResponder的watcher放到list尾部。
- try运行：
  - 返回 `runner.run(cmd_str, watchers=watchers, **kwargs)`
- 捕获 Failure异常：
  - 如果是ResponseNotAccepted异常，抛出 AuthFailure 错误
- 否则是其他的，直接重新正常抛出。



## ch06 tunnel

（，隧道和连接转发的内部原理？）Tunnel and connection forwarding internals. 

If you're looking for simple, end-user-focused connection forwarding, please see `.Connection`, e.g. `.Connection.forward_local`.

> 问：什么是connection forwarding？

### 2、Tunnel

（在一个SSH通道和本地套接字之间的双向数据转发）Bidirectionally forward data between an SSH channel and local socket.

# Part2 《Invoke项目》

- https://github.com/pyinvoke/invoke

（，用于管理面向shell的子进程和组织可执行的Python代码到CLI可激活的任务中；。。。）Invoke is a Python (2.7 and 3.4+) library for managing shell-oriented subprocesses and organizing executable Python code into CLI-invokable tasks. It draws inspiration from various sources (`make`/`rake`, Fabric 1.x, etc) to arrive at a powerful & clean feature set.



## ch01 runners

Runner对象：

### 1、描述

（核心命令运行API的部分抽象）Partially-abstract core command-running API.

（，这个类本身不可用，必须在子类中实现一些方法。。。）This class is not usable by itself and must be subclassed, implementing a number of methods such as `start`, `wait` and `returncode`. For a subclass implementation example, see the source code for `.Local`.

### 1.1 初始化

` def __init__(self, context: "Context") -> None:`

- context：是一个（上下文对象）的实例。用于传递默认选项和提供其它上下文化的信息访问（如：对于远程连接需要持有主机名和端口号）
- `self.program_finished = threading.Event()`： （？？？）Typically set after `wait` returns. <font color="clan">Some IO mechanisms rely on this to know when to exit an infinite read loop</font>.
- 。。。
- `self.watchers: List["StreamWatcher"] = []`：（，用于进行响应。其在运行时被run方法填充上；）A list of `.StreamWatcher` instances for use by `respond`. Is filled in at runtime by `run`.
- 。。。（略）

> ？1、Event对象：
>
> This is one of the simplest mechanisms for communication between threads: one thread signals an event and other threads wait for it.  
>
> An event object manages an internal flag that can be set to true with the set() method and reset to false with the clear() method. The wait() method blocks until the flag is true.



> ？2、python feature：**type hint** 之字符串名称（而不是类型本身）
>
> - 目的：如下所诉，可以起到一个**提前引用（forward reference）**的目的。但是实际指的还是一个类型
>
> ===
>
> - https://stackoverflow.com/questions/51629105/python-type-hint-given-as-a-string
>
> type annotations sometimes need to be set before the object has been created, for example:
>
> ```python
> class Node:
>     def next() -> Node:
>         pass
> ```
>
> This piece of code actually fails, because `Node` is referenced as an annotation of `Node.next` while the class `Node` is still being created. This is the same reason why the following fails:
>
> ```python
> class T:
>     t = T()
> ```
>
> To get around this, you can use a string instead
>
> ```python
> class Node:
>     def next() -> 'Node':
>         pass
> ```
>
> so the typechecker will only evaluate `Node` later (a [forward reference](https://www.python.org/dev/peps/pep-0484/#forward-references)).
>
> ------
>
> This was actually [decided](https://www.python.org/dev/peps/pep-0563/) to be a design flaw so in python 3.7 you can use `from __future__ import annotations` and the first example will work.
>
> ------
>
> **Edit April 2022**
>
> So a lot has changed (and yet a lot hasn't) since this was made. The most important difference is that string `annotations` was deferred from py 3.10 and it [sounds](https://discuss.python.org/t/type-annotations-pep-649-and-pep-563/11363) like it will never be stabilized. Instead the steering committee will probably end up stabilizing [PEP 649](https://peps.python.org/pep-0649/) instead which has slightly more complex behavior, but still supports self-referential type hints. See the PEP for details.



### 2、run

Execute ``command``, returning an instance of `Result` once complete.

（，默认，这个方法是同步的（只会在子进程结束后返回），并且允许与子进程间的交互性按键通信；）By default, this method is synchronous (it only returns once the subprocess has completed), and allows interactive keyboard communication with the subprocess.

（，其可以用异步的方式替换运行（提早返回 并 请求与result对象进行交互 而管理子进程的生命周期）。。。更多的，你可以完全地与子进程进行解相关于Invoke的控制（允许其在Python退出后自己存在）。。。；）It can instead behave asynchronously (returning early & requiring interaction with the resulting object to manage subprocess lifecycle) if you specify `asynchronous=True`. Furthermore, you can completely disassociate the subprocess from Invoke’s control (allowing it to persist on its own after Python exits) by saying `disown=True`. See the per-kwarg docs below for details on both of these.

> Note
>
> All kwargs will default to the values found in this instance’s `Runner.context` attribute, specifically in its configuration’s `run` subtree (e.g. `run.echo` provides the default value for the `echo` keyword, etc). The base default values are described in the parameter list below.

Parameters

- **command** ([*str*](https://docs.python.org/2.7/library/functions.html#str)) – The shell command to execute.

- **asynchronous** ([*bool*](https://docs.python.org/2.7/library/functions.html#bool)) –

  When set to `True` (default `False`), enables asynchronous behavior, as follows:

  - Connections to the controlling terminal are disabled, meaning you will not see the subprocess output and it will not respond to your keyboard input - similar to `hide=True` and `in_stream=False` (though explicitly given `(out|err|in)_stream` file-like objects will still be honored as normal).

  - [`run`](https://docs.pyinvoke.org/en/stable/api/runners.html#invoke.runners.Runner.run) returns immediately after starting the subprocess, and its return value becomes an instance of [`Promise`](https://docs.pyinvoke.org/en/stable/api/runners.html#invoke.runners.Promise) instead of [`Result`](https://docs.pyinvoke.org/en/stable/api/runners.html#invoke.runners.Result).

  - [`Promise`](https://docs.pyinvoke.org/en/stable/api/runners.html#invoke.runners.Promise) objects are primarily useful for their [`join`](https://docs.pyinvoke.org/en/stable/api/runners.html#invoke.runners.Promise.join) method, which blocks until the subprocess exits (similar to threading APIs) and either returns a final [`Result`](https://docs.pyinvoke.org/en/stable/api/runners.html#invoke.runners.Result) or raises an exception, just as a synchronous `run` would.

    > - As with threading and similar APIs, users of `asynchronous=True` should make sure to `join` their [`Promise`](https://docs.pyinvoke.org/en/stable/api/runners.html#invoke.runners.Promise) objects to prevent issues with interpreter shutdown.
    > - One easy way to handle such cleanup is to use the [`Promise`](https://docs.pyinvoke.org/en/stable/api/runners.html#invoke.runners.Promise) as a context manager - it will automatically `join` at the exit of the context block.

  *New in version 1.4.*

- **disown** ([*bool*](https://docs.python.org/2.7/library/functions.html#bool)) –

  When set to `True` (default `False`), returns immediately like `asynchronous=True`, but does not perform any background work related to that subprocess (it is completely ignored). This allows subprocesses using shell backgrounding or similar techniques (e.g. trailing `&`, `nohup`) to persist beyond the lifetime of the Python process running Invoke.

  > Note
  >
  > If you’re unsure whether you want this or `asynchronous`, you probably want `asynchronous`!

  Specifically, `disown=True` has the following behaviors:

  - The return value is `None` instead of a [`Result`](https://docs.pyinvoke.org/en/stable/api/runners.html#invoke.runners.Result) or subclass.
  - No I/O worker threads are spun up, so you will have no access to the subprocess’ stdout/stderr, your stdin will not be forwarded, `(out|err|in)_stream` will be ignored, and features like `watchers` will not function.
  - No exit code is checked for, so you will not receive any errors if the subprocess fails to exit cleanly.
  - `pty=True` may not function correctly (subprocesses may not run at all; this seems to be a potential bug in Python’s `pty.fork`) unless your command line includes tools such as `nohup` or (the shell builtin) `disown`.

  *New in version 1.4.*

- **dry** ([*bool*](https://docs.python.org/2.7/library/functions.html#bool)) –

  Whether to dry-run instead of truly invoking the given command. See [`--dry`](https://docs.pyinvoke.org/en/stable/invoke.html#cmdoption-dry) (which flips this on globally) for details on this behavior.

  *New in version 1.3.*

- **echo** ([*bool*](https://docs.python.org/2.7/library/functions.html#bool)) –

  （，功能：控制run是否打印执行命令的字符串到本地的stdout，在执行之前；默认：不打印）Controls whether [`run`](https://docs.pyinvoke.org/en/stable/api/runners.html#invoke.runners.Runner.run) prints the command string to local stdout prior to executing it. Default: `False`.

  > Note
  >
  > `hide=True` will override `echo=True` if both are given.

- **echo_format** –

  A string, which when passed to Python’s inbuilt `.format` method, will change the format of the output when `run.echo` is set to true.

  Currently, only `{command}` is supported as a parameter.

  Defaults to printing the full command string in ANSI-escaped bold.

- **echo_stdin** ([*bool*](https://docs.python.org/2.7/library/functions.html#bool)) –

  Whether to write data from `in_stream` back to `out_stream`.

  In other words, in normal interactive usage, this parameter controls whether Invoke mirrors what you type back to your terminal.

  By default (when `None`), this behavior is triggered by the following:

  - Not using a pty to run the subcommand (i.e. `pty=False`), as ptys natively echo stdin to stdout on their own;
  - And when the controlling terminal of Invoke itself (as per `in_stream`) appears to be a valid terminal device or TTY. (Specifically, when [`isatty`](https://docs.pyinvoke.org/en/stable/api/util.html#invoke.util.isatty) yields a `True` result when given `in_stream`.)

  > Note
  >
  > This property tends to be `False` when piping another program’s output into an Invoke session, or when running Invoke within another program (e.g. running Invoke from itself).

  If both of those properties are true, echoing will occur; if either is false, no echoing will be performed.

  When not `None`, this parameter will override that auto-detection and force, or disable, echoing.

- **encoding** ([*str*](https://docs.python.org/2.7/library/functions.html#str)) – Override auto-detection of which encoding the subprocess is using for its stdout/stderr streams (which defaults to the return value of [`default_encoding`](https://docs.pyinvoke.org/en/stable/api/runners.html#invoke.runners.default_encoding)).

- **err_stream** – Same as `out_stream`, except for standard error, and defaulting to `sys.stderr`.

- **env** ([*dict*](https://docs.python.org/2.7/library/stdtypes.html#dict)) –

  By default, subprocesses receive a copy of Invoke’s own environment (i.e. `os.environ`). Supply a dict here to update that child environment.

  For example, `run('command', env={'PYTHONPATH': '/some/virtual/env/maybe'})` would modify the `PYTHONPATH` env var, with the rest of the child’s env looking identical to the parent.

  > See also
  >
  > `replace_env` for changing ‘update’ to ‘replace’.

- **fallback** ([*bool*](https://docs.python.org/2.7/library/functions.html#bool)) – Controls auto-fallback behavior re: problems offering a pty when `pty=True`. Whether this has any effect depends on the specific [`Runner`](https://docs.pyinvoke.org/en/stable/api/runners.html#invoke.runners.Runner) subclass being invoked. Default: `True`.

- **hide** –

  Allows the caller to disable `run`’s default behavior of copying the subprocess’ stdout and stderr to the controlling terminal. Specify `hide='out'` (or `'stdout'`) to hide only the stdout stream, `hide='err'` (or `'stderr'`) to hide only stderr, or `hide='both'` (or `True`) to hide both streams.

  The default value is `None`, meaning to print everything; `False` will also disable hiding.

  > Note
  >
  > Stdout and stderr are always captured and stored in the `Result` object, regardless of `hide`’s value.

- **in_stream** –

  A file-like stream object to used as the subprocess’ standard input. If `None` (the default), `sys.stdin` will be used.

  If `False`, will disable stdin mirroring entirely (though other functionality which writes to the subprocess’ stdin, such as autoresponding, will still function.) Disabling stdin mirroring can help when `sys.stdin` is a misbehaving non-stream object, such as under test harnesses or headless command runners.

- **out_stream** – A file-like stream object to which the subprocess’ standard output should be written. If `None` (the default), `sys.stdout` will be used.

- **pty** ([*bool*](https://docs.python.org/2.7/library/functions.html#bool)) –

  By default, `run` connects directly to the invoked process and reads its stdout/stderr streams. Some programs will buffer (or even behave) differently in this situation compared to using an actual terminal or pseudoterminal (pty). To use a pty instead of the default behavior, specify `pty=True`.

  > Warning
  >
  > Due to their nature, ptys have a single output stream, so the ability to tell stdout apart from stderr is **not possible** when `pty=True`. As such, all output will appear on `out_stream` (see below) and be captured into the `stdout` result attribute. `err_stream` and `stderr` will always be empty when `pty=True`.

- **replace_env** ([*bool*](https://docs.python.org/2.7/library/functions.html#bool)) – When `True`, causes the subprocess to receive the dictionary given to `env` as its entire shell environment, instead of updating a copy of `os.environ` (which is the default behavior). Default: `False`.

- **shell** ([*str*](https://docs.python.org/2.7/library/functions.html#str)) – Which shell binary to use. Default: `/bin/bash` (on Unix; `COMSPEC` or `cmd.exe` on Windows.)

- **timeout** –

  （，使得runner提交一个中断到子进程并抛出一个CommandTimeout异常；如果命令的执行时间超过了给定的时间。默认树None，表示没有超时；）Cause the runner to submit an interrupt to the subprocess and raise [`CommandTimedOut`](https://docs.pyinvoke.org/en/stable/api/exceptions.html#invoke.exceptions.CommandTimedOut), if the command takes longer than `timeout` seconds to execute. Defaults to `None`, meaning no timeout.

  *New in version 1.3.*

- **warn** ([*bool*](https://docs.python.org/2.7/library/functions.html#bool)) –

  （，是否告警并继续，而不是抛异常UnexpectedExit，当这个执行的命令以一个非0的状态码返回时。默认：抛异常）Whether to warn and continue, instead of raising [`UnexpectedExit`](https://docs.pyinvoke.org/en/stable/api/exceptions.html#invoke.exceptions.UnexpectedExit), when the executed command exits with a nonzero status. Default: `False`.

  > Note
  >
  > This setting has no effect on exceptions, which will still be raised, typically bundled in [`ThreadException`](https://docs.pyinvoke.org/en/stable/api/exceptions.html#invoke.exceptions.ThreadException) objects if they were raised by the IO worker threads.
  >
  > Similarly, [`WatcherError`](https://docs.pyinvoke.org/en/stable/api/exceptions.html#invoke.exceptions.WatcherError) exceptions raised by [`StreamWatcher`](https://docs.pyinvoke.org/en/stable/api/watchers.html#invoke.watchers.StreamWatcher) instances will also ignore this setting, and will usually be bundled inside [`Failure`](https://docs.pyinvoke.org/en/stable/api/exceptions.html#invoke.exceptions.Failure) objects (in order to preserve the execution context).
  >
  > Ditto [`CommandTimedOut`](https://docs.pyinvoke.org/en/stable/api/exceptions.html#invoke.exceptions.CommandTimedOut) - basically, anything that prevents a command from actually getting to “exited with an exit code” ignores this flag.

- **watchers** –

  A list of [`StreamWatcher`](https://docs.pyinvoke.org/en/stable/api/watchers.html#invoke.watchers.StreamWatcher) instances which will be used to scan the program’s `stdout` or `stderr` and may write into its `stdin` (typically `bytes` objects) in response to patterns or other heuristics.

  See [Automatically responding to program output](https://docs.pyinvoke.org/en/stable/concepts/watchers.html) for details on this functionality.

  Default: `[]`.

### 2.1 run流程

- 捕获异常：
  - `_run_body`：run具体实现
    - _setup：Prepare all the bits n bobs.（Prepare data on ``self`` so we're ready to start running）
      - Normalize kwargs w/ config; sets self.opts, self.streams
        - Unify `run` kwargs with config options to arrive at local options.
        - 
      - （环境变量设置）Environment setup
  - If dry-run, stop here.
  - Start executing the actual command (runs in background)
  - （，如果是disown，则返回None）If disowned, we just stop here - no threads, no timer, no error checking, nada.
  - Stand up & kick off IO, timer threads
    - self.start_timer：启动定时器，超时时间为参数。
      - 如果参数不为None：
        - `_timer` 为 Thread的timer函数，处理函数为自身的kill函数
        - 启动timer
    - create_io_threads：（，创建并返回一个IO线程工作者对象的字典。调用者应该持久化并或启动包装的线程）Create and return a dictionary of IO thread worker objects. Caller is expected to handle persisting and/or starting the wrapped threads.
      - （，设置IO线程参数）Set up IO thread parameters (format - body_func: {kwargs})
      - （，在前面的可选项的处理后，）After opt processing above, in_stream will be a real stream obj or False, so we can truth-test it. We don't even create a stdin-handling thread if it's False, meaning user indicated stdin is nonexistent or problematic.
- finally：
  - 如果没有异步或者diown：
    - stop：走停止流程。





## ch02 config

【config是DataProxy的子类（2.1节）】

（，项目的主要配置处理类；）Invoke’s primary configuration handling class.

See [Configuration](https://docs.pyinvoke.org/en/stable/concepts/configuration.html) for details on the configuration system this class implements, including the [configuration hierarchy](https://docs.pyinvoke.org/en/stable/concepts/configuration.html#config-hierarchy). The rest of this class’ documentation assumes familiarity with that document.



### 2.1 DataProxy

Helper class implementing nested dict+attr access for `.Config`.

Specifically, is used both for `.Config` itself, and to wrap any other dicts assigned as config values (recursively).

> warning：
>
> （，这个对象或者子类的所有方法，都要使用set方法初始化新的属性。否则运行会出现错误）All methods (of this object or in subclasses) must take care to initialize new attributes via ``self._set(name='value')``, or they'll run into recursion errors!



`_set`方法：（参数：`*args: Any, **kwargs: Any`）

- 通过 `if args` ，来判断args元组对象，其是否存在边长列表元素存在。
  - 通过 `object.__setattr__(self, *args)`。注意1：name必须是字符串名称。 注意2：args只能传入2个变长参数。

- 遍历所有kwargs：
  - 将其key，value设置到类自身的属性上去。


> `object.__setattr__(self, name, value)`
>
> Called when an attribute assignment is attempted. This is called instead of the normal mechanism (i.e. store the value in the instance dictionary). *name* is the attribute name, *value* is the value to be assigned to it.

`__setattr__`方法：（参数：` key: str, value: Any`）

> Note：1、当我们给一个类实例通过 `t.f1 = xxx` 时直接设置属性时会调用。
>
> 2、内置方法 `dir`([*object*])：无参，则返回当前局部范围的一个名字列表；带一个参数，尝试这个对象的一个有效属性列表；
>
> 

Turn attribute-sets into config updates anytime we don't have a real attribute with the given name/key.

- 通过 `dir(self)` 判断给定的key是否在属性里面。
- 如果不存在，则调用 `self[key]` 设置值。也就是调用自定义的 `__setitem__`方法。（。Make sure to trigger our own `__setitem__` instead of going direct to our internal dict/cache）
- 否则：调用父类（object）的 `__setattr__`方法添加属性。（注：为什么要父类的，因为子类就是自身已经定义了。如果不这样会产生循环调用。）





## ch06 context

Context-aware API wrapper & state-passing object.





## doc01 Configuration

### 1.1 介绍

Invoke offers a multifaceted configuration mechanism allowing you to configure both core behavior and that of your tasks, via a hierarchy of configuration files, environment variables, [task namespaces](https://docs.pyinvoke.org/en/stable/concepts/namespaces.html) and CLI flags.

The end result of configuration seeking, loading, parsing & merging is a `Config` object, which behaves like a (nested) Python dictionary. Invoke references this object when it runs (determining the default behavior of methods like `Context.run`) and exposes it to users’ tasks as `Context.config` or as shorthand attribute access on the `Context` itself.

### 1.2 配置层级结构

**The configuration hierarchy**

（，配置的值的覆盖顺序如下。。。）In brief, the order in which configuration values override one another is as follows:

1. （，内部默认值：。。。）**Internal default values** for behaviors which are controllable via configuration. See Default configuration values for details.
2. **Collection-driven configurations** defined in tasks modules via `Collection.configure`. (See Collection-based configuration below for details.)
   1. Sub-collections’ configurations get merged into the top level collection and the final result forms the basis of the overall configuration setup.
3. （，系统级别配置文件：）**System-level configuration file** stored in `/etc/`, such as `/etc/invoke.yaml`. (See [Configuration files](https://docs.pyinvoke.org/en/stable/concepts/configuration.html#config-files) for details on this and the other config-file entries.)
4. （，用户级别配置文件：。。。）**User-level configuration file** found in the running user’s home directory, e.g. `~/.invoke.yaml`.
5. （，项目级别配置文件：。。。）**Project-level configuration file** living next to your top level `tasks.py`. For example, if your run of Invoke loads `/home/user/myproject/tasks.py` (see our docs on [the load process](https://docs.pyinvoke.org/en/stable/concepts/loading.html)), this might be `/home/user/myproject/invoke.yaml`.
6. （，环境变量：。。。）**Environment variables** found in the invoking shell environment.
   1. These aren’t as strongly hierarchical as the rest, nor is the shell environment namespace owned wholly by Invoke, so we must rely on slightly verbose prefixing instead - see [Environment variables](https://docs.pyinvoke.org/en/stable/concepts/configuration.html#env-vars) for details.
7. （，运行时配置文件）**Runtime configuration file** whose path is given to [`-f`](https://docs.pyinvoke.org/en/stable/invoke.html#cmdoption-f), e.g. `inv -f /random/path/to/config_file.yaml`. This path may also be set via the `INVOKE_RUNTIME_CONFIG` env var.
8. （，命令行标志：）**Command-line flags** for certain core settings, such as [`-e`](https://docs.pyinvoke.org/en/stable/invoke.html#cmdoption-e).
9. （用户代码在运行时进行的修改）**Modifications made by user code** at runtime.

### 1.3 默认配置值

**Default configuration values**

。。。

## doc02 激活任务

**Invoking tasks**

This page explains how to invoke your tasks on the CLI, both in terms of parser mechanics (how your tasks’ arguments are exposed as command-line options) and execution strategies (which tasks actually get run, and in what order).



## doc03 将Invoke当做库用

**Using Invoke as a library**

While most of our documentation involves the user/CLI facing use cases of task management and command execution, Invoke was designed for its constituent parts to be usable independently by advanced users - either out of the box or with a minimum of extra work. CLI parsing, **subprocess command execution**, task organization, etc, are all written as broadly separated concerns.

（，下游工具：Fabric在用。。。）This document outlines use cases already known to work (because **downstream tools** like [Fabric](https://fabfile.org/) are already utilizing them).

