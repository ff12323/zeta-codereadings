



书籍：

- 附录A-介绍：《Compilers: Principles, Techniques, and Tools》、《lex and yacc》

使用PLY的项目：

- [Pyrata.](https://github.com/nicolashernandez/PyRATA) A Python Rule-based feAture sTructure Analyzer.
- [pysmi](https://github.com/etingof/pysmi/). An SNMP MIB Parser.
- [pyfranca](https://github.com/zayfod/pyfranca). A Python module and tools for working with Franca interface definition language (IDL) models.
- [pycparser](https://github.com/eliben/pycparser). <font color="#F9DDA2">A C parser implemented in Python.</font>
- [cppheaderparser](http://sourceforge.net/projects/cppheaderparser/). A C++ header parser.
- [ZXBasic](http://www.zxbasic.net/). A cross compiler translates BASIC into Z80 assembler.
- [CGCC](http://tsemsb.blogspot.com/2010/04/cgcc-gcode-with-c-constructs.html). A C GCode compiler for industrial CNC machines
- [OE-lite.](http://oe-lite.org/) An embedded Linux build system.
- [ Add your project here by sending email to "dave" at "dabeaz.com" ]



## 代码阅读：基础知识

### 7、整数除法：`//` 运算符

```
m = (len(relist) // 2) + 1
```

解释：The `//` operator in Python is used to perform integer division. It returns the quotient of two integers, without any remainder.

### 4、`type(xxx)().join`

```python
    if isinstance(linfo.literals, (list, tuple)):
        lexobj.lexliterals = type(linfo.literals[0])().join(linfo.literals)
```

Bard：返回一个类型，再调用类型的静态方法。

The code snippet you provided is a Python code that checks if the `linfo.literals` variable is a list or tuple. If it is, then the code joins the elements of the list or tuple using the type of the first element of the list or tuple.

The `type(linfo.literals[0])()` line of code returns the type of the first element of the `linfo.literals` variable. In this case, the first element of the `linfo.literals` variable is a string, so the `type(linfo.literals[0])()` line of code will return the `str` type.

<font color="#F9DDA2">The `str` type has a `join()` method that takes a sequence of strings as input and returns a new string</font> that is the concatenation of the input strings. So, the `type(linfo.literals[0])().join(linfo.literals)` line of code will join the elements of the `linfo.literals` variable using the `str` type's `join()` method.

For example, if the `linfo.literals` variable is a list with the elements `"a"`, `"b"`, and `"c"`, then the `type(linfo.literals[0])().join(linfo.literals)` line of code will return the string `"abc"`.

### 3、`__doc__` 正则

```python
def _get_regex(func):
    return getattr(func, 'regex', func.__doc__)

def t_NUMBER(t):
    r'\d+'
    t.value = int(t.value)
    return t
```

解释：函数开始的docstring是正则表达式，会被获取而使用。

### 2、`(`**)` 字典聚合

```python
# -----------------------------------------------------------------------------
# get_caller_module_dict()
#
# This function returns a dictionary containing all of the symbols defined within
# a caller further down the call stack.  This is used to get the environment
# associated with the yacc() call if none was provided.
# -----------------------------------------------------------------------------
def get_caller_module_dict(levels):
    f = sys._getframe(levels)
    return { **f.f_globals, **f.f_locals }
```

========

> 文章：https://www.freecodecamp.org/news/python-merge-dictionaries-merging-two-dicts-in-python/
>
> You can use the double asterisk (also called double star) operator (`**`) to "unpack" and merge the key and value pairs of two or more dictionaries into a variable.
>
> Here's a code example:
>
> ```python
> devBio = {
>   "name": "Ihechikara",
>   "age": 500,
>   "language": "Python"
> }
> 
> tools = {
>   "dev environment": "JupyterLab",
>   "os": "Windows",
>   "visualization": "Matplotlib"
> }
> 
> merged_bio = { **devBio, **tools}
> 
> print(merged_bio)
> # {'name': 'Ihechikara', 'age': 500, 'language': 'Python', 'dev environment': 'JupyterLab', 'os': 'Windows', 'visualization': 'Matplotlib'}
> ```
>
> In the code above, we unpacked the `devBio` and `tools` dictionaries using the double asterisk operator: `{ **devBio, **tools}`.
>
> We then stored them in a variable called `merged_bio`.

### 1、`__code__`属性

问题：

```
                line = f.__code__.co_firstlineno
                file = f.__code__.co_filename 
                nargs = f.__code__.co_argcount
```

Bard：

The code snippet you provided assigns the following values to the variables `line`, `file`, and `nargs`:

- `line`: The line number in the source code where the function was defined.
- `file`: The path to the file where the function was defined.
- `nargs`: The number of arguments that the function takes.

These values are all stored in the `co_code` attribute of the function object. The `co_firstlineno` attribute stores the line number where the function was defined, the `co_filename` attribute stores the path to the file where the function was defined, and the `co_argcount` attribute stores the number of arguments that the function takes.

The `co_code` attribute is a special attribute that stores information about the function's source code. This information can be used for debugging or for getting information about the function's definition.

For example, the following code snippet shows how to use the `co_code` attribute to get the line number where a function was defined:

```python
def my_function(a, b, c):
    print("This is my function")

line = my_function.__code__.co_firstlineno
file = my_function.__code__.co_filename
nargs = my_function.__code__.co_argcount

print(f"The function is defined on line {line} of the file {file}")
print(f"The function takes {nargs} arguments")
```

打印值：

```
The function is defined on line 20 of the file d:\myProject\Demo_C_C++\python\textProcess\test\basic_dict_test.py
The function takes 3 arguments
```



> 





## 源码：Lex

### 3、辅助函数

_statetoken：

- 参数：1、一个以 `t_`开头的名字，2、一个字典其key是状态名。
- 返回值：解析s之后，(状态元组 + token名)
  - 默认值：状态，`'INITIAL'`
- 特殊：<font color="clan">（问：设计比较奇怪，但是算简单吗？？？？）</font>
  - token名与状态名互斥。。。（？？？）
  - ANY字符，表示状态字典中的任意状态。存在，则返回元组中包含所有。。。

```python
# -----------------------------------------------------------------------------
# def _statetoken(s,names)
#
# Given a declaration name s of the form "t_" and a dictionary whose keys are
# state names, this function returns a tuple (states,tokenname) where states
# is a tuple of state names and tokenname is the name of the token.  For example,
# calling this with s = "t_foo_bar_SPAM" might return (('foo','bar'),'SPAM')
# -----------------------------------------------------------------------------
def _statetoken(s, names):
    parts = s.split('_')
    for i, part in enumerate(parts[1:], 1):
        if part not in names and part != 'ANY':
            break

    if i > 1:
        states = tuple(parts[1:i])
    else:
        states = ('INITIAL',)

    if 'ANY' in states:
        states = tuple(names)

    tokenname = '_'.join(parts[i:])
    return (states, tokenname)
```

### 2、反射解析：LexerReflect

作用：从一个用户的输入文件中，提取出来的信息。将构建一个lexer所需要的提取出来。

- `ldict`： 就是通过获取栈帧的全局变量，和局部变量的输入。要进行筛选提取。
- `log`：默认提供了一个极其简单的Log实现，以stream为构造。也可以使用logging模块，接口相同。

```python
# -----------------------------------------------------------------------------
# LexerReflect()
#
# This class represents information needed to build a lexer as extracted from a
# user's input file.
# -----------------------------------------------------------------------------
class LexerReflect(object):
    def __init__(self, ldict, log=None, reflags=0):
        self.ldict      = ldict
        self.error_func = None
        self.tokens     = []
        self.reflags    = reflags
        self.stateinfo  = {'INITIAL': 'inclusive'}
        self.modules    = set()
        self.error      = False
        self.log        = PlyLogger(sys.stderr) if log is None else log
```

#### get与valid

作用：获取信息，校验信息。

- 关键变量名：
  - `tokens`变量：必须定义且不空。类型校验：list或tuple；每一个都必须是标识符。
  - `literals`变量：校验：必须是一个可迭代的对象（list，或 字符串序列等），每一个元素必须是单个字符。。。（默认值：空字符串）
  - `states`变量：类型校验：必须是有一个列表或者元组。 元素校验：`(statename,'exclusive|inclusive')`，必须是一个元组，且状态只有2个。名字不能重复。
  - rules：见下面的小节。

```python
    # Get all of the basic information
    def get_all(self):
        self.get_tokens()
        self.get_literals()
        self.get_states()
        self.get_rules()

    # Validate all of the information
    def validate_all(self):
        self.validate_tokens()
        self.validate_literals()
        self.validate_rules()
        return self.error
```



#### get_rules

大概：

- 以 `t_`开头的符号（变量名、函数名等），属于一种token类型（需要映射）
- 不同状态、不同类型都映射着一种函数列表、字符列表等。

```python
    # Get all of the symbols with a t_ prefix and sort them into various
    # categories (functions, strings, error functions, and ignore characters)

    def get_rules(self):
        tsymbols = [f for f in self.ldict if f[:2] == 't_']

        # Now build up a list of functions and a list of strings
        self.toknames = {}        # Mapping of symbols to token names
        self.funcsym  = {}        # Symbols defined as functions
        self.strsym   = {}        # Symbols defined as strings
        self.ignore   = {}        # Ignore strings by state
        self.errorf   = {}        # Error functions by state
        self.eoff     = {}        # EOF functions by state

        for s in self.stateinfo:
            self.funcsym[s] = []
            self.strsym[s] = []

        if len(tsymbols) == 0:
            self.log.error('No rules of the form t_rulename are defined')
            self.error = True
            return

        for f in tsymbols:
            t = self.ldict[f]
            states, tokname = _statetoken(f, self.stateinfo)
            self.toknames[f] = tokname

            if hasattr(t, '__call__'):
                if tokname == 'error':
                    for s in states:
                        self.errorf[s] = t
                elif tokname == 'eof':
                    for s in states:
                        self.eoff[s] = t
                elif tokname == 'ignore':
                    line = t.__code__.co_firstlineno
                    file = t.__code__.co_filename
                    self.log.error("%s:%d: Rule %r must be defined as a string", file, line, t.__name__)
                    self.error = True
                else:
                    for s in states:
                        self.funcsym[s].append((f, t))
            elif isinstance(t, StringTypes):
                if tokname == 'ignore':
                    for s in states:
                        self.ignore[s] = t
                    if '\\' in t:
                        self.log.warning("%s contains a literal backslash '\\'", f)

                elif tokname == 'error':
                    self.log.error("Rule %r must be defined as a function", f)
                    self.error = True
                else:
                    for s in states:
                        self.strsym[s].append((f, t))
            else:
                self.log.error('%s not defined as a function or string', f)
                self.error = True

        # Sort the functions by line number
        for f in self.funcsym.values():
            f.sort(key=lambda x: x[1].__code__.co_firstlineno)

        # Sort the strings by regular expression length
        for s in self.strsym.values():
            s.sort(key=lambda x: len(x[1]), reverse=True)

```

#### validate_rules

大概：校验所有的规则。

- 1、校验函数规则：
  - 函数参数个数校验：如果是函数是一个类的方法，则参数要<=2个（包含self）；如果不是则必须<=1个。

```python
    # Validate all of the t_rules collected
    def validate_rules(self):
        for state in self.stateinfo:
            # Validate all rules defined by functions

            for fname, f in self.funcsym[state]:
                line = f.__code__.co_firstlineno
                file = f.__code__.co_filename
                module = inspect.getmodule(f)
                self.modules.add(module)

                tokname = self.toknames[fname]
                if isinstance(f, types.MethodType):
                    reqargs = 2
                else:
                    reqargs = 1
                nargs = f.__code__.co_argcount
                if nargs > reqargs:
                    self.log.error("%s:%d: Rule %r has too many arguments", file, line, f.__name__)
                    self.error = True
                    continue

                if nargs < reqargs:
                    self.log.error("%s:%d: Rule %r requires an argument", file, line, f.__name__)
                    self.error = True
                    continue

                if not _get_regex(f):
                    self.log.error("%s:%d: No regular expression defined for rule %r", file, line, f.__name__)
                    self.error = True
                    continue

                try:
                    c = re.compile('(?P<%s>%s)' % (fname, _get_regex(f)), self.reflags)
                    if c.match(''):
                        self.log.error("%s:%d: Regular expression for rule %r matches empty string", file, line, f.__name__)
                        self.error = True
                except re.error as e:
                    self.log.error("%s:%d: Invalid regular expression for rule '%s'. %s", file, line, f.__name__, e)
                    if '#' in _get_regex(f):
                        self.log.error("%s:%d. Make sure '#' in rule %r is escaped with '\\#'", file, line, f.__name__)
                    self.error = True

            # Validate all rules defined by strings
            for name, r in self.strsym[state]:
                tokname = self.toknames[name]
                if tokname == 'error':
                    self.log.error("Rule %r must be defined as a function", name)
                    self.error = True
                    continue

                if tokname not in self.tokens and tokname.find('ignore_') < 0:
                    self.log.error("Rule %r defined for an unspecified token %s", name, tokname)
                    self.error = True
                    continue

                try:
                    c = re.compile('(?P<%s>%s)' % (name, r), self.reflags)
                    if (c.match('')):
                        self.log.error("Regular expression for rule %r matches empty string", name)
                        self.error = True
                except re.error as e:
                    self.log.error("Invalid regular expression for rule %r. %s", name, e)
                    if '#' in r:
                        self.log.error("Make sure '#' in rule %r is escaped with '\\#'", name)
                    self.error = True

            if not self.funcsym[state] and not self.strsym[state]:
                self.log.error("No rules defined for state %r", state)
                self.error = True

            # Validate the error function
            efunc = self.errorf.get(state, None)
            if efunc:
                f = efunc
                line = f.__code__.co_firstlineno
                file = f.__code__.co_filename
                module = inspect.getmodule(f)
                self.modules.add(module)

                if isinstance(f, types.MethodType):
                    reqargs = 2
                else:
                    reqargs = 1
                nargs = f.__code__.co_argcount
                if nargs > reqargs:
                    self.log.error("%s:%d: Rule %r has too many arguments", file, line, f.__name__)
                    self.error = True

                if nargs < reqargs:
                    self.log.error("%s:%d: Rule %r requires an argument", file, line, f.__name__)
                    self.error = True

        for module in self.modules:
            self.validate_module(module)

```



### 1、lex函数

全局函数：把提供的模块，将其中所有定义的正则表达式规则，进行构建。

- 返回值：lex对象。

```python
# -----------------------------------------------------------------------------
# lex(module)
#
# Build all of the regular expression rules from definitions in the supplied module
# -----------------------------------------------------------------------------
def lex(*, module=None, object=None, debug=False, 
        reflags=int(re.VERBOSE), debuglog=None, errorlog=None):
    
    lexobj = Lexer()
    
    return lexobj
```







《ReadMe》

介绍：

PLY is a zero-dependency Python implementation of the traditional parsing tools lex and yacc. It uses the same LALR(1) parsing algorithm as yacc and has most of its core features. It is compatible with all modern versions of Python.

PLY was originally created in 2001 to support an Introduction to Compilers course at the University of Chicago. As such, it has almost no features other than the core LALR(1) parsing algorithm. This is by design--students should be made to suffer. Well, at least a little bit. However, from a more practical point of view, there is a lot flexibility in terms of how you decide to use it. You can use PLY to build **Abstract Syntax Trees (ASTs)**, simple one-pass compilers, protocol decoders, or even a more advanced parsing framework.



# 附录A：《Python Lex Yacc 手册》

- https://doc.yonyoucloud.com/doc/wiki/project/python-lex-yacc/index.html

如果你从事编译器或解析器的开发工作，你可能对lex和yacc不会陌生，[PLY](http://www.dabeaz.com/ply/)是David Beazley实现的基于Python的lex和yacc。作者最著名的成就可能是其撰写的Python Cookbook, 3rd Edition。

本文指导你使用 PLY 进行词法分析和语法解析的，鉴于解析本身是个复杂性的事情，在你使用 PLY 投入大规模的开发前，我强烈建议你完整地阅读或者浏览本文档。

PLY-3.0 能同时兼容 Python2 和 Python3。需要注意的是，对于 Python3 的支持是新加入的，还没有广泛的测试（尽管所有的例子和单元测试都能够在 Python3 下通过）。如果你使用的是 Python2，应该使用 Python2.4 以上版本，虽然 PLY 最低能够支持到 Python2.2，不过一些可选的功能需要新版本模块的支持。

版本信息

书中演示代码基于以下版本：

| 语言/框架 | 版本信息               |
| :-------- | :--------------------- |
| Python    | 支持Python2.4 以上版本 |

> **致谢**
> 原文：http://www.dabeaz.com/ply/
>
> 译者：http://www.pchou.info/open-source/2014/01/18/52da47204d4cb.html

| 英                          | 译                                    |
| :-------------------------- | :------------------------------------ |
| token                       | 标记                                  |
| context free grammar        | 上下文无关文法                        |
| syntax directed translation | 语法制导的翻译                        |
| ambiguity                   | 二义                                  |
| terminals                   | 终结符                                |
| non-terminals               | 非终结符                              |
| documentation string        | 文档字符串（python中的`_docstring_`） |
| shift-reduce                | 移进-归约                             |
| Empty Productions           | 空产生式                              |
| Panic mode recovery         | 悲观恢复模式                          |

## 介绍

PLY 是纯粹由 Python 实现的 Lex 和 yacc（流行的编译器构建工具）。PLY 的设计目标是尽可能的沿袭传统 lex 和 yacc 工具的工作方式，包括支持 LALR(1)分析法、提供丰富的输入验证、错误报告和诊断。因此，如果你曾经在其他编程语言下使用过 yacc，你应该能够很容易的迁移到 PLY 上。

2001 年，我在芝加哥大学教授“编译器简介”课程时开发了的早期的 PLY。学生们使用 Python 和 PLY 构建了一个类似 Pascal 的语言的完整编译器，其中的语言特性包括：词法分析、语法分析、类型检查、类型推断、嵌套作用域，并针对 SPARC 处理器生成目标代码等。最终他们大约实现了 30 种不同的编译器！PLY 在接口设计上影响使用的问题也被学生们所提出。从 2001 年以来，PLY 继续从用户的反馈中不断改进。为了适应对未来的改进需求，<font color="clan">PLY3.0 在原来基础上进行了重大的重构</font>。

由于 PLY 是作为教学工具来开发的，你会发现它对于标记和语法规则是相当严谨的，这一定程度上是为了帮助新手用户找出常见的编程错误。不过，高级用户也会发现这有助于处理真实编程语言的复杂语法。还需要注意的是，PLY 没有提供太多花哨的东西（例如，自动构建抽象语法树和遍历树），我也不认为它是个分析框架。相反，你会发现它是一个用 Python 实现的，基本的，但能够完全胜任的 lex/yacc。

本文的假设你多少熟悉分析理论、语法制导的翻译、基于其他编程语言使用过类似 lex 和 yacc 的编译器构建工具。如果你对这些东西不熟悉，你可能需要先去一些书籍中学习一些基础，比如：Aho, Sethi 和 Ullman 的《Compilers: Principles, Techniques, and Tools》（《编译原理》），和 O'Reilly' 出版的J ohn Levine 的《lex and yacc》。事实上，《lex and yacc》和 PLY 使用的概念几乎相同。

## PLY 概要

PLY 包含两个独立的模块：lex.py 和 yacc.py，都定义在 ply 包下。lex.py 模块用来<font color="#F9DDA2">将输入字符通过一系列的正则表达式分解成标记序列</font>，yacc.py 通过一些上下文无关的文法来识别编程语言语法。yacc.py **使用 LR 解析法，并使用 LALR(1)算法（默认）**或者 SLR 算法生成分析表。

这两个工具是为了一起工作的。lex.py <u>通过向外部提供`token()`方法作为接口，方法每次会从输入中返回下一个有效的标记。yacc.py 将会不断的调用这个方法来获取标记并匹配语法规则</u>。yacc.py 的功能通常是生成抽象语法树(`AST`)，不过，这完全取决于用户，如果需要，yacc.py 可以直接用来完成简单的翻译。

就像相应的 unix 工具，yacc.py 提供了大多数你期望的特性，其中包括：丰富的错误检查、语法验证、支持空产生式、错误的标记、通过优先级规则解决二义性。事实上，传统 yacc 能够做到的 PLY 都应该支持。

yacc.py 与 Unix 下的 yacc 的主要不同之处在于，yacc.py 没有包含一个独立的代码生成器，而是在 PLY 中依赖反射来构建词法分析器和语法解析器。不像传统的 lex/yacc 工具需要一个独立的输入文件，并将之转化成一个源文件，Python 程序必须是一个可直接可用的程序，这意味着不能有额外的源文件和特殊的创建步骤（像是那种执行 yacc 命令来生成 Python 代码）。又由于生成分析表开销较大，PLY 会缓存生成的分析表，并将它们保存在独立的文件中，除非源文件有变化，会重新生成分析表，否则将从缓存中直接读取。

## Lex

`lex.py`是用来将输入字符串标记化。例如，假设你正在设计一个编程语言，用户的输入字符串如下：

```python
x = 3 + 42 * (s - t)
```

标记器将字符串分割成独立的标记：

```python
'x','=', '3', '+', '42', '*', '(', 's', '-', 't', ')'
```

标记通常用一组名字来命名和表示：

```python
'ID','EQUALS','NUMBER','PLUS','NUMBER','TIMES','LPAREN','ID','MINUS','ID','RPAREN'
```

将标记名和标记值本身组合起来：

```python
('ID','x'), ('EQUALS','='), ('NUMBER','3'),('PLUS','+'), ('NUMBER','42), ('TIMES','*'),('LPAREN','('), ('ID','s'),('MINUS','-'),('ID','t'), ('RPAREN',')
```

正则表达式是描述标记规则的典型方法，下一节展示如何用 lex.py 实现。

### Lex 的例子

下面的例子展示了如何使用 lex.py 对输入进行标记

```python
# ------------------------------------------------------------
# calclex.py
#
# tokenizer for a simple expression evaluator for
# numbers and +,-,*,/
# ------------------------------------------------------------
import ply.lex as lex

# List of token names.   This is always required
tokens = (
   'NUMBER',
   'PLUS',
   'MINUS',
   'TIMES',
   'DIVIDE',
   'LPAREN',
   'RPAREN',
)

# Regular expression rules for simple tokens
t_PLUS    = r'\+'
t_MINUS   = r'-'
t_TIMES   = r'\*'
t_DIVIDE  = r'/'
t_LPAREN  = r'\('
t_RPAREN  = r'\)'

# A regular expression rule with some action code
def t_NUMBER(t):
    r'\d+'
    t.value = int(t.value)    
    return t

# Define a rule so we can track line numbers
def t_newline(t):
    r'\n+'
    t.lexer.lineno += len(t.value)

# A string containing ignored characters (spaces and tabs)
t_ignore  = ' \t'

# Error handling rule
def t_error(t):
    print "Illegal character '%s'" % t.value[0]
    t.lexer.skip(1)

# Build the lexer
lexer = lex.lex()
```

为了使 lexer 工作，你需要给定一个输入，并传递给`input()`方法。然后，重复调用`token()`方法来获取标记序列，下面的代码展示了这种用法：

```python
# Test it out
data = '''
3 + 4 * 10
  + -20 *2
'''

# Give the lexer some input
lexer.input(data)

# Tokenize
while True:
    tok = lexer.token()
    if not tok: break      # No more input
    print tok
```

程序执行，将给出如下输出：

```
$ python example.py
LexToken(NUMBER,3,2,1)
LexToken(PLUS,'+',2,3)
LexToken(NUMBER,4,2,5)
LexToken(TIMES,'*',2,7)
LexToken(NUMBER,10,2,10)
LexToken(PLUS,'+',3,14)
LexToken(MINUS,'-',3,16)
LexToken(NUMBER,20,3,18)
LexToken(TIMES,'*',3,20)
LexToken(NUMBER,2,3,21)
```

Lexers 也同时支持迭代，你可以把上面的循环写成这样：

```python
for tok in lexer:
    print tok
```

由 lexer.token()方法返回的标记是 LexToken 类型的实例，拥有`tok.type`,`tok.value`,`tok.lineno`和`tok.lexpos`属性，下面的代码展示了如何访问这些属性：

```python
# Tokenize
while True:
    tok = lexer.token()
    if not tok: break      # No more input
    print tok.type, tok.value, tok.line, tok.lexpos
```

`tok.type`和`tok.value`属性表示标记本身的类型和值。`tok.line`和`tok.lexpos`属性包含了标记的位置信息，`tok.lexpos`表示标记相对于输入串起始位置的偏移。

### 标记列表

词法分析器必须提供一个标记的列表，这个列表将所有可能的标记告诉分析器，用来执行各种验证，同时也提供给 yacc.py 作为终结符。

在上面的例子中，是这样给定标记列表的：

```
tokens = (
   'NUMBER',
   'PLUS',
   'MINUS',
   'TIMES',
   'DIVIDE',
   'LPAREN',
   'RPAREN',
)
```

### 标记的规则

每种标记用一个正则表达式规则来表示，每个规则需要以"t_"开头声明，表示该声明是对标记的规则定义。对于简单的标记，可以定义成这样（在 Python 中使用 raw string 能比较方便的书写正则表达式）：

```python
t_PLUS = r'\+'
```

这里，紧跟在 t_ 后面的单词，必须跟标记列表中的某个标记名称对应。如果需要执行动作的话，规则可以写成一个方法。例如，下面的规则匹配数字字串，并且将匹配的字符串转化成 Python 的整型：

```python
def t_NUMBER(t):
    r'\d+'
    t.value = int(t.value)
    return t
```

如果使用方法的话，正则表达式写成方法的文档字符串。方法总是需要接受一个 LexToken 实例的参数，该实例有一个 t.type 的属性（字符串表示）来表示标记的类型名称，t.value 是标记值（匹配的实际的字符串），t.lineno 表示当前在源输入串中的作业行，t.lexpos 表示标记相对于输入串起始位置的偏移。默认情况下，t.type 是以t_开头的变量或方法的后面部分。方法可以在方法体里面修改这些属性。但是，如果这样做，应该返回结果 token，否则，标记将被丢弃。

在 lex 内部，lex.py 用`re`模块处理模式匹配，在构造最终的完整的正则式的时候，用户提供的规则按照下面的顺序加入：

1. 所有由方法定义的标记规则，按照他们的出现顺序依次加入
2. 由字符串变量定义的标记规则按照其正则式长度倒序后，依次加入（长的先入）
3. <font color="#F9DDA2">顺序的约定对于精确匹配是必要的。比如，如果你想区分‘=’和‘==’，你需要确保‘==’优先检查</font>。如果用字符串来定义这样的表达式的话，通过将较长的正则式先加入，可以帮助解决这个问题。用方法定义标记，可以显示地控制哪个规则优先检查。

为了处理保留字，你应该写一个单一的规则来匹配这些标识，并在方法里面作特殊的查询：

```python
reserved = {
   'if' : 'IF',
   'then' : 'THEN',
   'else' : 'ELSE',
   'while' : 'WHILE',
   ...
}

tokens = ['LPAREN','RPAREN',...,'ID'] + list(reserved.values())

def t_ID(t):
    r'[a-zA-Z_][a-zA-Z_0-9]*'
    t.type = reserved.get(t.value,'ID')    # Check for reserved words
    return t
```

这样做可以大大减少正则式的个数，并稍稍加快处理速度。注意：你应该避免为保留字编写单独的规则，例如，如果你像下面这样写：

```python
t_FOR   = r'for'
t_PRINT = r'print'
```

但是，这些规则照样也能够匹配以这些字符开头的单词，比如'forget'或者'printed'，这通常不是你想要的。

### 标记的值

标记被 lex 返回后，它们的值被保存在`value`属性中。正常情况下，value 是匹配的实际文本。事实上，value 可以被赋为任何 Python 支持的类型。例如，当扫描到标识符的时候，你可能不仅需要返回标识符的名字，还需要返回其在符号表中的位置，可以像下面这样写：

```python
def t_ID(t):
    ...
    # Look up symbol table information and return a tuple
    t.value = (t.value, symbol_lookup(t.value))
    ...
    return t
```

需要注意的是，不推荐用其他属性来保存值，因为 yacc.py 模块只会暴露出标记的 value属 性，访问其他属性会变得不自然。如果想保存多种属性，可以将元组、字典、或者对象实例赋给 value。

### 丢弃标记

想丢弃像注释之类的标记，只要不返回 value 就行了，像这样：

```python
def t_COMMENT(t):
    r'\#.*'
    pass
    # No return value. Token discarded
```

为标记声明添加"ignore_"前缀同样可以达到目的：

```python
t_ignore_COMMENT = r'\#.*'
```

如果有多种文本需要丢弃，建议使用方法来定义规则，因为方法能够提供更精确的匹配优先级控制（方法根据出现的顺序，而字符串的正则表达式依据正则表达式的长度）

### 行号和位置信息

默认情况下，lex.py 对行号一无所知。因为 lex.py 根本不知道何为"行"的概念（换行符本身也作为文本的一部分）。不过，可以通过写一个特殊的规则来记录行号：

```python
# Define a rule so we can track line numbers
def t_newline(t):
    r'\n+'
    t.lexer.lineno += len(t.value)
```

在这个规则中，当前 lexer 对象 t.lexer 的 lineno 属性被修改了，而且空行被简单的丢弃了，因为没有任何的返回。

lex.py 也不自动做列跟踪。但是，位置信息被记录在了每个标记对象的`lexpos`属性中，这样，就有可能来计算列信息了。例如：每当遇到新行的时候就重置列值：

```python
# Compute column. 
#     input is the input text string
#     token is a token instance
def find_column(input,token):
    last_cr = input.rfind('\n',0,token.lexpos)
    if last_cr < 0:
        last_cr = 0
    column = (token.lexpos - last_cr) + 1
    return column
```

通常，计算列的信息是为了指示上下文的错误位置，所以只在必要时有用。

### 忽略字符

`t_ignore`规则比较特殊，是lex.py所保留用来忽略字符的，通常用来跳过空白或者不需要的字符。虽然可以通过定义像`t_newline()`这样的规则来完成相同的事情，不过使用t_ignore能够提供较好的词法分析性能，因为相比普通的正则式，它被特殊化处理了。

### 字面字符

字面字符可以通过在词法模块中定义一个`literals`变量做到，例如：

```python
literals = [ '+','-','*','/' ]
```

或者

```python
literals = "+-*/"
```

字面字符是指单个字符，表示把字符本身作为标记，标记的`type`和`value`都是字符本身。不过，字面字符是在其他正则式之后被检查的，因此如果有规则是以这些字符开头的，那么这些规则的优先级较高。

### 错误处理

最后，在词法分析中遇到非法字符时，`t_error()`用来处理这类错误。这种情况下，`t.value`包含了余下还未被处理的输入字串，在之前的例子中，错误处理方法是这样的：

```python
# Error handling rule
def t_error(t):
    print "Illegal character '%s'" % t.value[0]
    t.lexer.skip(1)
```

这个例子中，我们只是简单的输出不合法的字符，并且通过调用`t.lexer.skip(1)`跳过一个字符。

### 构建和使用 lexer

函数`lex.lex()`使用 Python 的反射机制读取调用上下文中的正则表达式，来创建 lexer。lexer 一旦创建好，有两个方法可以用来控制 lexer 对象：

- `lexer.input(data)` 重置 lexer 和输入字串
- `lexer.token()` 返回下一个 LexToken 类型的标记实例，如果进行到输入字串的尾部时将返回`None`

推荐直接在 lex() 函数返回的 lexer 对象上调用上述接口，尽管也可以向下面这样用模块级别的 lex.input() 和 lex.token()：

```python
lex.lex()
lex.input(sometext)
while 1:
    tok = lex.token()
    if not tok: break
    print tok
```

在这个例子中，lex.input() 和 lex.token() 是模块级别的方法，在 lex 模块中，input() 和 token() 方法绑定到最新创建的 lexer 对象的对应方法上。最好不要这样用，因为这种接口可能不知道在什么时候就失效（译者注：垃圾回收？）

### @TOKEN 装饰器

在一些应用中，你可能需要定义一系列辅助的记号来构建复杂的正则表达式，例如：

```python
digit            = r'([0-9])'
nondigit         = r'([_A-Za-z])'
identifier       = r'(' + nondigit + r'(' + digit + r'|' + nondigit + r')*)'        

def t_ID(t):
    # want docstring to be identifier above. ?????
    ...
```

在这个例子中，我们希望 ID 的规则引用上面的已有的变量。然而，使用文档字符串无法做到，为了解决这个问题，你可以使用`@TOKEN`装饰器：

```python
from ply.lex import TOKEN

@TOKEN(identifier)
def t_ID(t):
    ...
```

装饰器可以将 identifier 关联到 t_ID() 的文档字符串上以使 lex.py 正常工作，一种等价的做法是直接给文档字符串赋值：

```python
def t_ID(t):
    ...

t_ID.__doc__ = identifier
```

注意：@TOKEN 装饰器需要 Python-2.4 以上的版本。如果你在意老版本Python的兼容性问题，使用上面的等价办法。

### 优化模式

为了提高性能，你可能希望使用 Python 的优化模式（比如，使用 -o 选项执行 Python）。然而，这样的话，Python 会忽略文档字串，这是 lex.py 的特殊问题，可以通过在创建 lexer 的时候使用 optimize 选项：

```python
lexer = lex.lex(optimize=1)
```

接着，用 Python 常规的模式运行，这样，lex.py 会在当前目录下创建一个 lextab.py 文件，这个文件会包含所有的正则表达式规则和词法分析阶段的分析表。然后，lextab.py 可以被导入用来构建 lexer。这种方法大大改善了词法分析程序的启动时间，而且可以在 Python 的优化模式下工作。

想要更改生成的文件名，使用如下参数：

```python
lexer = lex.lex(optimize=1,lextab="footab")
```

在优化模式下执行，需要注意的是 lex 会被禁用大多数的错误检查。因此，建议只在确保万事俱备准备发布最终代码时使用。

### 调试

如果想要调试，可以使 lex() 运行在调试模式：

```python
lexer = lex.lex(debug=1)
```

这将打出一些调试信息，包括添加的规则、最终的正则表达式和词法分析过程中得到的标记。

除此之外，lex.py 有一个简单的主函数，不但支持对命令行参数输入的字串进行扫描，还支持命令行参数指定的文件名：

```python
if __name__ == '__main__':
     lex.runmain()
```

想要了解高级调试的详情，请移步至最后的高级调试部分。

### 其他方式定义词法规则

上面的例子，词法分析器都是在单个的 Python 模块中指定的。如果你想将标记的规则放到不同的模块，使用 module 关键字参数。例如，你可能有一个专有的模块，包含了标记的规则：

```python
# module: tokrules.py
# This module just contains the lexing rules

# List of token names.   This is always required
tokens = (
   'NUMBER',
   'PLUS',
   'MINUS',
   'TIMES',
   'DIVIDE',
   'LPAREN',
   'RPAREN',
)

# Regular expression rules for simple tokens
t_PLUS    = r'\+'
t_MINUS   = r'-'
t_TIMES   = r'\*'
t_DIVIDE  = r'/'
t_LPAREN  = r'\('
t_RPAREN  = r'\)'

# A regular expression rule with some action code
def t_NUMBER(t):
    r'\d+'
    t.value = int(t.value)    
    return t

# Define a rule so we can track line numbers
def t_newline(t):
    r'\n+'
    t.lexer.lineno += len(t.value)

# A string containing ignored characters (spaces and tabs)
t_ignore  = ' \t'

# Error handling rule
def t_error(t):
    print "Illegal character '%s'" % t.value[0]
    t.lexer.skip(1)
```

现在，如果你想要从不同的模块中构建分析器，应该这样（在交互模式下）：

```python
>>> import tokrules
>>> lexer = lex.lex(module=tokrules)
>>> lexer.input("3 + 4")
>>> lexer.token()
LexToken(NUMBER,3,1,1,0)
>>> lexer.token()
LexToken(PLUS,'+',1,2)
>>> lexer.token()
LexToken(NUMBER,4,1,4)
>>> lexer.token()
None
```

`module`选项也可以指定类型的实例，例如：

```python
import ply.lex as lex

class MyLexer:
    # List of token names.   This is always required
    tokens = (
       'NUMBER',
       'PLUS',
       'MINUS',
       'TIMES',
       'DIVIDE',
       'LPAREN',
       'RPAREN',
    )

    # Regular expression rules for simple tokens
    t_PLUS    = r'\+'
    t_MINUS   = r'-'
    t_TIMES   = r'\*'
    t_DIVIDE  = r'/'
    t_LPAREN  = r'\('
    t_RPAREN  = r'\)'

    # A regular expression rule with some action code
    # Note addition of self parameter since we're in a class
    def t_NUMBER(self,t):
        r'\d+'
        t.value = int(t.value)    
        return t

    # Define a rule so we can track line numbers
    def t_newline(self,t):
        r'\n+'
        t.lexer.lineno += len(t.value)

    # A string containing ignored characters (spaces and tabs)
    t_ignore  = ' \t'

    # Error handling rule
    def t_error(self,t):
        print "Illegal character '%s'" % t.value[0]
        t.lexer.skip(1)

    # Build the lexer
    def build(self,**kwargs):
        self.lexer = lex.lex(module=self, **kwargs)

    # Test it output
    def test(self,data):
        self.lexer.input(data)
        while True:
             tok = lexer.token()
             if not tok: break
             print tok

# Build the lexer and try it out
m = MyLexer()
m.build()           # Build the lexer
m.test("3 + 4")     # Test it
```

当从类中定义 lexer，你需要创建类的实例，而不是类本身。这是因为，lexer 的方法只有被绑定（bound-methods）对象后才能使 PLY 正常工作。

当给 lex() 方法使用 module 选项时，PLY 使用`dir()`方法，从对象中获取符号信息，因为不能直接访问对象的`__dict__`属性。（译者注：可能是因为兼容性原因，**dict**这个方法可能不存在）

最后，如果你希望保持较好的封装性，但不希望什么东西都写在类里面，lexers 可以在闭包中定义，例如：

```python
import ply.lex as lex

# List of token names.   This is always required
tokens = (
  'NUMBER',
  'PLUS',
  'MINUS',
  'TIMES',
  'DIVIDE',
  'LPAREN',
  'RPAREN',
)

def MyLexer():
    # Regular expression rules for simple tokens
    t_PLUS    = r'\+'
    t_MINUS   = r'-'
    t_TIMES   = r'\*'
    t_DIVIDE  = r'/'
    t_LPAREN  = r'\('
    t_RPAREN  = r'\)'

    # A regular expression rule with some action code
    def t_NUMBER(t):
        r'\d+'
        t.value = int(t.value)    
        return t

    # Define a rule so we can track line numbers
    def t_newline(t):
        r'\n+'
        t.lexer.lineno += len(t.value)

    # A string containing ignored characters (spaces and tabs)
    t_ignore  = ' \t'

    # Error handling rule
    def t_error(t):
        print "Illegal character '%s'" % t.value[0]
        t.lexer.skip(1)

    # Build the lexer from my environment and return it    
    return lex.lex()
```

### 额外状态维护

在你的词法分析器中，你可能想要维护一些状态。这可能包括模式设置，符号表和其他细节。例如，假设你想要跟踪`NUMBER`标记的出现个数。

一种方法是维护一个全局变量：

```python
num_count = 0
def t_NUMBER(t):
    r'\d+'
    global num_count
    num_count += 1
    t.value = int(t.value)    
    return t
```

如果你不喜欢全局变量，另一个记录信息的地方是 lexer 对象内部。可以通过当前标记的 lexer 属性访问：

```python
def t_NUMBER(t):
    r'\d+'
    t.lexer.num_count += 1     # Note use of lexer attribute
    t.value = int(t.value)    
    return t

lexer = lex.lex()
lexer.num_count = 0            # Set the initial count
```

上面这样做的优点是当同时存在多个 lexer 实例的情况下，简单易行。不过这看上去似乎是严重违反了面向对象的封装原则。lexer 的内部属性（除了 lineno ）都是以 lex 开头命名的（lexdata、lexpos）。因此，只要不以 lex 开头来命名属性就很安全的。

如果你不喜欢给 lexer 对象赋值，你可以自定义你的 lexer 类型，就像前面看到的那样：

```python
class MyLexer:
    ...
    def t_NUMBER(self,t):
        r'\d+'
        self.num_count += 1
        t.value = int(t.value)    
        return t

    def build(self, **kwargs):
        self.lexer = lex.lex(object=self,**kwargs)

    def __init__(self):
        self.num_count = 0
```

如果你的应用会创建很多 lexer 的实例，并且需要维护很多状态，上面的类可能是最容易管理的。

状态也可以用闭包来管理，比如，在 Python3 中：

```python
def MyLexer():
    num_count = 0
    ...
    def t_NUMBER(t):
        r'\d+'
        nonlocal num_count
        num_count += 1
        t.value = int(t.value)    
        return t
    ...
```

### Lexer 克隆

如果有必要的话，lexer 对象可以通过`clone()`方法来复制：

```python
lexer = lex.lex()
...
newlexer = lexer.clone()
```

当 lexer 被克隆后，复制品能够精确的保留输入串和内部状态，不过，新的 lexer 可以接受一个不同的输出字串，并独立运作起来。这在几种情况下也许有用：当你在编写的解析器或编译器涉及到递归或者回退处理时，你需要扫描先前的部分，你可以clone并使用复制品，或者你在实现某种预编译处理，可以 clone 一些 lexer 来处理不同的输入文件。

创建克隆跟重新调用 lex.lex() 的不同点在于，PLY 不会重新构建任何的内部分析表或者正则式。当 lexer 是用类或者闭包创建的，需要注意类或闭包本身的的状态。换句话说你要注意新创建的 lexer 会共享原始 lexer 的这些状态，比如：

```python
m = MyLexer()
a = lex.lex(object=m)      # Create a lexer

b = a.clone()              # Clone the lexer
```

### Lexer 的内部状态

lexer 有一些内部属性在特定情况下有用：

- `lexer.lexpos`。这是一个表示当前分析点的位置的整型值。如果你修改这个值的话，这会改变下一个 token() 的调用行为。在标记的规则方法里面，这个值表示紧跟匹配字串后面的第一个字符的位置，如果这个值在规则中修改，下一个返回的标记将从新的位置开始匹配
- `lexer.lineno`。表示当前行号。PLY 只是声明这个属性的存在，却永远不更新这个值。如果你想要跟踪行号的话，你需要自己添加代码（ 4.6 行号和位置信息）
- `lexer.lexdata`。当前 lexer 的输入字串，这个字符串就是 input() 方法的输入字串，更改它可能是个糟糕的做法，除非你知道自己在干什么。
- `lexer.lexmatch`。PLY 内部调用 Python 的 re.match() 方法得到的当前标记的原始的 Match 对象，该对象被保存在这个属性中。如果你的正则式中包含分组的话，你可以通过这个对象获得这些分组的值。注意：这个属性只在有标记规则定义的方法中才有效。

### 基于条件的扫描和启动条件

在高级的分析器应用程序中，使用状态化的词法扫描是很有用的。比如，你想在出现特定标记或句子结构的时候触发开始一个不同的词法分析逻辑。PLY 允许 lexer 在不同的状态之间转换。每个状态可以包含一些自己独特的标记和规则等。这是基于 GNU flex 的“启动条件”来实现的，关于 flex 详见 http://flex.sourceforge.net/manual/Start-Conditions.html#Start-Conditions

要使用 lex 的状态，你必须首先声明。通过在 lex 模块中声明"states"来做到：

```python
states = (
   ('foo','exclusive'),
   ('bar','inclusive'),
)
```

这个声明中包含有两个状态：'foo'和'bar'。状态可以有两种类型：'排他型'和'包容型'。排他型的状态会使得 lexer 的行为发生完全的改变：只有能够匹配在这个状态下定义的规则的标记才会返回；包容型状态会将定义在这个状态下的规则添加到默认的规则集中，进而，只要能匹配这个规则集的标记都会返回。

一旦声明好之后，标记规则的命名需要包含状态名：

```python
t_foo_NUMBER = r'\d+'                      # Token 'NUMBER' in state 'foo'        
t_bar_ID     = r'[a-zA-Z_][a-zA-Z0-9_]*'   # Token 'ID' in state 'bar'

def t_foo_newline(t):
    r'\n'
    t.lexer.lineno += 1
```

一个标记可以用在多个状态中，只要将多个状态名包含在声明中：

```python
t_foo_bar_NUMBER = r'\d+'         # Defines token 'NUMBER' in both state 'foo' and 'bar'
```

同样的，在任何状态下都生效的声明可以在命名中使用`ANY`：

```python
t_ANY_NUMBER = r'\d+'         # Defines a token 'NUMBER' in all states
```

不包含状态名的情况下，标记被关联到一个特殊的状态`INITIAL`，比如，下面两个声明是等价的：

```python
t_NUMBER = r'\d+'
t_INITIAL_NUMBER = r'\d+'
```

特殊的`t_ignore()`和`t_error()`也可以用状态关联：

```python
t_foo_ignore = " \t\n"       # Ignored characters for state 'foo'

def t_bar_error(t):          # Special error handler for state 'bar'
    pass 
```

词法分析默认在`INITIAL`状态下工作，这个状态下包含了所有默认的标记规则定义。对于不希望使用“状态”的用户来说，这是完全透明的。在分析过程中，如果你想要改变词法分析器的这种的状态，使用`begin()`方法：

```python
def t_begin_foo(t):
    r'start_foo'
    t.lexer.begin('foo')             # Starts 'foo' state
```

使用 begin() 切换回初始状态：

```python
def t_foo_end(t):
    r'end_foo'
    t.lexer.begin('INITIAL')        # Back to the initial state
```

状态的切换可以使用栈：

```python
def t_begin_foo(t):
    r'start_foo'
    t.lexer.push_state('foo')             # Starts 'foo' state

def t_foo_end(t):
    r'end_foo'
    t.lexer.pop_state()                   # Back to the previous state
```

当你在面临很多状态可以选择进入，而又仅仅想要回到之前的状态时，状态栈比较有用。

举个例子会更清晰。假设你在写一个分析器想要从一堆 C 代码中获取任意匹配的闭合的大括号里面的部分：这意味着，当遇到起始括号'{'，你需要读取与之匹配的'}'以上的所有部分。并返回字符串。使用通常的正则表达式几乎不可能，这是因为大括号可以嵌套，而且可以有注释，字符串等干扰。因此，试图简单的匹配第一个出现的'}'是不行的。这里你可以用lex的状态来做到：

```python
# Declare the state
states = (
  ('ccode','exclusive'),
)

# Match the first {. Enter ccode state.
def t_ccode(t):
    r'\{'
    t.lexer.code_start = t.lexer.lexpos        # Record the starting position
    t.lexer.level = 1                          # Initial brace level
    t.lexer.begin('ccode')                     # Enter 'ccode' state

# Rules for the ccode state
def t_ccode_lbrace(t):     
    r'\{'
    t.lexer.level +=1                

def t_ccode_rbrace(t):
    r'\}'
    t.lexer.level -=1

    # If closing brace, return the code fragment
    if t.lexer.level == 0:
         t.value = t.lexer.lexdata[t.lexer.code_start:t.lexer.lexpos+1]
         t.type = "CCODE"
         t.lexer.lineno += t.value.count('\n')
         t.lexer.begin('INITIAL')           
         return t

# C or C++ comment (ignore)    
def t_ccode_comment(t):
    r'(/\*(.|\n)*?\*/)|(//.*)'
    pass

# C string
def t_ccode_string(t):
   r'\"([^\\\n]|(\\.))*?\"'

# C character literal
def t_ccode_char(t):
   r'\'([^\\\n]|(\\.))*?\''

# Any sequence of non-whitespace characters (not braces, strings)
def t_ccode_nonspace(t):
   r'[^\s\{\}\'\"]+'

# Ignored characters (whitespace)
t_ccode_ignore = " \t\n"

# For bad characters, we just skip over it
def t_ccode_error(t):
    t.lexer.skip(1)
```

这个例子中，第一个'{'使得 lexer 记录了起始位置，并且进入新的状态'ccode'。一系列规则用来匹配接下来的输入，这些规则只是丢弃掉标记（不返回值），如果遇到闭合右括号，t_ccode_rbrace 规则收集其中所有的代码（利用先前记录的开始位置），并保存，返回的标记类型为'CCODE'，与此同时，词法分析的状态退回到初始状态。

### 其他问题

- lexer 需要输入的是一个字符串。好在大多数机器都有足够的内存，这很少导致性能的问题。这意味着，lexer 现在还不能用来处理文件流或者 socket 流。这主要是受到 re 模块的限制。
- lexer 支持用 Unicode 字符描述标记的匹配规则，也支持输入字串包含 Unicode
- 如果你想要向`re.compile()`方法提供 flag，使用 reflags 选项：lex.lex(reflags=re.UNICODE)
- 由于 lexer 是全部用 Python 写的，性能很大程度上取决于 Python 的 re 模块，即使已经尽可能的高效了。当接收极其大量的输入文件时表现并不尽人意。如果担忧性能，你可以升级到最新的 Python，或者手工创建分析器，或者用 C 语言写 lexer 并做成扩展模块。

如果你要创建一个手写的词法分析器并计划用在 yacc.py 中，只需要满足下面的要求：

- 需要提供一个 token() 方法来返回下一个标记，如果没有可用的标记了，则返回 None。
- token() 方法必须返回一个 tok 对象，具有 type 和 valu e属性。如果行号需要跟踪的话，标记还需要定义 lineno 属性。

## 语法分析基础

yacc.py 用来对语言进行语法分析。在给出例子之前，必须提一些重要的背景知识。首先，‘语法’通常用 BNF 范式来表达。例如，如果想要分析简单的算术表达式，你应该首先写下无二义的文法：

```
expression : expression + term
           | expression - term
           | term

term       : term * factor
           | term / factor
           | factor

factor     : NUMBER
           | ( expression )
```

在这个文法中，像`NUMBER`,`+`,`-`,`*`,`/`的符号被称为终结符，对应原始的输入。类似`term`，`factor`等称为非终结符，它们由一系列终结符或其他规则的符号组成，用来指代语法规则。

通常使用一种叫语法制导翻译的技术来指定某种语言的语义。在语法制导翻译中，符号及其属性出现在每个语法规则后面的动作中。每当一个语法被识别，动作就能够描述需要做什么。比如，对于上面给定的文法，想要实现一个简单的计算器，应该写成下面这样：

```
Grammar                             Action
--------------------------------    -------------------------------------------- 
expression0 : expression1 + term    expression0.val = expression1.val + term.val
            | expression1 - term    expression0.val = expression1.val - term.val
            | term                  expression0.val = term.val

term0       : term1 * factor        term0.val = term1.val * factor.val
            | term1 / factor        term0.val = term1.val / factor.val
            | factor                term0.val = factor.val

factor      : NUMBER                factor.val = int(NUMBER.lexval)
            | ( expression )        factor.val = expression.val
```

一种理解语法指导翻译的好方法是将符号看成对象。与符号相关的值代表了符号的“状态”（比如上面的 val 属性），语义行为用一组操作符号及符号值的函数或者方法来表达。

Yacc 用的分析技术是著名的 LR 分析法或者叫移进-归约分析法。LR 分析法是一种自下而上的技术：首先尝试识别右部的语法规则，每当右部得到满足，相应的行为代码将被触发执行，当前右边的语法符号将被替换为左边的语法符号。（归约）

LR 分析法一般这样实现：将下一个符号进栈，然后结合栈顶的符号和后继符号（译者注：下一个将要输入符号），与文法中的某种规则相比较。具体的算法可以在编译器的手册中查到，下面的例子展现了如果通过上面定义的文法，来分析 3 + 5 * ( 10 - 20 ) 这个表达式，$ 用来表示输入结束

```
Step Symbol Stack           Input Tokens            Action
---- ---------------------  ---------------------   -------------------------------
1                           3 + 5 * ( 10 - 20 )$    Shift 3
2    3                        + 5 * ( 10 - 20 )$    Reduce factor : NUMBER
3    factor                   + 5 * ( 10 - 20 )$    Reduce term   : factor
4    term                     + 5 * ( 10 - 20 )$    Reduce expr : term
5    expr                     + 5 * ( 10 - 20 )$    Shift +
6    expr +                     5 * ( 10 - 20 )$    Shift 5
7    expr + 5                     * ( 10 - 20 )$    Reduce factor : NUMBER
8    expr + factor                * ( 10 - 20 )$    Reduce term   : factor
9    expr + term                  * ( 10 - 20 )$    Shift *
10   expr + term *                  ( 10 - 20 )$    Shift (
11   expr + term * (                  10 - 20 )$    Shift 10
12   expr + term * ( 10                  - 20 )$    Reduce factor : NUMBER
13   expr + term * ( factor              - 20 )$    Reduce term : factor
14   expr + term * ( term                - 20 )$    Reduce expr : term
15   expr + term * ( expr                - 20 )$    Shift -
16   expr + term * ( expr -                20 )$    Shift 20
17   expr + term * ( expr - 20                )$    Reduce factor : NUMBER
18   expr + term * ( expr - factor            )$    Reduce term : factor
19   expr + term * ( expr - term              )$    Reduce expr : expr - term
20   expr + term * ( expr                     )$    Shift )
21   expr + term * ( expr )                    $    Reduce factor : (expr)
22   expr + term * factor                      $    Reduce term : term * factor
23   expr + term                               $    Reduce expr : expr + term
24   expr                                      $    Reduce expr
25                                             $    Success!
```

（译者注：action 里面的 Shift 就是进栈动作，简称移进；Reduce 是归约）

在分析表达式的过程中，一个相关的自动状态机和后继符号决定了下一步应该做什么。如果下一个标记看起来是一个有效语法（产生式）的一部分（通过栈上的其他项判断这一点），那么这个标记应该进栈。如果栈顶的项可以组成一个完整的右部语法规则，一般就可以进行“归约”，用产生式左边的符号代替这一组符号。当归约发生时，相应的行为动作就会执行。如果输入标记既不能移进也不能归约的话，就会发生语法错误，分析器必须进行相应的错误恢复。分析器直到栈空并且没有另外的输入标记时，才算成功。 需要注意的是，这是基于一个有限自动机实现的，有限自动器被转化成分析表。分析表的构建比较复杂，超出了本文的讨论范围。不过，这构建过程的微妙细节能够解释为什么在上面的例子中，解析器选择在步骤 9 将标记转移到堆栈中，而不是按照规则 expr : expr + term 做归约。

## Yacc

ply.yacc 模块实现了 PLY 的分析功能，‘yacc’是‘Yet Another Compiler Compiler’的缩写并保留了其作为 Unix 工具的名字。

### 一个例子

假设你希望实现上面的简单算术表达式的语法分析，代码如下：

```python
# Yacc example

import ply.yacc as yacc

# Get the token map from the lexer.  This is required.
from calclex import tokens

def p_expression_plus(p):
    'expression : expression PLUS term'
    p[0] = p[1] + p[3]

def p_expression_minus(p):
    'expression : expression MINUS term'
    p[0] = p[1] - p[3]

def p_expression_term(p):
    'expression : term'
    p[0] = p[1]

def p_term_times(p):
    'term : term TIMES factor'
    p[0] = p[1] * p[3]

def p_term_div(p):
    'term : term DIVIDE factor'
    p[0] = p[1] / p[3]

def p_term_factor(p):
    'term : factor'
    p[0] = p[1]

def p_factor_num(p):
    'factor : NUMBER'
    p[0] = p[1]

def p_factor_expr(p):
    'factor : LPAREN expression RPAREN'
    p[0] = p[2]

# Error rule for syntax errors
def p_error(p):
    print "Syntax error in input!"

# Build the parser
parser = yacc.yacc()

while True:
   try:
       s = raw_input('calc > ')
   except EOFError:
       break
   if not s: continue
   result = parser.parse(s)
   print result
```

在这个例子中，每个语法规则被定义成一个 Python 的方法，方法的文档字符串描述了相应的上下文无关文法，方法的语句实现了对应规则的语义行为。每个方法接受一个单独的 p 参数，p 是一个包含有当前匹配语法的符号的序列，p[i] 与语法符号的对应关系如下：

```python
def p_expression_plus(p):
    'expression : expression PLUS term'
    #   ^            ^        ^    ^
    #  p[0]         p[1]     p[2] p[3]

    p[0] = p[1] + p[3]
```

其中，p[i] 的值相当于词法分析模块中对 p.value 属性赋的值，对于非终结符的值，将在归约时由 p[0] 的赋值决定，这里的值可以是任何类型，当然，大多数情况下只是 Python 的简单类型、元组或者类的实例。在这个例子中，我们依赖这样一个事实：NUMBER 标记的值保存的是整型值，所有规则的行为都是得到这些整型值的算术运算结果，并传递结果。

> 注意：在这里负数的下标有特殊意义--这里的 p[-1] 不等同于 p[3]。详见下面的嵌入式动作部分

在 yacc 中定义的第一个语法规则被默认为起始规则（这个例子中的第一个出现的 expression 规则）。一旦起始规则被分析器归约，而且再无其他输入，分析器终止，最后的值将返回（这个值将是起始规则的p[0]）。注意：也可以通过在 yacc() 中使用 start 关键字参数来指定起始规则

p_error(p) 规则用于捕获语法错误。详见处理语法错误部分

为了构建分析器，需要调用 yacc.yacc() 方法。这个方法查看整个当前模块，然后试图根据你提供的文法构建 LR 分析表。第一次执行 yacc.yacc()，你会得到如下输出：

```python
$ python calcparse.py
Generating LALR tables
calc >
```

由于分析表的得出相对开销较大（尤其包含大量的语法的情况下），分析表被写入当前目录的一个叫 parsetab.py 的文件中。除此之外，会生成一个调试文件 parser.out。在接下来的执行中，yacc 直到发现文法发生变化，才会重新生成分析表和 parsetab.py 文件，否则 yacc 会从 parsetab.py 中加载分析表。注：如果有必要的话这里输出的文件名是可以改的。

如果在你的文法中有任何错误的话，yacc.py 会产生调试信息，而且可能抛出异常。一些可以被检测到的错误如下：

- 方法重复定义（在语法文件中具有相同名字的方法）
- 二义文法产生的移进-归约和归约-归约冲突
- 指定了错误的文法
- 不可终止的递归（规则永远无法终结）
- 未使用的规则或标记
- 未定义的规则或标记

下面几个部分将更详细的讨论语法规则

这个例子的最后部分展示了如何执行由 yacc() 方法创建的分析器。你只需要简单的调用 parse()，并将输入字符串作为参数就能运行分析器。它将运行所有的语法规则，并返回整个分析的结果，这个结果就是在起始规则中赋给 p[0] 的值。

### 将语法规则合并

如果语法规则类似的话，可以合并到一个方法中。例如，考虑前面例子中的两个规则：

```python
def p_expression_plus(p):
    'expression : expression PLUS term'
    p[0] = p[1] + p[3]

def p_expression_minus(t):
    'expression : expression MINUS term'
    p[0] = p[1] - p[3]
```

比起写两个方法，你可以像下面这样写在一个方法里面：

```python
def p_expression(p):
    '''expression : expression PLUS term
                  | expression MINUS term'''
    if p[2] == '+':
        p[0] = p[1] + p[3]
    elif p[2] == '-':
        p[0] = p[1] - p[3]
```

总之，方法的文档字符串可以包含多个语法规则。所以，像这样写也是合法的（尽管可能会引起困惑）：

```python
def p_binary_operators(p):
    '''expression : expression PLUS term
                  | expression MINUS term
       term       : term TIMES factor
                  | term DIVIDE factor'''
    if p[2] == '+':
        p[0] = p[1] + p[3]
    elif p[2] == '-':
        p[0] = p[1] - p[3]
    elif p[2] == '*':
        p[0] = p[1] * p[3]
    elif p[2] == '/':
        p[0] = p[1] / p[3]
```

如果所有的规则都有相似的结构，那么将语法规则合并才是个不错的注意（比如，产生式的项数相同）。不然，语义动作可能会变得复杂。不过，简单情况下，可以使用`len()`方法区分，比如：

```python
def p_expressions(p):
    '''expression : expression MINUS expression
                  | MINUS expression'''
    if (len(p) == 4):
        p[0] = p[1] - p[3]
    elif (len(p) == 3):
        p[0] = -p[2]
```

如果考虑解析的性能，你应该避免像这些例子一样在一个语法规则里面用很多条件来处理。因为，每次检查当前究竟匹配的是哪个语法规则的时候，实际上重复做了分析器已经做过的事（分析器已经准确的知道哪个规则被匹配了）。为每个规则定义单独的方法，可以消除这点开销。

### 字面字符

如果愿意，可以在语法规则里面使用单个的字面字符，例如：

```python
def p_binary_operators(p):
    '''expression : expression '+' term
                  | expression '-' term
       term       : term '*' factor
                  | term '/' factor'''
    if p[2] == '+':
        p[0] = p[1] + p[3]
    elif p[2] == '-':
        p[0] = p[1] - p[3]
    elif p[2] == '*':
        p[0] = p[1] * p[3]
    elif p[2] == '/':
        p[0] = p[1] / p[3]
```

字符必须像'+'那样使用单引号。除此之外，需要将用到的字符定义单独定义在 lex 文件的`literals`列表里：

```python
# Literals.  Should be placed in module given to lex()
literals = ['+','-','*','/' ]
```

字面的字符只能是单个字符。因此，像'<='或者'=='都是不合法的，只能使用一般的词法规则（例如 t_EQ = r'==')。

### 空产生式

yacc.py 可以处理空产生式，像下面这样做：

```python
def p_empty(p):
    'empty :'
    pass
```

现在可以使用空匹配，只要将'empty'当成一个符号使用：

```python
def p_optitem(p):
    'optitem : item'
    '        | empty'
    ...
```

注意：你可以将产生式保持'空'，来表示空匹配。然而，我发现用一个'empty'规则并用其来替代'空'，更容易表达意图，并有较好的可读性。

### 改变起始符号

默认情况下，在 yacc 中的第一条规则是起始语法规则（顶层规则）。可以用 start 标识来改变这种行为：

```python
start = 'foo'

def p_bar(p):
    'bar : A B'

# This is the starting rule due to the start specifier above
def p_foo(p):
    'foo : bar X'
...
```

用 start 标识有助于在调试的时候将大型的语法规则分成小部分来分析。也可把 start 符号作为yacc的参数：

```python
yacc.yacc(start='foo')
```

### 处理二义文法

上面例子中，对表达式的文法描述用一种特别的形式规避了二义文法。然而，在很多情况下，这样的特殊文法很难写，或者很别扭。一个更为自然和舒服的语法表达应该是这样的：

```python
expression : expression PLUS expression
           | expression MINUS expression
           | expression TIMES expression
           | expression DIVIDE expression
           | LPAREN expression RPAREN
           | NUMBER
```

不幸的是，这样的文法是存在二义性的。举个例子，如果你要解析字符串"3 *4 + 5"，操作符如何分组并没有指明，究竟是表示"(3* 4) + 5"还是"3 * (4 + 5)"呢？

如果在 yacc.py 中存在二义文法，会输出"移进归约冲突"或者"归约归约冲突"。在分析器无法确定是将下一个符号移进栈还是将当前栈中的符号归约时会产生移进归约冲突。例如，对于"3 * 4 + 5"，分析器内部栈是这样工作的：

```python
Step Symbol Stack           Input Tokens            Action
---- ---------------------  ---------------------   -------------------------------
1    $                                3 * 4 + 5$    Shift 3
2    $ 3                                * 4 + 5$    Reduce : expression : NUMBER
3    $ expr                             * 4 + 5$    Shift *
4    $ expr *                             4 + 5$    Shift 4
5    $ expr * 4                             + 5$    Reduce: expression : NUMBER
6    $ expr * expr                          + 5$    SHIFT/REDUCE CONFLICT ????
```

在这个例子中，当分析器来到第 6 步的时候，有两种选择：一是按照 expr : expr * expr 归约，一是将标记'+'继续移进栈。两种选择对于上面的上下文无关文法而言都是合法的。

默认情况下，所有的移进归约冲突会倾向于使用移进来处理。因此，对于上面的例子，分析器总是会将'+'进栈，而不是做归约。虽然在很多情况下，这个策略是合适的（像"if-then"和"if-then-else"），但这对于算术表达式是不够的。事实上，对于上面的例子，将'+'进栈是完全错误的，应当先将expr * expr归约，因为乘法的优先级要高于加法。

为了解决二义文法，尤其是对表达式文法，yacc.py 允许为标记单独指定优先级和结合性。需要像下面这样增加一个 precedence 变量：

```python
precedence = (
    ('left', 'PLUS', 'MINUS'),
    ('left', 'TIMES', 'DIVIDE'),
)
```

这样的定义说明 PLUS/MINUS 标记具有相同的优先级和左结合性，TIMES/DIVIDE 具有相同的优先级和左结合性。在 precedence 声明中，标记的优先级从低到高。因此，这个声明表明 TIMES/DIVIDE（他们较晚加入 precedence）的优先级高于 PLUS/MINUS。

由于为标记添加了数字表示的优先级和结合性的属性，所以，对于上面的例子，将会得到：

```python
PLUS      : level = 1,  assoc = 'left'
MINUS     : level = 1,  assoc = 'left'
TIMES     : level = 2,  assoc = 'left'
DIVIDE    : level = 2,  assoc = 'left'
```

随后这些值被附加到语法规则的优先级和结合性属性上，这些值由最右边的终结符的优先级和结合性决定：

```python
expression : expression PLUS expression                 # level = 1, left
           | expression MINUS expression                # level = 1, left
           | expression TIMES expression                # level = 2, left
           | expression DIVIDE expression               # level = 2, left
           | LPAREN expression RPAREN                   # level = None (not specified)
           | NUMBER                                     # level = None (not specified)
```

当出现移进归约冲突时，分析器生成器根据下面的规则解决二义文法：

1. 如果当前的标记的优先级高于栈顶规则的优先级，移进当前标记
2. 如果栈顶规则的优先级更高，进行归约
3. 如果当前的标记与栈顶规则的优先级相同，如果标记是左结合的，则归约，否则，如果是右结合的则移进
4. 如果没有优先级可以参考，默认对于移进归约冲突执行移进

比如，当解析到"expression PLUS expression"这个语法时，下一个标记是 TIMES，此时将执行移进，因为 TIMES 具有比 PLUS 更高的优先级；当解析到"expression TIMES expression"，下一个标记是 PLUS，此时将执行归约，因为 PLUS 的优先级低于 TIMES。

如果在使用前三种技术解决已经归约冲突后，yacc.py 将不会报告语法中的冲突或者错误（不过，会在 parser.out 这个调试文件中输出一些信息）

使用 precedence 指定优先级的技术会带来一个问题，有时运算符的优先级需要基于上下文。例如，考虑"3 + 4 * -5"中的一元的'-'。数学上讲，一元运算符应当拥有较高的优先级。然而，在我们的 precedence 定义中，MINUS 的优先级却低于 TIMES。为了解决这个问题，precedene 规则中可以包含"虚拟标记"：

```python
precedence = (
    ('left', 'PLUS', 'MINUS'),
    ('left', 'TIMES', 'DIVIDE'),
    ('right', 'UMINUS'),            # Unary minus operator
)
```

在语法文件中，我们可以这么表示一元算符：

```python
def p_expr_uminus(p):
    'expression : MINUS expression %prec UMINUS'
    p[0] = -p[2]
```

在这个例子中，%prec UMINUS 覆盖了默认的优先级（MINUS 的优先级），将 UMINUS 指代的优先级应用在该语法规则上。

起初，UMINUS 标记的例子会让人感到困惑。UMINUS 既不是输入的标记也不是语法规则，你应当将其看成 precedence 表中的特殊的占位符。当你使用 %prec 宏时，你是在告诉 yacc，你希望表达式使用这个占位符所表示的优先级，而不是正常的优先级。

还可以在 precedence 表中指定"非关联"。这表明你不希望链式运算符。比如，假如你希望支持比较运算符'<'和'>'，但是你不希望支持 a < b < c，只要简单指定规则如下：

```python
precedence = (
    ('nonassoc', 'LESSTHAN', 'GREATERTHAN'),  # Nonassociative operators
    ('left', 'PLUS', 'MINUS'),
    ('left', 'TIMES', 'DIVIDE'),
    ('right', 'UMINUS'),            # Unary minus operator
)
```

此时，当输入形如 a < b < c 时，将产生语法错误，却不影响形如 a < b 的表达式。

对于给定的符号集，存在多种语法规则可以匹配时会产生归约/归约冲突。这样的冲突往往很严重，而且总是通过匹配最早出现的语法规则来解决。归约/归约冲突几乎总是相同的符号集合具有不同的规则可以匹配，而在这一点上无法抉择，比如：

```python
assignment :  ID EQUALS NUMBER
           |  ID EQUALS expression

expression : expression PLUS expression
           | expression MINUS expression
           | expression TIMES expression
           | expression DIVIDE expression
           | LPAREN expression RPAREN
           | NUMBER
```

这个例子中，对于下面这两条规则将产生归约/归约冲突：

```python
assignment  : ID EQUALS NUMBER
expression  : NUMBER
```

比如，对于"a = 5"，分析器不知道应当按照 assignment : ID EQUALS NUMBER 归约，还是先将 5 归约成 expression，再归约成 assignment : ID EQUALS expression。

应当指出的是，只是简单的查看语法规则是很难减少归约/归约冲突。如果出现归约/归约冲突，yacc()会帮助打印出警告信息：

```python
WARNING: 1 reduce/reduce conflict
WARNING: reduce/reduce conflict in state 15 resolved using rule (assignment -> ID EQUALS NUMBER)
WARNING: rejected rule (expression -> NUMBER)
```

上面的信息标识出了冲突的两条规则，但是，并无法指出究竟在什么情况下会出现这样的状态。想要发现问题，你可能需要结合语法规则和`parser.out`调试文件的内容。

### parser.out调试文件

使用 LR 分析算法跟踪移进/归约冲突和归约/归约冲突是件乐在其中的事。为了辅助调试，yacc.py 在生成分析表时会创建出一个调试文件叫 parser.out：

```python
Unused terminals:

Grammar

Rule 1     expression -> expression PLUS expression
Rule 2     expression -> expression MINUS expression
Rule 3     expression -> expression TIMES expression
Rule 4     expression -> expression DIVIDE expression
Rule 5     expression -> NUMBER
Rule 6     expression -> LPAREN expression RPAREN

Terminals, with rules where they appear

TIMES                : 3
error                : 
MINUS                : 2
RPAREN               : 6
LPAREN               : 6
DIVIDE               : 4
PLUS                 : 1
NUMBER               : 5

Nonterminals, with rules where they appear

expression           : 1 1 2 2 3 3 4 4 6 0

Parsing method: LALR

state 0

    S' -> . expression
    expression -> . expression PLUS expression
    expression -> . expression MINUS expression
    expression -> . expression TIMES expression
    expression -> . expression DIVIDE expression
    expression -> . NUMBER
    expression -> . LPAREN expression RPAREN

    NUMBER          shift and go to state 3
    LPAREN          shift and go to state 2

state 1

    S' -> expression .
    expression -> expression . PLUS expression
    expression -> expression . MINUS expression
    expression -> expression . TIMES expression
    expression -> expression . DIVIDE expression

    PLUS            shift and go to state 6
    MINUS           shift and go to state 5
    TIMES           shift and go to state 4
    DIVIDE          shift and go to state 7

state 2

    expression -> LPAREN . expression RPAREN
    expression -> . expression PLUS expression
    expression -> . expression MINUS expression
    expression -> . expression TIMES expression
    expression -> . expression DIVIDE expression
    expression -> . NUMBER
    expression -> . LPAREN expression RPAREN

    NUMBER          shift and go to state 3
    LPAREN          shift and go to state 2

state 3

    expression -> NUMBER .

    $               reduce using rule 5
    PLUS            reduce using rule 5
    MINUS           reduce using rule 5
    TIMES           reduce using rule 5
    DIVIDE          reduce using rule 5
    RPAREN          reduce using rule 5

state 4

    expression -> expression TIMES . expression
    expression -> . expression PLUS expression
    expression -> . expression MINUS expression
    expression -> . expression TIMES expression
    expression -> . expression DIVIDE expression
    expression -> . NUMBER
    expression -> . LPAREN expression RPAREN

    NUMBER          shift and go to state 3
    LPAREN          shift and go to state 2

state 5

    expression -> expression MINUS . expression
    expression -> . expression PLUS expression
    expression -> . expression MINUS expression
    expression -> . expression TIMES expression
    expression -> . expression DIVIDE expression
    expression -> . NUMBER
    expression -> . LPAREN expression RPAREN

    NUMBER          shift and go to state 3
    LPAREN          shift and go to state 2

state 6

    expression -> expression PLUS . expression
    expression -> . expression PLUS expression
    expression -> . expression MINUS expression
    expression -> . expression TIMES expression
    expression -> . expression DIVIDE expression
    expression -> . NUMBER
    expression -> . LPAREN expression RPAREN

    NUMBER          shift and go to state 3
    LPAREN          shift and go to state 2

state 7

    expression -> expression DIVIDE . expression
    expression -> . expression PLUS expression
    expression -> . expression MINUS expression
    expression -> . expression TIMES expression
    expression -> . expression DIVIDE expression
    expression -> . NUMBER
    expression -> . LPAREN expression RPAREN

    NUMBER          shift and go to state 3
    LPAREN          shift and go to state 2

state 8

    expression -> LPAREN expression . RPAREN
    expression -> expression . PLUS expression
    expression -> expression . MINUS expression
    expression -> expression . TIMES expression
    expression -> expression . DIVIDE expression

    RPAREN          shift and go to state 13
    PLUS            shift and go to state 6
    MINUS           shift and go to state 5
    TIMES           shift and go to state 4
    DIVIDE          shift and go to state 7

state 9

    expression -> expression TIMES expression .
    expression -> expression . PLUS expression
    expression -> expression . MINUS expression
    expression -> expression . TIMES expression
    expression -> expression . DIVIDE expression

    $               reduce using rule 3
    PLUS            reduce using rule 3
    MINUS           reduce using rule 3
    TIMES           reduce using rule 3
    DIVIDE          reduce using rule 3
    RPAREN          reduce using rule 3

  ! PLUS            [ shift and go to state 6 ]
  ! MINUS           [ shift and go to state 5 ]
  ! TIMES           [ shift and go to state 4 ]
  ! DIVIDE          [ shift and go to state 7 ]

state 10

    expression -> expression MINUS expression .
    expression -> expression . PLUS expression
    expression -> expression . MINUS expression
    expression -> expression . TIMES expression
    expression -> expression . DIVIDE expression

    $               reduce using rule 2
    PLUS            reduce using rule 2
    MINUS           reduce using rule 2
    RPAREN          reduce using rule 2
    TIMES           shift and go to state 4
    DIVIDE          shift and go to state 7

  ! TIMES           [ reduce using rule 2 ]
  ! DIVIDE          [ reduce using rule 2 ]
  ! PLUS            [ shift and go to state 6 ]
  ! MINUS           [ shift and go to state 5 ]

state 11

    expression -> expression PLUS expression .
    expression -> expression . PLUS expression
    expression -> expression . MINUS expression
    expression -> expression . TIMES expression
    expression -> expression . DIVIDE expression

    $               reduce using rule 1
    PLUS            reduce using rule 1
    MINUS           reduce using rule 1
    RPAREN          reduce using rule 1
    TIMES           shift and go to state 4
    DIVIDE          shift and go to state 7

  ! TIMES           [ reduce using rule 1 ]
  ! DIVIDE          [ reduce using rule 1 ]
  ! PLUS            [ shift and go to state 6 ]
  ! MINUS           [ shift and go to state 5 ]

state 12

    expression -> expression DIVIDE expression .
    expression -> expression . PLUS expression
    expression -> expression . MINUS expression
    expression -> expression . TIMES expression
    expression -> expression . DIVIDE expression

    $               reduce using rule 4
    PLUS            reduce using rule 4
    MINUS           reduce using rule 4
    TIMES           reduce using rule 4
    DIVIDE          reduce using rule 4
    RPAREN          reduce using rule 4

  ! PLUS            [ shift and go to state 6 ]
  ! MINUS           [ shift and go to state 5 ]
  ! TIMES           [ shift and go to state 4 ]
  ! DIVIDE          [ shift and go to state 7 ]

state 13

    expression -> LPAREN expression RPAREN .

    $               reduce using rule 6
    PLUS            reduce using rule 6
    MINUS           reduce using rule 6
    TIMES           reduce using rule 6
    DIVIDE          reduce using rule 6
    RPAREN          reduce using rule 6
```

文件中出现的不同状态，代表了有效输入标记的所有可能的组合，这是依据文法规则得到的。当得到输入标记时，分析器将构造一个栈，并找到匹配的规则。每个状态跟踪了当前输入进行到语法规则中的哪个位置，在每个规则中，'.'表示当前分析到规则的哪个位置，而且，对于在当前状态下，输入的每个有效标记导致的动作也被罗列出来。当出现移进/归约或归约/归约冲突时，被忽略的规则前面会添加!，就像这样：

```python
! TIMES           [ reduce using rule 2 ]
  ! DIVIDE          [ reduce using rule 2 ]
  ! PLUS            [ shift and go to state 6 ]
  ! MINUS           [ shift and go to state 5 ]
```

通过查看这些规则并结合一些实例，通常能够找到大部分冲突的根源。应该强调的是，不是所有的移进归约冲突都是不好的，想要确定解决方法是否正确，唯一的办法就是查看 parser.out。

### 处理语法错误

如果你创建的分析器用于产品，处理语法错误是很重要的。一般而言，你不希望分析器在遇到错误的时候就抛出异常并终止，相反，你需要它报告错误，尽可能的恢复并继续分析，一次性的将输入中所有的错误报告给用户。这是一些已知语言编译器的标准行为，例如 C,C++,Java。在 PLY 中，在语法分析过程中出现错误，错误会被立即检测到（分析器不会继续读取源文件中错误点后面的标记）。然而，这时，分析器会进入恢复模式，这个模式能够用来尝试继续向下分析。LR 分析器的错误恢复是个理论与技巧兼备的问题，yacc.py 提供的错误机制与 Unix 下的 yacc 类似，所以你可以从诸如 O'Reilly 出版的《Lex and yacc》的书中找到更多的细节。

当错误发生时，yacc.py 按照如下步骤进行：

1. 第一次错误产生时，用户定义的 p_error()方法会被调用，出错的标记会作为参数传入；如果错误是因为到达文件结尾造成的，传入的参数将为 None。随后，分析器进入到“错误恢复”模式，该模式下不会在产生`p_error()`调用，直到它成功的移进 3 个标记，然后回归到正常模式。
2. 如果在 p_error() 中没有指定恢复动作的话，这个导致错误的标记会被替换成一个特殊的 error 标记。
3. 如果导致错误的标记已经是 error 的话，原先的栈顶的标记将被移除。
4. 如果整个分析栈被放弃，分析器会进入重置状态，并从他的初始状态开始分析。
5. 如果此时的语法规则接受 error 标记，error 标记会移进栈。
6. 如果当前栈顶是 error 标记，之后的标记将被忽略，直到有标记能够导致 error 的归约。

#### 根据 error 规则恢复和再同步

最佳的处理语法错误的做法是在语法规则中包含 error 标记。例如，假设你的语言有一个关于 print 的语句的语法规则：

```python
def p_statement_print(p):
     'statement : PRINT expr SEMI'
     ...
```

为了处理可能的错误表达式，你可以添加一条额外的语法规则：

```python
def p_statement_print_error(p):
     'statement : PRINT error SEMI'
     print "Syntax error in print statement. Bad expression"
```

这样（expr 错误时），error 标记会匹配任意多个分号之前的标记（分号是`SEMI`指代的字符）。一旦找到分号，规则将被匹配，这样 error 标记就被归约了。

这种类型的恢复有时称为"分析器再同步"。error 标记扮演了表示所有错误标记的通配符的角色，而紧随其后的标记扮演了同步标记的角色。

重要的一个说明是，通常 error 不会作为语法规则的最后一个标记，像这样：

```python
def p_statement_print_error(p):
    'statement : PRINT error'
    print "Syntax error in print statement. Bad expression"
```

这是因为，第一个导致错误的标记会使得该规则立刻归约，进而使得在后面还有错误标记的情况下，恢复变得困难。

#### 悲观恢复模式

另一个错误恢复方法是采用“悲观模式”：该模式下，开始放弃剩余的标记，直到能够达到一个合适的恢复机会。

悲观恢复模式都是在 p_error() 方法中做到的。例如，这个方法在开始丢弃标记后，直到找到闭合的'}'，才重置分析器到初始化状态：

```python
def p_error(p):
    print "Whoa. You are seriously hosed."
    # Read ahead looking for a closing '}'
    while 1:
        tok = yacc.token()             # Get the next token
        if not tok or tok.type == 'RBRACE': break
    yacc.restart()
```

下面这个方法简单的抛弃错误的标记，并告知分析器错误被接受了：

```python
def p_error(p):
    print "Syntax error at token", p.type
    # Just discard the token and tell the parser it's okay.
    yacc.errok()
```

在`p_error()`方法中，有三个可用的方法来控制分析器的行为：

- `yacc.errok()` 这个方法将分析器从恢复模式切换回正常模式。这会使得不会产生 error 标记，并重置内部的 error 计数器，而且下一个语法错误会再次产生 p_error() 调用
- `yacc.token()` 这个方法用于得到下一个标记
- `yacc.restart()` 这个方法抛弃当前整个分析栈，并重置分析器为起始状态

注意：这三个方法只能在`p_error()`中使用，不能用在其他任何地方。

p_error()方法也可以返回标记，这样能够控制将哪个标记作为下一个标记返回给分析器。这对于需要同步一些特殊标记的时候有用，比如：

```python
def p_error(p):
    # Read ahead looking for a terminating ";"
    while 1:
        tok = yacc.token()             # Get the next token
        if not tok or tok.type == 'SEMI': break
    yacc.errok()

    # Return SEMI to the parser as the next lookahead token
    return tok
```

#### 从产生式中抛出错误

如果有需要的话，产生式规则可以主动的使分析器进入恢复模式。这是通过抛出`SyntaxError`异常做到的：

```python
def p_production(p):
    'production : some production ...'
    raise SyntaxError
```

raise SyntaxError 错误的效果就如同当前的标记是错误标记一样。因此，当你这么做的话，最后一个标记将被弹出栈，当前的下一个标记将是 error 标记，分析器进入恢复模式，试图归约满足 error 标记的规则。此后的步骤与检测到语法错误的情况是完全一样的，p_error() 也会被调用。

手动设置错误有个重要的方面，就是 p_error() 方法在这种情况下不会调用。如果你希望记录错误，确保在抛出 SyntaxError 错误的产生式中实现。

注：这个功能是为了模仿 yacc 中的`YYERROR`宏的行为

#### 错误恢复总结

对于通常的语言，使用 error 规则和再同步标记可能是最合理的手段。这是因为你可以将语法设计成在一个相对容易恢复和继续分析的点捕获错误。悲观恢复模式只在一些十分特殊的应用中有用，这些应用往往需要丢弃掉大量输入，再寻找合理的同步点。

### 行号和位置的跟踪

位置跟踪通常是个设计编译器时的技巧性玩意儿。默认情况下，PLY 跟踪所有标记的行号和位置，这些信息可以这样得到：

- p.lineno(num) 返回第 num 个符号的行号
- p.lexpos(num) 返回第 num 个符号的词法位置偏移

例如：

```python
def p_expression(p):
    'expression : expression PLUS expression'
    p.lineno(1)        # Line number of the left expression
    p.lineno(2)        # line number of the PLUS operator
    p.lineno(3)        # line number of the right expression
    ...
    start,end = p.linespan(3)    # Start,end lines of the right expression
    starti,endi = p.lexspan(3)   # Start,end positions of right expression
```

注意：lexspan() 方法只会返回的结束位置是最后一个符号的起始位置。

虽然，PLY 对所有符号的行号和位置的跟踪很管用，但经常是不必要的。例如，你仅仅是在错误信息中使用行号，你通常可以仅仅使用关键标记的信息，比如：

```python
def p_bad_func(p):
    'funccall : fname LPAREN error RPAREN'
    # Line number reported from LPAREN token
    print "Bad function call at line", p.lineno(2)
```

类似的，为了改善性能，你可以有选择性的将行号信息在必要的时候进行传递，这是通过 p.set_lineno() 实现的，例如：

```python
def p_fname(p):
    'fname : ID'
    p[0] = p[1]
    p.set_lineno(0,p.lineno(1))
```

对于已经完成分析的规则，PLY 不会保留行号信息，如果你是在构建抽象语法树而且需要行号，你应该确保行号保留在树上。

### 构造抽象语法树

yacc.py 没有构造抽像语法树的特殊方法。不过，你可以自己很简单的构造出来。

一个最为简单的构造方法是为每个语法规则创建元组或者字典，并传递它们。有很多中可行的方案，下面是一个例子：

```python
def p_expression_binop(p):
    '''expression : expression PLUS expression
                  | expression MINUS expression
                  | expression TIMES expression
                  | expression DIVIDE expression'''

    p[0] = ('binary-expression',p[2],p[1],p[3])

def p_expression_group(p):
    'expression : LPAREN expression RPAREN'
    p[0] = ('group-expression',p[2])

def p_expression_number(p):
    'expression : NUMBER'
    p[0] = ('number-expression',p[1])
```

另一种方法可以是为不同的抽象树节点创建一系列的数据结构，并赋值给 p[0]：

```python
class Expr: pass

class BinOp(Expr):
    def __init__(self,left,op,right):
        self.type = "binop"
        self.left = left
        self.right = right
        self.op = op

class Number(Expr):
    def __init__(self,value):
        self.type = "number"
        self.value = value

def p_expression_binop(p):
    '''expression : expression PLUS expression
                  | expression MINUS expression
                  | expression TIMES expression
                  | expression DIVIDE expression'''

    p[0] = BinOp(p[1],p[2],p[3])

def p_expression_group(p):
    'expression : LPAREN expression RPAREN'
    p[0] = p[2]

def p_expression_number(p):
    'expression : NUMBER'
    p[0] = Number(p[1])
```

这种方式的好处是在处理复杂语义时比较简单：类型检查、代码生成、以及其他针对树节点的功能。

为了简化树的遍历，可以创建一个通用的树节点结构，例如：

```python
class Node:
    def __init__(self,type,children=None,leaf=None):
         self.type = type
         if children:
              self.children = children
         else:
              self.children = [ ]
         self.leaf = leaf

def p_expression_binop(p):
    '''expression : expression PLUS expression
                  | expression MINUS expression
                  | expression TIMES expression
                  | expression DIVIDE expression'''

    p[0] = Node("binop", [p[1],p[3]], p[2])
```

### 嵌入式动作

yacc 使用的分析技术只允许在规则规约后执行动作。假设有如下规则：

```python
def p_foo(p):
    "foo : A B C D"
    print "Parsed a foo", p[1],p[2],p[3],p[4]
```

方法只会在符号 A,B,C和D 都完成后才能执行。可是有的时候，在中间阶段执行一小段代码是有用的。假如，你想在 A 完成后立即执行一些动作，像下面这样用空规则：

```python
def p_foo(p):
    "foo : A seen_A B C D"
    print "Parsed a foo", p[1],p[3],p[4],p[5]
    print "seen_A returned", p[2]

def p_seen_A(p):
    "seen_A :"
    print "Saw an A = ", p[-1]   # Access grammar symbol to left
    p[0] = some_value            # Assign value to seen_A
```

在这个例子中，空规则 seen_A 将在 A 移进分析栈后立即执行。p[-1] 指代的是在分析栈上紧跟在 seen_A 左侧的符号。在这个例子中，是 A 符号。像其他普通的规则一样，在嵌入式行为中也可以通过为 p[0] 赋值来返回某些值。

使用嵌入式动作可能会导致移进归约冲突，比如，下面的语法是没有冲突的：

```python
def p_foo(p):
    """foo : abcd
           | abcx"""

def p_abcd(p):
    "abcd : A B C D"

def p_abcx(p):
    "abcx : A B C X"
```

可是，如果像这样插入一个嵌入式动作：

```python
def p_foo(p):
    """foo : abcd
           | abcx"""

def p_abcd(p):
    "abcd : A B C D"

def p_abcx(p):
    "abcx : A B seen_AB C X"

def p_seen_AB(p):
    "seen_AB :"
```

会产生移进归约冲，只是由于对于两个规则 abcd 和 abcx 中的 C，分析器既可以根据 abcd 规则移进，也可以根据 abcx 规则先将空的 seen_AB 归约。

嵌入动作的一般用于分析以外的控制，比如为本地变量定义作用于。对于 C 语言：

```python
def p_statements_block(p):
    "statements: LBRACE new_scope statements RBRACE"""
    # Action code
    ...
    pop_scope()        # Return to previous scope

def p_new_scope(p):
    "new_scope :"
    # Create a new scope for local variables
    s = new_scope()
    push_scope(s)
    ...
```

在这个例子中，new_scope 作为嵌入式行为，在左大括号{之后立即执行。可以是调正内部符号表或者其他方面。statements_block 一完成，代码可能会撤销在嵌入动作时的操作（比如，pop_scope())

### Yacc 的其他

- 默认的分析方法是 LALR，使用 SLR 请像这样运行 yacc()：yacc.yacc(method="SLR") 注意：LRLR 生成的分析表大约要比 SLR 的大两倍。解析的性能没有本质的区别，因为代码是一样的。由于 LALR 能力稍强，所以更多的用于复杂的语法。
- 默认情况下，yacc.py 依赖 lex.py 产生的标记。不过，可以用一个等价的词法标记生成器代替： yacc.parse(lexer=x) 这个例子中，x 必须是一个 Lexer 对象，至少拥有 x.token() 方法用来获取标记。如果将输入字串提供给 yacc.parse()，lexer 还必须具有 x.input() 方法。
- 默认情况下，yacc 在调试模式下生成分析表（会生成 parser.out 文件和其他东西），使用 yacc.yacc(debug=0) 禁用调试模式。
- 改变 parsetab.py 的文件名：yacc.yacc(tabmodule="foo")
- 改变 parsetab.py 的生成目录：yacc.yacc(tabmodule="foo",outputdir="somedirectory")
- 不生成分析表：yacc.yacc(write_tables=0)。注意：如果禁用分析表生成，yacc()将在每次运行的时候重新构建分析表（这里耗费的时候取决于语法文件的规模）
- 想在分析过程中输出丰富的调试信息，使用：yacc.parse(debug=1)
- yacc.yacc()方法会返回分析器对象，如果你想在一个程序中支持多个分析器：

```python
p = yacc.yacc()
...
p.parse()
```

注意：yacc.parse() 方法只绑定到最新创建的分析器对象上。

- 由于生成生成 LALR 分析表相对开销较大，先前生成的分析表会被缓存和重用。判断是否重新生成的依据是对所有的语法规则和优先级规则进行 MD5 校验，只有不匹配时才会重新生成。生成分析表是合理有效的办法，即使是面对上百个规则和状态的语法。对于复杂的编程语言，像 C 语言，在一些慢的机器上生成分析表可能要花费 30-60 秒，请耐心。
- 由于 LR 分析过程是基于分析表的，分析器的性能很大程度上取决于语法的规模。最大的瓶颈可能是词法分析器和语法规则的复杂度。