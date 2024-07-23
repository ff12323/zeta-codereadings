









`sphinx/sphinx/util/docutils.py`

- 参考附录

```python
class CustomReSTDispatcher:
    """Custom reST's mark-up dispatcher.

    This replaces docutils's directives and roles dispatch mechanism for reST parser
    by original one temporarily.
    """

    def __init__(self) -> None:
        self.directive_func: Callable = lambda *args: (None, [])
        self.roles_func: Callable = lambda *args: (None, [])

    def __enter__(self) -> None:
        self.enable()

    def __exit__(
        self, exc_type: type[Exception], exc_value: Exception, traceback: Any,
    ) -> None:
        self.disable()

```



`sphinx/parsers.py`：

- 环境变量对象：是环境模块下初始化所提供的类。表示

```python
class Parser(docutils.parsers.Parser):
    """
    A base class of source parsers.  The additional parsers should inherit this class instead
    of ``docutils.parsers.Parser``.  Compared with ``docutils.parsers.Parser``, this class
    improves accessibility to Sphinx APIs.

    The subclasses can access sphinx core runtime objects (app, config and env).
    """

    #: The config object
    config: Config

    #: The environment object
    env: BuildEnvironment

    def set_application(self, app: Sphinx) -> None:
        """set_application will be called from Sphinx to set app and other instance variables

        :param sphinx.application.Sphinx app: Sphinx application object
        """
        self._app = app
        self.config = app.config
        self.env = app.env
```



`sphinx/environment/__init__.py`

- 行数：从138 ~ 737 是一个较大的类。来表示各种类型，各种情况的环境变量。
- 初始化：

```python
class BuildEnvironment:
    """
    The environment in which the ReST files are translated.
    （， 存储了一个清单里面有着夸文件目标，）
    Stores an inventory of cross-file targets and provides doctree
    transformations to resolve links to them.
    """
```



`sphinx/application.py`

文件：整个应用的类，以及扩展性接口； 很好第适配了已有的文本xxx系统；

```python
"""Sphinx application class and extensibility interface.

Gracefully adapted from the TextPress system by Armin.
"""
```

主类：

```python
class Sphinx:
    """The main application class and extensibility interface.（主应用类和扩展性接口）

    :ivar srcdir: Directory containing source.（包含源文件的目录）
    :ivar confdir: Directory containing ``conf.py``.（包含配置文件的目录）
    :ivar doctreedir: Directory for storing pickled doctrees.（目录永远存储序列好的文档树）
    :ivar outdir: Directory for storing build documents.（，）
    """
```



# 附录：

### 一、《python `__enter__` 与 `__exit__`的作用，以及与 with 语句的关系》

- https://www.cnblogs.com/lipijin/p/4460487.html

有一些任务，可能事先需要设置，事后做清理工作。对于这种场景，Python的with语句提供了一种非常方便的处理方式。一个很好的例子是文件处理，你需要获取一个文件句柄，从文件中读取数据，然后关闭文件句柄。

如果不用with语句，代码如下：

```python
file = open("/tmp/foo.txt")
data = file.read()
file.close()
```

这里有两个问题。一是可能忘记关闭文件句柄；二是文件读取数据发生异常，没有进行任何处理。下面是处理异常的加强版本：

```python
file = open("/tmp/foo.txt")
try:
    data = file.read()
finally:
    file.close()
```

虽然这段代码运行良好，但是太冗长了。这时候就是with一展身手的时候了。除了有更优雅的语法，with还可以很好的处理上下文环境产生的异常。下面是with版本的代码：

```python
with open("/tmp /foo.txt") as file:
    data = file.read()
```

**with如何工作？**

这看起来充满魔法，但不仅仅是魔法，Python对with的处理还很聪明。基本思想是with所求值的对象必须有一个`__enter__()`方法，一个`__exit__()`方法。

紧跟with后面的语句被求值后，返回对象的`__enter__()`方法被调用，这个方法的返回值将被赋值给as后面的变量。当with后面的代码块全部被执行完之后，将调用前面返回对象的`__exit__()`方法。

下面例子可以具体说明with如何工作：

```python
#!/usr/bin/env python
# with_example01.py

class Sample:
    def __enter__(self):
        print "In __enter__()"
        return "Foo"

    def __exit__(self, type, value, trace):
        print "In __exit__()"


def get_sample():
    return Sample()


with get_sample() as sample:
    print "sample:", sample
```

输出如下

```bash
bash-3.2$ ./with_example01.py
In __enter__()
sample: Foo
In __exit__()
```

正如你看到的，

`__enter__()`方法被执行
`__enter__()`方法返回的值 - 这个例子中是"Foo"，赋值给变量'sample'
执行代码块，打印变量"sample"的值为 "Foo"
`__exit__()`方法被调用

**with真正强大之处是它可以处理异常。可能你已经注意到Sample类的`__exit__`方法有三个参数`val`,`type` 和 `trace`。 这些参数在异常处理中相当有用。我们来改一下代码，看看具体如何工作的。**

```python
#!/usr/bin/env python
# with_example02.py


class Sample:
    def __enter__(self):
        return self

    def __exit__(self, type, value, trace):
        print "type:", type
        print "value:", value
        print "trace:", trace

    def do_something(self):
        bar = 1/0
        return bar + 10

with Sample() as sample:
    sample.do_something()
```

这个例子中，with后面的get_sample()变成了Sample()。这没有任何关系，只要紧跟with后面的语句所返回的对象有 `__enter__()`和`__exit__()`方法即可。此例中，Sample()的`__enter__()`方法返回新创建的Sample对象，并赋值给变量sample。

代码执行后：

```bash
bash-3.2$ ./with_example02.py
type: <type 'exceptions.ZeroDivisionError'>
value: integer division or modulo by zero
trace: <traceback object at 0x1004a8128>
Traceback (most recent call last):
  File "./with_example02.py", line 19, in <module>
    sample.do_somet hing()
  File "./with_example02.py", line 15, in do_something
    bar = 1/0
ZeroDivisionError: integer division or modulo by zero
BASH 复制 全屏
```

实际上，在with后面的代码块抛出任何异常时，`__exit__()`方法被执行。正如例子所示，异常抛出时，与之关联的type，value和stack trace传给`__exit__()`方法，因此抛出的ZeroDivisionError异常被打印出来了。开发库时，清理资源，关闭文件等等操作，都可以放在`__exit__`方法当中。

因此，Python的with语句是提供一个有效的机制，让代码更简练，同时在异常产生时，清理工作更简单。

