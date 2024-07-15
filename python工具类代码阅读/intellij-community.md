





## 源码：python/helpers/pycharm





`_jb_runner_tools.py`：

> Pycharm运行例子：
>
> ```
> C:\Users\Administrator\AppData\Local\Programs\Python\Python310\python.exe "C:/Program Files/JetBrains/PyCharm Community Edition 2023.1.4/plugins/python-ce/helpers/pycharm/_jb_unittest_runner.py" --path E:\GItHub\ply\tests\testlex.py 
> Testing started at 14:43 ...
> Launching unittests with arguments python -m unittest E:\GItHub\ply\tests\testlex.py in E:\GItHub\ply\tests
> 
> 
> Test ignored.
> AttributeError: '_io.TextIOWrapper' object has no attribute 'getvalue'
> 
> Process finished with exit code 1
> ```

作用：报告运行时的参数给运行者。

```python
# getcwd resolves symlinks, but PWD is not supported by some shells
PROJECT_DIR = os.getenv('PWD', os.getcwd())

def jb_doc_args(framework_name, args):
    """
    Runner encouraged to report its arguments to user with aid of this function

    """
    print("Launching {0} with arguments {1} in {2}\n".format(framework_name,
                                                             " ".join(args),
                                                             PROJECT_DIR))
```

`_jb_unittest_runner.py`：

大致流程：

- 1、解析传入的参数。

> 参数的作用：
>
> - `--path E:\GItHub\ply\tests\bs_tests.py` ：当点击 `if __name__ == '__main__':`的按钮时，则传入这个参数。直接运行文件。
> - `--target bs_tests.DictTest`：当点击 `class DictTest(unittest.TestCase):`的按钮时，则传入。直接运行运行这个测试用例。
> - `--target bs_tests.DictTest.test_dict_const`，同理，只运行一个测试项

- 2、报告运行时的参数
- 3、将当前目录（working dir）放入系统路径，这是unittest通过命令行运行的等效原理。
- 4、运行main函数，传入参数。

```python
# coding=utf-8
import os
import sys
from unittest import main

from _jb_runner_tools import jb_start_tests, jb_doc_args, JB_DISABLE_BUFFERING, \
    PROJECT_DIR, jb_finish_tests
from teamcity import unittestpy

if __name__ == '__main__':
    path, targets, additional_args = jb_start_tests()

    args = ["python -m unittest"]
    if path:
        assert os.path.exists(path), "{0}: No such file or directory".format(path)
        if sys.version_info > (3, 0) and os.path.isfile(path):
            # In Py3 it is possible to run script directly which is much more stable than discovery machinery
            # For example it supports hyphens in file names PY-23549
            additional_args = [path] + additional_args
        else:
            discovery_args = ["discover", "-s"]
            # Unittest in py2 does not support running script directly (and folders in py2 and py3),
            # but it can use "discover" to find all tests in some folder (optionally filtering by script)
            if os.path.isfile(path):
                discovery_args += [os.path.dirname(path), "-p", os.path.basename(path)]
            else:
                discovery_args.append(path)
            discovery_args += ["-t",
                               PROJECT_DIR]  # To force unit calculate path relative to this folder
            additional_args = discovery_args + additional_args
    elif targets:
        additional_args += targets
    args += additional_args
    jb_doc_args("unittests", args)
    # Working dir should be on path, that is how unittest work when launched from command line
    sys.path.insert(0, PROJECT_DIR)
    try:
        sys.exit(main(argv=args, module=None, testRunner=unittestpy.TeamcityTestRunner,
                      buffer=not JB_DISABLE_BUFFERING))
    finally:
        jb_finish_tests()
```

