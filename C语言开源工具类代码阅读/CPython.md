

地址：https://github.com/python/cpython/tree/main



```
agnostic：If someone believes they cannot know whether or not God exists, that person is an agnostic. 
```



第一次记录：

- Lib：
  - A2：
    - 同时继承了2个类型，一个是独特类，一个是公共类（**mixin class** provides common functions）。
    - `class MyClass(object):` 为什么如下的类声明，继承自object？

# Lib：纯Python库



## A2：logging



### 1、convertXXX

ConvertingMixin：

- 要点：继承了 object类。

> 参考：https://stackoverflow.com/questions/4015417/why-do-python-classes-inherit-object、
>
> 问题：
>
> （，为什么如下的类声明，继承自object？）Why does the following class declaration inherit from `object`?
>
> ```python
> class MyClass(object):
>     ...
> ```
>
> 关键搜索："old vs. new-style python objects"（python对象的旧的、新的样式）
>
> 要点回答：在Python3是默认的，但是在Python2则不是。这样做是当需要在2者环境之间不确定时，而显式使用。。。
>
> > ### Is there any reason for a class declaration to inherit from `object`?
>
> In Python 3, apart from compatibility between Python 2 and 3, *no reason*. In Python 2, *many reasons*.
>
> ------
>
> ### Python 2.x story:
>
> In Python 2.x (from 2.2 onwards) there's two styles of classes depending on the presence or absence of `object` as a base-class:
>
> 1. **"classic" style** classes: they don't have `object` as a base class:
>
>    ```python
>    >>> class ClassicSpam:      # no base class
>    ...     pass
>    >>> ClassicSpam.__bases__
>    ()
>    ```
>
> 2. **"new" style** classes: they have, directly *or indirectly* (e.g inherit from a [built-in type](https://docs.python.org/3/library/stdtypes.html)), `object` as a base class:
>
>    ```python
>    >>> class NewSpam(object):           # directly inherit from object
>    ...    pass
>    >>> NewSpam.__bases__
>    (<type 'object'>,)
>    >>> class IntSpam(int):              # indirectly inherit from object...
>    ...    pass
>    >>> IntSpam.__bases__
>    (<type 'int'>,) 
>    >>> IntSpam.__bases__[0].__bases__   # ... because int inherits from object  
>    (<type 'object'>,)
>    ```
>
> Without a doubt, when writing a class you'll *always* want to go for new-style classes. The perks of doing so are numerous, to list some of them:
>
> - [Support for descriptors](https://docs.python.org/3/howto/descriptor.html). Specifically, the following constructs are made possible with descriptors:
>   1. [`classmethod`](https://docs.python.org/3/library/functions.html#classmethod): A method that receives the class as an implicit argument instead of the instance.
>   2. [`staticmethod`](https://docs.python.org/3/library/functions.html#staticmethod): A method that does not receive the implicit argument `self` as a first argument.
>   3. properties with [`property`](https://docs.python.org/3/library/functions.html#property): Create functions for managing the getting, setting and deleting of an attribute.
>   4. [`__slots__`](https://docs.python.org/3/reference/datamodel.html#slots): Saves memory consumptions of a class and also results in faster attribute access. Of course, it does [impose limitations](https://docs.python.org/3/reference/datamodel.html#notes-on-using-slots).
> - The [`__new__`](https://docs.python.org/3/reference/datamodel.html#object.__new__) static method: lets you customize how new class instances are created.
> - [Method resolution order (MRO)](https://www.python.org/download/releases/2.3/mro/): in what order the base classes of a class will be searched when trying to resolve which method to call.
> - Related to MRO, [`super` calls](https://docs.python.org/3/library/functions.html#super). Also see, [`super()` considered super.](https://rhettinger.wordpress.com/2011/05/26/super-considered-super/)
>
> If you don't inherit from `object`, forget these. A more exhaustive description of the previous bullet points along with other perks of "new" style classes can be found [here](https://www.python.org/download/releases/2.2.3/descrintro/).
>
> One of the downsides of new-style classes is that the class itself is more memory demanding. Unless you're creating many class objects, though, I doubt this would be an issue and it's a negative sinking in a sea of positives.
>
> ------
>
> ### Python 3.x story:
>
> （，在python3，事情变得简化了。只有新样式的类存在。）In Python 3, things are simplified. Only new-style classes exist (referred to plainly as classes) so, the only difference in adding `object` is requiring you to type in 8 more characters. This:
>
> ```python
> class ClassicSpam:
>     pass
> ```
>
> is completely equivalent (apart from their name :-) to this:
>
> ```python
> class NewSpam(object):
>      pass
> ```
>
> and to this:
>
> ```python
> class Spam():
>     pass
> ```
>
> All have `object` in their `__bases__`.
>
> ```python
> >>> [object in cls.__bases__ for cls in {Spam, NewSpam, ClassicSpam}]
> [True, True, True]
> ```
>
> ------
>
> ## So, what should you do?
>
> **In Python 2:** *always inherit from `object` explicitly*. Get the perks.
>
> **In Python 3:** inherit from `object` if you are writing code that tries to be Python agnostic, that is, it needs to work both in Python 2 and in Python 3. Otherwise don't, it really makes no difference since Python inserts it for you behind the scenes.

ConvertingDict：

- 要点：同时继承了2个类型，一个是独特类，一个是公共类（**mixin class** provides common functions）。
- 注释：
  - 此类是对Python的标准容器的封装器。它们服务于将这个容器的任何合适的值进行转化。
  - 转化：将基础的dict、list、和tuple转换到它们的包装的等效体上。。。。。
  - 要求：每一个包装器都要有一个配置器的属性，其持有真正的配置器而用于转化的。

```python
class ConvertingMixin(object):
    """For ConvertingXXX's, this mixin class provides common functions"""

    def convert_with_key(self, key, value, replace=True):
        result = self.configurator.convert(value)
        #If the converted value is different, save for next time
        if value is not result:
            if replace:
                self[key] = result
            if type(result) in (ConvertingDict, ConvertingList,
                               ConvertingTuple):
                result.parent = self
                result.key = key
        return result

    def convert(self, value):
        result = self.configurator.convert(value)
        if value is not result:
            if type(result) in (ConvertingDict, ConvertingList,
                               ConvertingTuple):
                result.parent = self
        return result


# The ConvertingXXX classes are wrappers around standard Python containers,
# and they serve to convert any suitable values in the container. The
# conversion converts base dicts, lists and tuples to their wrapped
# equivalents, whereas strings which match a conversion format are converted
# appropriately.
#
# Each wrapper should have a configurator attribute holding the actual
# configurator to use for conversion.

class ConvertingDict(dict, ConvertingMixin):
    """（，字段转化包装器）A converting dictionary wrapper."""

    def __getitem__(self, key):
        value = dict.__getitem__(self, key)
        return self.convert_with_key(key, value)

    def get(self, key, default=None):
        value = dict.get(self, key, default)
        return self.convert_with_key(key, value)

    def pop(self, key, default=None):
        value = dict.pop(self, key, default)
        return self.convert_with_key(key, value, replace=False)
```





### 2、BaseConfig

知识点：

- `importer = staticmethod(__import__)` ： 可能会使用不同的记录，
- Self：看不懂，需要补充这个方面的基础知识。。。

```
type(staticmethod(__import__))
<class 'staticmethod'>
```

> 装饰器 @classmethod 修饰的方法称为：类方法，在使用的时候，会将类本身作为第一个参数 cls 传递给类方法
>
> ```python
> class Web(object):
>     # 静态变量（类变量）
>     name = "Python_Web"
> 
>     # 类方法，第一个参数为cls，代表类本身
>     @classmethod
>     def foo_classmethod(cls):
>         # 调用静态变量方式一
>         print(cls.name)
> 
>         # 调用静态变量方式二
>         print(Web.name)
>         
> if __name__ == '__main__':
>     # 使用类名去调用类方法
>     Web.foo_classmethod()
> ```
>
> 





```python
class BaseConfigurator(object):
    """
    The configurator base class which defines some useful defaults.
    """

    CONVERT_PATTERN = re.compile(r'^(?P<prefix>[a-z]+)://(?P<suffix>.*)$')

    WORD_PATTERN = re.compile(r'^\s*(\w+)\s*')
    DOT_PATTERN = re.compile(r'^\.\s*(\w+)\s*')
    INDEX_PATTERN = re.compile(r'^\[\s*(\w+)\s*\]\s*')
    DIGIT_PATTERN = re.compile(r'^\d+$')

    value_converters = {
        'ext' : 'ext_convert',
        'cfg' : 'cfg_convert',
    }

    # We might want to use a different one, e.g. importlib
    importer = staticmethod(__import__)

    def __init__(self, config):
        self.config = ConvertingDict(config)
        self.config.configurator = self

    def resolve(self, s):
        """
        Resolve strings to objects using standard import and attribute
        syntax.
        """
        name = s.split('.')
        used = name.pop(0)
        try:
            found = self.importer(used)
            for frag in name:
                used += '.' + frag
                try:
                    found = getattr(found, frag)
                except AttributeError:
                    self.importer(used)
                    found = getattr(found, frag)
            return found
        except ImportError as e:
            v = ValueError('Cannot resolve %r: %s' % (s, e))
            raise v from e

    def ext_convert(self, value):
        """Default converter for the ext:// protocol."""
        return self.resolve(value)

    def cfg_convert(self, value):
        """Default converter for the cfg:// protocol."""
        rest = value
        m = self.WORD_PATTERN.match(rest)
        if m is None:
            raise ValueError("Unable to convert %r" % value)
        else:
            rest = rest[m.end():]
            d = self.config[m.groups()[0]]
            #print d, rest
            while rest:
                m = self.DOT_PATTERN.match(rest)
                if m:
                    d = d[m.groups()[0]]
                else:
                    m = self.INDEX_PATTERN.match(rest)
                    if m:
                        idx = m.groups()[0]
                        if not self.DIGIT_PATTERN.match(idx):
                            d = d[idx]
                        else:
                            try:
                                n = int(idx) # try as number first (most likely)
                                d = d[n]
                            except TypeError:
                                d = d[idx]
                if m:
                    rest = rest[m.end():]
                else:
                    raise ValueError('Unable to convert '
                                     '%r at %r' % (value, rest))
        #rest should be empty
        return d

    def convert(self, value):
        """
        Convert values to an appropriate type. dicts, lists and tuples are
        replaced by their converting alternatives. Strings are checked to
        see if they have a conversion format and are converted if they do.
        """
        if not isinstance(value, ConvertingDict) and isinstance(value, dict):
            value = ConvertingDict(value)
            value.configurator = self
        elif not isinstance(value, ConvertingList) and isinstance(value, list):
            value = ConvertingList(value)
            value.configurator = self
        elif not isinstance(value, ConvertingTuple) and\
                 isinstance(value, tuple) and not hasattr(value, '_fields'):
            value = ConvertingTuple(value)
            value.configurator = self
        elif isinstance(value, str): # str for py3k
            m = self.CONVERT_PATTERN.match(value)
            if m:
                d = m.groupdict()
                prefix = d['prefix']
                converter = self.value_converters.get(prefix, None)
                if converter:
                    suffix = d['suffix']
                    converter = getattr(self, converter)
                    value = converter(suffix)
        return value

    def configure_custom(self, config):
        """Configure an object with a user-supplied factory."""
        c = config.pop('()')
        if not callable(c):
            c = self.resolve(c)
        props = config.pop('.', None)
        # Check for valid identifiers
        kwargs = {k: config[k] for k in config if valid_ident(k)}
        result = c(**kwargs)
        if props:
            for name, value in props.items():
                setattr(result, name, value)
        return result

    def as_tuple(self, value):
        """Utility function which converts lists to tuples."""
        if isinstance(value, list):
            value = tuple(value)
        return value

```

## A3：🤢 unittest

#### 1、main类。

作用：命令行程序运行测试集；目的是为了使得测试模块更方便执行。。。

- 特点：这是一个在init就会运行完所有的测试用例的类。
- 1、模块：默认以main模块，否则指定的话则动态导入特定模块。
- 2、参数：为指定则用系统命令行参数，否则用指定参数。

```python
main = TestProgram

class TestProgram(object):
    """A command-line program that runs a set of tests; this is primarily
       for making test modules conveniently executable.
    """
    # defaults for testing
    module=None
    verbosity = 1
    failfast = catchbreak = buffer = progName = warnings = testNamePatterns = None
    _discovery_parser = None

    def __init__(self, module='__main__', defaultTest=None, argv=None,
                    testRunner=None, testLoader=loader.defaultTestLoader,
                    exit=True, verbosity=1, failfast=None, catchbreak=None,
                    buffer=None, warnings=None, *, tb_locals=False):
        if isinstance(module, str):
            self.module = __import__(module)
            for part in module.split('.')[1:]:
                self.module = getattr(self.module, part)
        else:
            self.module = module
        if argv is None:
            argv = sys.argv

        self.exit = exit
        self.failfast = failfast
        self.catchbreak = catchbreak
        self.verbosity = verbosity
        self.buffer = buffer
        self.tb_locals = tb_locals
        if warnings is None and not sys.warnoptions:
            # even if DeprecationWarnings are ignored by default
            # print them anyway unless other warnings settings are
            # specified by the warnings arg or the -W python flag
            self.warnings = 'default'
        else:
            # here self.warnings is set either to the value passed
            # to the warnings args or to None.
            # If the user didn't pass a value self.warnings will
            # be None. This means that the behavior is unchanged
            # and depends on the values passed to -W.
            self.warnings = warnings
        self.defaultTest = defaultTest
        self.testRunner = testRunner
        self.testLoader = testLoader
        self.progName = os.path.basename(argv[0])
        self.parseArgs(argv)
        self.runTests()
```

#### 2、运行测试

步骤：

- 1、安装信号量处理
- 2、安装运行测试类（默认文本输出，比较难看：`runner.TextTestRunner`）
- 3、（🤢）解析参数，并发现对应的测试用例。加入到列表中，后面执行。
  - 。。。
- 4、传入的testRunner，可能是一个类实例，也可能是类型（需要：进行各种实例化。。。 ？？？ 这种不断进行try的方式？？？比较奇怪）。
- 5、testRunner：负责运行测试，输出报告等

```python
    def runTests(self):
        if self.catchbreak:
            installHandler()
        if self.testRunner is None:
            self.testRunner = runner.TextTestRunner
        if isinstance(self.testRunner, type):
            try:
                try:
                    testRunner = self.testRunner(verbosity=self.verbosity,
                                                 failfast=self.failfast,
                                                 buffer=self.buffer,
                                                 warnings=self.warnings,
                                                 tb_locals=self.tb_locals)
                except TypeError:
                    # didn't accept the tb_locals argument
                    testRunner = self.testRunner(verbosity=self.verbosity,
                                                 failfast=self.failfast,
                                                 buffer=self.buffer,
                                                 warnings=self.warnings)
            except TypeError:
                # didn't accept the verbosity, buffer or failfast arguments
                testRunner = self.testRunner()
        else:
            # it is assumed to be a TestRunner instance
            testRunner = self.testRunner
        self.result = testRunner.run(self.test)
        if self.exit:
            # True---> 0, False --> 1 
            sys.exit(not self.result.wasSuccessful())
```

#### 3、🤢 runner类

接口：一共2个函数。

```python
TextTestRunner
  def _makeResult(self):
  def run(self, test): 
```



#### 4、🤢 测试集--可运行且可迭代对象。

大概：一个测试集，包含着多个测试用例（list）

- 1、`__call__`，这个够着好的对象。可以想调用函数一样，直接用。
- 2、`__iter__`，在运行run的时候可以 `for index, test in enumerate(self)`，把对象变为可迭代。取出一个一个测试用例，进行运作。



> =====
>
> ```
> type(test._tests[0]._tests[0])
>    <class 'bs_tests.DictTest'>
> test._tests[0]._tests
>   list [<bs_tests.DictTest testMethod=test_dict_const>]
> ```
>
> ====
>
> `getattr`(*object*, *name*[, *default*])：
>
> - 获取一个x.y属性值。无则抛异常，有给默认值则不抛。
>
> `if hasattr(test, 'countTestCases')`
>
> - （，通过有无抛异常，而判断一个对象是否存在某一个字段。）The arguments are an object and a string. The result is `True` if the string is the name of one of the object’s attributes, `False` if not. (This is implemented by calling `getattr(object, name)` and seeing whether it raises an [`AttributeError`](https://docs.python.org/3.10/library/exceptions.html#AttributeError) or not.)

```python
class BaseTestSuite(object):
    """A simple test suite that doesn't provide class or module shared fixtures.
    """
    _cleanup = True

    def __init__(self, tests=()):
        self._tests = []
        self._removed_tests = 0
        self.addTests(tests)

    def __repr__(self):
        return "<%s tests=%s>" % (util.strclass(self.__class__), list(self))

    def __eq__(self, other):
        if not isinstance(other, self.__class__):
            return NotImplemented
        return list(self) == list(other)

    def __iter__(self):
        return iter(self._tests)

    def countTestCases(self):
        cases = self._removed_tests
        for test in self:
            if test:
                cases += test.countTestCases()
        return cases

    def addTest(self, test):
        # sanity checks
        if not callable(test):
            raise TypeError("{} is not callable".format(repr(test)))
        if isinstance(test, type) and issubclass(test,
                                                 (case.TestCase, TestSuite)):
            raise TypeError("TestCases and TestSuites must be instantiated "
                            "before passing them to addTest()")
        self._tests.append(test)

    def addTests(self, tests):
        if isinstance(tests, str):
            raise TypeError("tests must be an iterable of tests, not a string")
        for test in tests:
            self.addTest(test)

    def run(self, result):
        for index, test in enumerate(self):
            if result.shouldStop:
                break
            test(result)
            if self._cleanup:
                self._removeTestAtIndex(index)
        return result

    def _removeTestAtIndex(self, index):
        """Stop holding a reference to the TestCase at index."""
        try:
            test = self._tests[index]
        except TypeError:
            # support for suite implementations that have overridden self._tests
            pass
        else:
            # Some unittest tests add non TestCase/TestSuite objects to
            # the suite.
            if hasattr(test, 'countTestCases'):
                self._removed_tests += test.countTestCases()
            self._tests[index] = None

    def __call__(self, *args, **kwds):
        return self.run(*args, **kwds)

    def debug(self):
        """Run the tests without collecting errors in a TestResult"""
        for test in self:
            test.debug()
```

#### 5、🤢测试用例--可运行对象



# 附录A：各种各样

## 一、《Python中的 @staticmethod@classmethod方法》

- https://zhuanlan.zhihu.com/p/28010894

Python面向对象编程中，类中定义的方法可以是 @classmethod 装饰的**类方法**，也可以是 @staticmethod 装饰的**静态方法**，用的最多的还是不带装饰器的**实例方法**，如果把这几个方法放一块，对初学者来说无疑是一头雾水，那我们该如何正确地使用它们呢？

先来看一个简单示例：

```python
class A(object):
    def m1(self, n):
        print("self:", self)

    @classmethod
    def m2(cls, n):
        print("cls:", cls)

    @staticmethod
    def m3(n):
        pass

a = A()
a.m1(1) # self: <__main__.A object at 0x000001E596E41A90>
A.m2(1) # cls: <class '__main__.A'>
A.m3(1)
```

我在类中一共定义了3个方法，m1 是实例方法，第一个参数必须是 self（约定俗成的）。m2 是类方法，第一个参数必须是cls（同样是约定俗成），m3 是静态方法，参数根据业务需求定，可有可无。当程序运行时，大概发生了这么几件事（结合下面的图来看）。

- 第一步：代码从第一行开始执行 class 命令，此时会创建一个类 A 对象（没错，类也是对象，一切皆对象嘛）同时初始化类里面的属性和方法，记住，此刻实例对象还没创建出来。
- 第二、三步：接着执行 a=A()，系统自动调用类的构造器，构造出实例对象 a
- 第四步：接着调用 a.m1(1) ，m1 是实例方法，内部会自动把实例对象传递给 self 参数进行绑定，也就是说， self 和 a 指向的都是同一个实例对象。
- 第五步：调用A.m2(1)时，python内部隐式地把类对象传递给 cls 参数，cls 和 A 都指向类对象。



![img](https://pic3.zhimg.com/80/v2-f5960c8e540c96a30c2513655b6ae9a2_720w.webp)



严格意义上来说，左边的都是变量名，是对象的引用，右边才是真正的对像，为了描述方便，我直接把 a 称为对象，你应该明白我说对象其实是它所引用右边的那个真正的对象。

再来看看每个方法各有什么特性

### 实例方法

```python3
print(A.m1)
# A.m1在py2中显示为<unbound method A.m1>
<function A.m1 at 0x000002BF7FF9A488>

print(a.m1)
<bound method A.m1 of <__main__.A object at 0x000002BF7FFA2BE0>>
```

A.m1是一个还没有绑定实例对象的方法，对于未绑定方法，调用 A.m1 时必须显示地传入一个实例对象进去，而 a.m1是已经绑定了实例的方法，python隐式地把对象传递给了self参数，所以不再手动传递参数，这是调用实例方法的过程。

```python3
A.m1(a, 1)
# 等价  
a.m1(1)
```

如果未绑定的方法 A.m1 不传实例对象给 self 时，就会报参数缺失错误，在 py3 与 py2 中，两者报的错误不一致，python2 要求第一个参数self是实例对象，而python3中可以是任意对象。

```python3
A.m1(1)
TypeError: m1() missing 1 required positional argument: 'n'
```

### 类方法

```python3
print(A.m2)
<bound method A.m2 of <class '__main__.A'>>

print(a.m2)
<bound method A.m2 of <class '__main__.A'>>
```

m2是类方法，不管是 A.m2 还是 a.m2，都是已经自动绑定了类对象A的方法，对于后者，因为python可以通过实例对象a找到它所属的类是A，找到A之后自动绑定到 cls。

```python
A.m2(1) 
 # 等价
 a.m2(1)
```

这使得我们可以在实例方法中通过使用 self.m2()这种方式来调用类方法和静态方法。

```python
def m1(self, n):
    print("self:", self)
    self.m2(n)
```

### 静态方法

```python
print(A.m3)
<function A.m3 at 0x000002BF7FF9A840>

print(a.m3)
<function A.m3 at 0x000002BF7FF9A840>
```

m3是类里面的一个静态方法，跟普通函数没什么区别，与类和实例都没有所谓的绑定关系，它只不过是碰巧存在类中的一个函数而已。不论是通过类还是实例都可以引用该方法。

~~~python
A.m3(1) 
 # 等价
 a.m3(1)
 ```

以上就是几个方法的基本介绍。现在把几个基本的概念理清楚了，那么现在来说说几个方法之间的使用场景以及他们之间的优缺点。

### 应用场景

静态方法的使用场景：

如果在方法中不需要访问任何实例方法和属性，纯粹地通过传入参数并返回数据的功能性方法，那么它就适合用静态方法来定义，它节省了实例化对象的开销成本，往往这种方法放在类外面的模块层作为一个函数存在也是没问题的，而放在类中，仅为这个类服务。例如下面是微信公众号开发中验证微信签名的一个例子，它没有引用任何类或者实例相关的属性和方法。

```python
from hashlib import sha1
import tornado.web

class SignatureHandler(tornado.web.RequestHandler):
    def get(self):
        """
         根据签名判断请求是否来自微信
        """
        signature = self.get_query_argument("signature", None)
        echostr = self.get_query_argument("echostr", None)
        timestamp = self.get_query_argument("timestamp", None)
        nonce = self.get_query_argument("nonce", None)
        if self._check_sign(TOKEN, timestamp, nonce, signature):
            logger.info("微信签名校验成功")
            self.write(echostr)
        else:
            self.write("你不是微信发过来的请求")

    @staticmethod
    def _check_sign(token, timestamp, nonce, signature):
        sign = [token, timestamp, nonce]
        sign.sort()
        sign = "".join(sign)
        sign = sha1(sign).hexdigest()
        return sign == signature
~~~

类方法的使用场景有：

作为工厂方法创建实例对象，例如内置模块 datetime.date 类中就有大量使用类方法作为工厂方法，以此来创建date对象。

```python
class date:

    def __new__(cls, year, month=None, day=None):
        self = object.__new__(cls)
        self._year = year
        self._month = month
        self._day = day
        return self

    @classmethod
    def fromtimestamp(cls, t):
        y, m, d, hh, mm, ss, weekday, jday, dst = _time.localtime(t)
        return cls(y, m, d)

    @classmethod
    def today(cls):
        t = _time.time()
        return cls.fromtimestamp(t)
```

如果希望在方法裡面调用静态类，那么把方法定义成类方法是合适的，因为要是定义成静态方法，那么你就要显示地引用类A，这对继承来说可不是一件好事情。

```python
class A:

    @staticmethod
    def m1()
        pass

    @staticmethod
    def m2():
        A.m1() # bad

    @classmethod
    def m3(cls):
        cls.m1() # good
```

其实也不算是什么深入理解吧，最多算是明白怎么用，真要深入理解恐怕还要另写一篇文章，有兴趣的可以去了解一下Python的描述符。
